/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the license at
 *
 *      http://www.apache.org/license/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the license.
 */
#define LOG_TAG "LocSvc_HIDL_FlpSvcProvider_jni"
#define LOG_NDEBUG 0
#define LOCATION_CLASS_NAME "android/location/Location"

#ifndef FLP_SUCCESS
#define FLP_SUCCESS 0
#endif
#ifndef FLP_EROR
#define FLP_ERROR 1
#endif

#include <inttypes.h>
#include "JNIHelp.h"
#include "jni.h"
#include "utils_jni.h"
#include "hardware_legacy/power.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/Log.h"

#if defined LOC_HIDL_VERSION_1_0
#include <vendor/qti/gnss/1.0/ILocHidlGnss.h>
#else
#include <vendor/qti/gnss/1.1/ILocHidlGnss.h>
#endif
#include <vendor/qti/gnss/1.0/ILocHidlFlpServiceCallback.h>

#define LOCATION_REPORT_ON_FULL_INDICATION     0x0000001
#define LOCATION_REPORT_ON_FIX_INDICATION      0x0000002
#define LOCATION_REPORT_ON_QUERY               0x0000004
#define LOCATION_REPORT_ON_INDICATIONS         0x0000008

#if defined LOC_HIDL_VERSION_1_0
using ::vendor::qti::gnss::V1_0::ILocHidlGnss;
using ::vendor::qti::gnss::V1_0::ILocHidlFlpService;
#else
using ::vendor::qti::gnss::V1_1::ILocHidlGnss;
using ::vendor::qti::gnss::V1_1::ILocHidlFlpService;
#endif
using ::vendor::qti::gnss::V1_0::ILocHidlFlpServiceCallback;
using ::vendor::qti::gnss::V1_0::LocHidlLocation;
using ::vendor::qti::gnss::V1_0::LocHidlLocationFlagsBits;
using ::vendor::qti::gnss::V1_0::LocHidlBatchMode;
using ::vendor::qti::gnss::V1_0::LocHidlBatchStatus;
using ::vendor::qti::gnss::V1_0::LocHidlBatchOptions;
using ::vendor::qti::gnss::V1_0::LocHidlBatchStatusInfo;


using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_death_recipient;
using ::android::hidl::base::V1_0::IBase;
using ::android::sp;

using namespace android;

static jobject sCallbacksObj = NULL;
static jmethodID sOnLocationReport = NULL;
static jmethodID sOnBatchingStatusChange = NULL;
static jmethodID sOnMaxPowerAllocatedChanged = NULL;

static inline void CheckExceptions(JNIEnv* env, const char* methodName) {

    if(!env->ExceptionCheck()) {
        return;
    }
    ALOGE("An exception was thrown by '%s'.", methodName);
    LOGE_EX(env);
    env->ExceptionClear();
}

/* =================================================================
 *   HIDL Data Item Translation APIs
 * =================================================================*/
static void TranslateToObject(
        JNIEnv* env, const LocHidlLocation* location, jobject& locationObject) {

    jclass locationClass = env->FindClass(LOCATION_CLASS_NAME);
    jmethodID locationCtor = env->GetMethodID(locationClass,
                                                       "<init>",
                                                       "(Ljava/lang/String;)V");
    // the provider is set in the upper JVM layer
    locationObject = env->NewObject(locationClass, locationCtor, NULL);
    jint flags = location->locationFlagsMask;

    // set the valid information in the object
    if (flags & LocHidlLocationFlagsBits::LAT_LONG_BIT) {
        jmethodID setLatitude = env->GetMethodID(locationClass,
                                                          "setLatitude",
                                                          "(D)V");
        env->CallVoidMethod(locationObject, setLatitude, location->latitude);

        jmethodID setLongitude = env->GetMethodID(locationClass,
                                                           "setLongitude",
                                                           "(D)V");
        env->CallVoidMethod( locationObject, setLongitude, location->longitude);

        jmethodID setTime = env->GetMethodID(locationClass,
                                                      "setTime",
                                                      "(J)V");
        env->CallVoidMethod(locationObject, setTime, location->timestamp);
    }

    if (flags & LocHidlLocationFlagsBits::ALTITUDE_BIT) {
        jmethodID setAltitude = env->GetMethodID(locationClass,
                                                          "setAltitude",
                                                          "(D)V");
        env->CallVoidMethod(locationObject, setAltitude, location->altitude);
    }

    if (flags & LocHidlLocationFlagsBits::SPEED_BIT) {
        jmethodID setSpeed = env->GetMethodID(locationClass,
                                                       "setSpeed",
                                                       "(F)V");
        env->CallVoidMethod(locationObject, setSpeed, location->speed);
    }

    if (flags & LocHidlLocationFlagsBits::BEARING_BIT) {
        jmethodID setBearing = env->GetMethodID(locationClass,
                                                         "setBearing",
                                                         "(F)V");
        env->CallVoidMethod(locationObject, setBearing, location->bearing);
    }

    if (flags & LocHidlLocationFlagsBits::ACCURACY_BIT) {
        jmethodID setAccuracy = env->GetMethodID(locationClass,
                                                          "setAccuracy",
                                                          "(F)V");
        env->CallVoidMethod(locationObject, setAccuracy, location->accuracy);
    }

    if (flags & LocHidlLocationFlagsBits::VERTICAL_ACCURACY_BIT) {
        jmethodID setVerticalAccuracy = env->GetMethodID(locationClass,
                                                            "setVerticalAccuracyMeters",
                                                            "(F)V");
        env->CallVoidMethod(locationObject, setVerticalAccuracy, location->verticalAccuracy);
    }

    if (flags & LocHidlLocationFlagsBits::SPEED_ACCURACY_BIT) {
        jmethodID setSpeedAccuracy = env->GetMethodID(locationClass,
                                                            "setSpeedAccuracyMetersPerSecond",
                                                            "(F)V");
        env->CallVoidMethod(locationObject, setSpeedAccuracy, location->speedAccuracy);
    }

    if (flags & LocHidlLocationFlagsBits::BEARING_ACCURACY_BIT) {
        jmethodID setBearingAccuracy = env->GetMethodID(locationClass,
                                                            "setBearingAccuracyDegrees",
                                                            "(F)V");
        env->CallVoidMethod(locationObject, setBearingAccuracy, location->bearingAccuracy);
    }
    env->DeleteLocalRef(locationClass);
}

static void TranslateToObjectArray(JNIEnv* env, int32_t locationsCount,
                                   const LocHidlLocation* gnssLocations,
                                   jobjectArray& locationsArray) {

    jclass locationClass = env->FindClass(LOCATION_CLASS_NAME);
    locationsArray = env->NewObjectArray(locationsCount,
                                                  locationClass,
                                                  NULL);
    for (int i = 0; i < locationsCount; ++i) {
        jobject locationObject = NULL;
        TranslateToObject(env, gnssLocations + i, locationObject);
        env->SetObjectArrayElement(locationsArray, i, locationObject);
        env->DeleteLocalRef(locationObject);
    }
    env->DeleteLocalRef(locationClass);
}

static void TranslateVectorToObjectArray(
        JNIEnv* env, const hidl_vec<LocHidlLocation>& gnssLocations,
        jobjectArray& locationsArray) {

    int32_t locationsCount = gnssLocations.size();

    jclass locationClass = env->FindClass(LOCATION_CLASS_NAME);
    locationsArray = env->NewObjectArray(locationsCount,
                                                  locationClass,
                                                  NULL);
    for (int i = 0; i < locationsCount; ++i) {
        jobject locationObject = NULL;
        TranslateToObject(env, &gnssLocations[i], locationObject);
        env->SetObjectArrayElement(locationsArray, i, locationObject);
        env->DeleteLocalRef(locationObject);
    }
    env->DeleteLocalRef(locationClass);
}

/* ===========================================================
 *   HIDL Callbacks : ILocHidlFlpServiceCallback.hal
 * ===========================================================*/
struct GnssFlpServiceProviderCallback : public ILocHidlFlpServiceCallback {

    Return<void> gnssLocationTrackingCb(
      const LocHidlLocation& gnssLocation) override;

    Return<void> gnssLocationBatchingCb(
      const LocHidlBatchOptions &batchOptions,
      const hidl_vec<LocHidlLocation>& gnssLocations) override;

    Return<void> gnssBatchingStatusCb(
            const LocHidlBatchStatusInfo &batchStatus,
            const hidl_vec<uint32_t>& listOfCompletedTrips) override;

    Return<void> gnssMaxPowerAllocatedCb(int32_t powerInMW) override;
};

Return<void> GnssFlpServiceProviderCallback::gnssLocationTrackingCb(
        const LocHidlLocation& gnssLocation) {

    FROM_HIDL_SERVICE();
    JNIEnv* env = getCallbackEnv();
    if (env == NULL) {
        ALOGE("JNIEnv NULL");
        return Void();
    }

    int reportTrigger = LOCATION_REPORT_ON_FIX_INDICATION;
    jobjectArray locationsArray = NULL;
    TranslateToObjectArray(env, 1, &gnssLocation, locationsArray);

    env->CallVoidMethod(
        sCallbacksObj,
        sOnLocationReport,
        locationsArray,
        reportTrigger,
        LocHidlBatchMode::BATCH_MODE_NONE);
    CheckExceptions(env, __FUNCTION__);

    if(locationsArray != NULL) {
        env->DeleteLocalRef(locationsArray);
    }

    return Void();
}

Return<void> GnssFlpServiceProviderCallback::gnssLocationBatchingCb(
        const LocHidlBatchOptions &batchOptions,
        const hidl_vec<LocHidlLocation>& gnssLocations) {

    FROM_HIDL_SERVICE();
    JNIEnv* env = getCallbackEnv();
    if (env == NULL) {
        ALOGE("JNIEnv NULL");
        return Void();
    }

    uint32_t count = gnssLocations.size();
    if(count == 0) {
        ALOGE("Location Count = 0");
        return Void();
    }

    int reportTrigger = LOCATION_REPORT_ON_FULL_INDICATION;
    jobjectArray locationsArray = NULL;
    TranslateVectorToObjectArray(env, gnssLocations, locationsArray);

    env->CallVoidMethod(
        sCallbacksObj,
        sOnLocationReport,
        locationsArray,
        reportTrigger,
        batchOptions.batchMode);
    CheckExceptions(env, __FUNCTION__);

    if(locationsArray != NULL) {
        env->DeleteLocalRef(locationsArray);
    }

    return Void();
}

Return<void> GnssFlpServiceProviderCallback::gnssBatchingStatusCb(
        const LocHidlBatchStatusInfo &batchStatusInfo,
        const hidl_vec<uint32_t>& listOfCompletedTrips) {

    FROM_HIDL_SERVICE();
    JNIEnv* env = getCallbackEnv();
    if (env == NULL) {
        ALOGE("JNIEnv NULL");
        return Void();
    }

    jintArray tripCompletedArray = NULL;

    ALOGD("BatchingStatus = %d Number of trips completed = %zu",
            batchStatusInfo.batchStatus, listOfCompletedTrips.size());

    if (batchStatusInfo.batchStatus == LocHidlBatchStatus::BATCH_STATUS_TRIP_COMPLETED) {
        int listSize = listOfCompletedTrips.size();
        if (listSize == 0) {
            return Void();
        }

        tripCompletedArray = (jintArray)env->NewIntArray(listSize);
        if (tripCompletedArray == NULL) {
            ALOGD("Failed to allocate array for completed trip ids's");
            return Void();
        }

        for (size_t index = 0; index < listOfCompletedTrips.size(); index++) {
            jint tripClientId = listOfCompletedTrips[index];
            env->SetIntArrayRegion(tripCompletedArray, (int)index, 1, &tripClientId);
        }
    }

    env->CallVoidMethod(sCallbacksObj,
                        sOnBatchingStatusChange,
                        batchStatusInfo.batchStatus,
                        tripCompletedArray);
    CheckExceptions(env, __FUNCTION__);

    if(tripCompletedArray!= NULL) {
        env->DeleteLocalRef(tripCompletedArray);
    }

    return Void();
}

Return<void> GnssFlpServiceProviderCallback::gnssMaxPowerAllocatedCb(
        int32_t powerInMW) {

    FROM_HIDL_SERVICE();
    JNIEnv* env = getCallbackEnv();
    if (env == NULL) {
        ALOGE("JNIEnv NULL");
        return Void();
    }

    env->CallVoidMethod(
        sCallbacksObj,
        sOnMaxPowerAllocatedChanged,
        powerInMW);
    CheckExceptions(env, __FUNCTION__);

    return Void();
}

/* ===================================================================
 *   HIDL Interface
 * ===================================================================*/
static sp<ILocHidlFlpService> gnssFlpServiceProviderIface = nullptr;
static sp<ILocHidlFlpServiceCallback> gnssFlpServiceProviderCb = nullptr;

struct LocHidlDeathRecipient : virtual public hidl_death_recipient
{
    // hidl_death_recipient interface
    virtual void serviceDied(uint64_t cookie, const wp<IBase>& /*who*/) override {
        gnssFlpServiceProviderIface = nullptr;
        ALOGE("%s] ILocHidlFlpService died with cookie: %" PRIu64, __FILE__, cookie);
    }
};
static sp<LocHidlDeathRecipient> locHidlDeathRecipient = nullptr;

static sp<ILocHidlFlpService>& getFlpServiceIface() {
    static uint64_t deathCount = 0;
    if (gnssFlpServiceProviderIface == nullptr) {
        ALOGD("Get GnssVendor Service.");
        auto gnssVendorHal = ILocHidlGnss::getService(GNSS_VENDOR_SERVICE_INSTANCE);
        if (gnssVendorHal == nullptr) {
            ALOGE("Unable to get GnssVendor service\n");
        } else {
            ALOGD("Get FlpServiceProvider Extension");
#if defined LOC_HIDL_VERSION_1_0
            auto flp = gnssVendorHal->getExtensionLocHidlFlpService();
#else
            auto flp = gnssVendorHal->getExtensionLocHidlFlpService_1_1();
#endif
            if (!flp.isOk()) {
                ALOGE("Unable to get a handle to FlpServiceProvider extension !");
            } else {
                ALOGD("Got GnssFlpServiceProvider Extension.");
                if (locHidlDeathRecipient == nullptr) {
                    locHidlDeathRecipient = new LocHidlDeathRecipient();
                }
                gnssFlpServiceProviderIface = flp;
                auto r = gnssFlpServiceProviderIface->linkToDeath(locHidlDeathRecipient,
                        deathCount++);
                if (!r.isOk() || r == false) {
                    ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
                    gnssFlpServiceProviderIface = nullptr;
                }
            }
        }
    }

    return gnssFlpServiceProviderIface;
}

static void FlpServiceProvider_class_init(JNIEnv* env, jclass clazz)
{
    ALOGD("%s", __FUNCTION__);

    // get references to the Java methods
    sOnLocationReport = env->GetMethodID(clazz,
                                         "onLocationReport",
                                         "([Landroid/location/Location;II)V");
    sOnMaxPowerAllocatedChanged = env->GetMethodID(clazz,
                                                   "onMaxPowerAllocatedChanged",
                                                   "(I)V");
    sOnBatchingStatusChange = env->GetMethodID(clazz,
                                         "onBatchingStatusCb",
                                         "(I[I)V");

    getFlpServiceIface();
}

static jint FlpServiceProvider_init(JNIEnv* env, jobject obj)
{
    ALOGD("%s", __FUNCTION__);

    auto iface = getFlpServiceIface();
    if (iface == nullptr) {
        ALOGE("FLP Iface NULL !");
        return FLP_ERROR;
    }

    if (sCallbacksObj == NULL) {
        sCallbacksObj = env->NewGlobalRef(obj);
    }
    if (gnssFlpServiceProviderCb == nullptr) {
        gnssFlpServiceProviderCb = new GnssFlpServiceProviderCallback();
    }

    bool success = false;
    TO_HIDL_SERVICE();
    auto r = iface->init(gnssFlpServiceProviderCb);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    } else {
        success = r;
    }
    if (!success) {
        ALOGE("GnssFlpServiceProvider init failed !");
        return FLP_ERROR;
    }

    ALOGD("GnssFlpServiceProvider init success.");
    return FLP_SUCCESS;
}

static jint
FlpServiceProvider_get_all_supported_features(JNIEnv* /*env*/, jobject /*obj*/)
{
    ALOGD("%s", __FUNCTION__);

    auto iface = getFlpServiceIface();
    if (iface == nullptr) {
        ALOGE("FLP Iface NULL !");
        return FLP_ERROR;
    }

    TO_HIDL_SERVICE();
    uint32_t featureMask = 0;
    auto r = iface->getAllSupportedFeatures();
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    } else {
        featureMask = r;
    }
    ALOGD("Got feature mask: 0x%x", featureMask);
    return featureMask;
}

static int FlpServiceProvider_start_flp_session(JNIEnv* /*env*/, jobject /*object*/,
                                                jint id,
                                                jint flags,
                                                jlong period_ns,
                                                jint distance_ms,
                                                jlong trip_distance)
{
    ALOGD("%s", __FUNCTION__);

    auto iface = getFlpServiceIface();
    if (iface == nullptr) {
        ALOGE("FLP Iface NULL !");
        return FLP_ERROR;
    }

    TO_HIDL_SERVICE();
    int32_t result = -1;
    auto r = iface->startFlpSession(
            (int32_t)id,
            (uint32_t)flags,
            (int64_t)period_ns,
            (int32_t)distance_ms,
            (uint32_t)trip_distance);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    } else {
        result = r;
    }
    if (result != 0) {
        ALOGE("Start FLP Session failed, err %d !", result);
        return FLP_ERROR;
    }

    ALOGD("Start FLP Session success.");
    return FLP_SUCCESS;
}

static int FlpServiceProvider_update_flp_session(JNIEnv* /*env*/, jobject /*object*/,
                                                 jint id,
                                                 jint flags,
                                                 jlong period_ns,
                                                 jint distance_ms,
                                                 jlong trip_distance)
{
    ALOGD("%s", __FUNCTION__);

    auto iface = getFlpServiceIface();
    if (iface == nullptr) {
        ALOGE("FLP Iface NULL !");
        return FLP_ERROR;
    }

    TO_HIDL_SERVICE();
    int32_t result = -1;
    auto r = iface->updateFlpSession(
            (int32_t)id,
            (uint32_t)flags,
            (int64_t)period_ns,
            (int32_t)distance_ms,
            (uint32_t)trip_distance);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    } else {
        result = r;
    }
    if (result != 0) {
        ALOGE("Update FLP Session failed, err %d !", result);
        return FLP_ERROR;
    }

    ALOGD("Update FLP Session success.");
    return FLP_SUCCESS;
}

static int FlpServiceProvider_stop_flp_session(JNIEnv* /*env*/, jobject /*object*/, jint id)
{
    ALOGD("%s", __FUNCTION__);

    auto iface = getFlpServiceIface();
    if (iface == nullptr) {
        ALOGE("FLP Iface NULL !");
        return FLP_ERROR;
    }

    TO_HIDL_SERVICE();
    int32_t result = -1;
    auto r = iface->stopFlpSession(id);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    } else {
        result = r;
    }
    if (result != 0) {
        ALOGE("Stop FLP Session failed, err %d !", result);
        return FLP_ERROR;
    }

    ALOGD("Stop FLP Session success.");
    return FLP_SUCCESS;
}

static int FlpServiceProvider_get_all_locations(JNIEnv* /*env*/, jobject /*object*/,
        jint id)
{
    ALOGD("%s", __FUNCTION__);

    auto iface = getFlpServiceIface();
    if (iface == nullptr) {
        ALOGE("FLP Iface NULL !");
        return FLP_ERROR;
    }

    TO_HIDL_SERVICE();
    int32_t result = -1;
    auto r = iface->getAllBatchedLocations(id);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    } else {
        result = r;
    }
    if (result != 0) {
        ALOGE("Get all batched locations failed, err %d !", result);
        return FLP_ERROR;
    }

    ALOGD("Get all batched locations success.");
    return FLP_SUCCESS;
}

static void FlpServiceProvider_delete_aiding_data(JNIEnv* /*env*/, jobject /*object*/,
                                                  jlong flags)
{
    ALOGD("%s", __FUNCTION__);

    auto iface = getFlpServiceIface();
    if (iface == nullptr) {
        ALOGE("FLP Iface NULL !");
        return;
    }

    TO_HIDL_SERVICE();
    auto r = iface->deleteAidingData((int64_t)flags);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
}

static void FlpServiceProvider_update_xtra_throttle(JNIEnv* /*env*/, jobject /*object*/,
                                                  jboolean enabled)
{
    ALOGD("%s", __FUNCTION__);

#ifndef LOC_HIDL_VERSION_1_0
    auto iface = getFlpServiceIface();
    if (iface == nullptr) {
        ALOGE("FLP Iface NULL !");
        return;
    }

    TO_HIDL_SERVICE();
    auto r = iface->updateXtraThrottle((enabled==JNI_TRUE) ? true : false);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
#endif
}

static void FlpServiceProvider_get_max_power_allocated_in_mw(JNIEnv* /*env*/,
                                                             jobject /*object*/)
{
    ALOGD("%s", __FUNCTION__);

    auto iface = getFlpServiceIface();
    if (iface == nullptr) {
        ALOGE("FLP Iface NULL !");
        return;
    }

    TO_HIDL_SERVICE();
    auto r = iface->getMaxPowerAllocated();
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
}

static JNINativeMethod sMethods[] = {
     /* name, signature, funcPtr */
    {"native_flp_class_init",
     "()V",
     reinterpret_cast<void*>(FlpServiceProvider_class_init)},
    {"native_flp_init",
     "()I",
     reinterpret_cast<void*>(FlpServiceProvider_init)},
    {"native_flp_get_all_supported_features",
     "()I",
     reinterpret_cast<void*>(FlpServiceProvider_get_all_supported_features)},
    {"native_flp_get_all_locations",
     "(I)I",
     reinterpret_cast<void*>(FlpServiceProvider_get_all_locations)},
    {"native_flp_stop_session",
     "(I)I",
     reinterpret_cast<void*>(FlpServiceProvider_stop_flp_session)},
    {"native_flp_start_session",
     "(IIJIJ)I",
     reinterpret_cast<void*>(FlpServiceProvider_start_flp_session)},
    {"native_flp_update_session",
     "(IIJIJ)I",
     reinterpret_cast<void*>(FlpServiceProvider_update_flp_session)},
    {"native_flp_delete_aiding_data",
     "(J)V",
     reinterpret_cast<void*>(FlpServiceProvider_delete_aiding_data)},
    {"native_flp_update_xtra_throttle",
     "(Z)V",
     reinterpret_cast<void*>(FlpServiceProvider_update_xtra_throttle)},
    {"native_flp_get_max_power_allocated_in_mw",
     "()V",
     reinterpret_cast<void*>(FlpServiceProvider_get_max_power_allocated_in_mw)},
};

int register_FlpServiceProvider(JNIEnv* env)
{
    ALOGD("register_FlpServiceProvider");
    return jniRegisterNativeMethods(env,
                                    "com/qualcomm/location/izat/flp/FlpServiceProvider",
                                    sMethods,
                                    NELEM(sMethods));
}
