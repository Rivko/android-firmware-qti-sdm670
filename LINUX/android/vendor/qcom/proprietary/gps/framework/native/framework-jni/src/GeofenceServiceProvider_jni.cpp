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
 * Copyright (C) 2008 The Android Open Source Project
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
#define LOG_TAG "LocSvc_HIDL_GeofenceSvcProvider_jni"
#define LOG_NDEBUG 0
#define LOCATION_CLASS_NAME "android/location/Location"

#include <inttypes.h>
#include "JNIHelp.h"
#include "jni.h"
#include "utils_jni.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/Log.h"

#include <vendor/qti/gnss/1.0/ILocHidlGnss.h>
#include <vendor/qti/gnss/1.0/ILocHidlGeofenceServiceCallback.h>

#define LOC_GPS_GEOFENCE_ENTERED     (1<<0L)
#define LOC_GPS_GEOFENCE_EXITED      (1<<1L)
#define LOC_GPS_GEOFENCE_UNCERTAIN   (1<<2L)

#define LOC_GPS_GEOFENCE_UNAVAILABLE (1<<0L)
#define LOC_GPS_GEOFENCE_AVAILABLE   (1<<1L)

#define LOC_GPS_GEOFENCE_OPERATION_SUCCESS           0
#define LOC_GPS_GEOFENCE_ERROR_TOO_MANY_GEOFENCES -100
#define LOC_GPS_GEOFENCE_ERROR_ID_EXISTS          -101
#define LOC_GPS_GEOFENCE_ERROR_ID_UNKNOWN         -102
#define LOC_GPS_GEOFENCE_ERROR_INVALID_TRANSITION -103
#define LOC_GPS_GEOFENCE_ERROR_GENERIC            -149

using ::vendor::qti::gnss::V1_0::ILocHidlGnss;
using ::vendor::qti::gnss::V1_0::ILocHidlGeofenceService;
using ::vendor::qti::gnss::V1_0::ILocHidlGeofenceServiceCallback;
using ::vendor::qti::gnss::V1_0::LocHidlLocation;
using ::vendor::qti::gnss::V1_0::LocHidlLocationFlagsBits;
using ::vendor::qti::gnss::V1_0::LocHidlGeofenceBreachType;
using ::vendor::qti::gnss::V1_0::LocHidlGeofenceStatus;
using ::vendor::qti::gnss::V1_0::LocHidlLocationError;

using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_death_recipient;
using ::android::hidl::base::V1_0::IBase;
using ::android::sp;

using namespace android;

static jobject sCallbacksObj = NULL;

static int32_t GEOFENCE_REQUEST_TYPE_ADD = 1;
static int32_t GEOFENCE_REQUEST_TYPE_REMOVE = 2;
static int32_t GEOFENCE_REQUEST_TYPE_PAUSE = 3;
static int32_t GEOFENCE_REQUEST_TYPE_RESUME = 4;

static jmethodID method_reportGeofenceTransition = NULL;
static jmethodID method_reportGeofenceRequestStatus = NULL;
static jmethodID method_reportGeofenceEngineStatus = NULL;

static void checkAndClearExceptionFromCallback(JNIEnv* env, const char* methodName) {
    if (env->ExceptionCheck()) {
        ALOGE("An exception was thrown by callback '%s'.", methodName);
        LOGE_EX(env);
        env->ExceptionClear();
    }
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
    locationObject = env->NewObject(locationClass, locationCtor, NULL);
    jint flags = location->locationFlagsMask;

    if (flags & LocHidlLocationFlagsBits::LAT_LONG_BIT) {
        jmethodID setLatitude = env->GetMethodID(locationClass,
                                                 "setLatitude",
                                                 "(D)V");
        env->CallVoidMethod(locationObject, setLatitude, location->latitude);

        jmethodID setLongitude = env->GetMethodID(locationClass,
                                                  "setLongitude",
                                                  "(D)V");
        env->CallVoidMethod(locationObject, setLongitude, location->longitude);

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
    env->DeleteLocalRef(locationClass);
}

/* ===========================================================
 *   HIDL Callbacks : ILocHidlGeofenceServiceCallback.hal
 * ===========================================================*/
struct GnssGeofenceServiceProviderCallback :
        public ILocHidlGeofenceServiceCallback {

    Return<void> gnssGeofenceBreachCb(
            int32_t count, const hidl_vec<int32_t>& idList,
            const LocHidlLocation& location, int32_t breachType,
            uint64_t timestamp) override;
    Return<void> gnssGeofenceStatusCb(
            int32_t statusAvailable, int32_t locTechType) override;
    Return<void> gnssAddGeofencesCb(
            int32_t count, const hidl_vec<int32_t>& locationErrorList,
            const hidl_vec<int32_t>& idList) override;
    Return<void> gnssRemoveGeofencesCb(
            int32_t count, const hidl_vec<int32_t>& locationErrorList,
            const hidl_vec<int32_t>& idList) override;
    Return<void> gnssPauseGeofencesCb(
            int32_t count, const hidl_vec<int32_t>& locationErrorList,
            const hidl_vec<int32_t>& idList) override;
    Return<void> gnssResumeGeofencesCb(
            int32_t count, const hidl_vec<int32_t>& locationErrorList,
            const hidl_vec<int32_t>& idList) override;
};

Return<void> GnssGeofenceServiceProviderCallback::gnssGeofenceBreachCb(
        int32_t count, const hidl_vec<int32_t>& idList,
        const LocHidlLocation& location, int32_t breachType,
        uint64_t /*timestamp*/) {

    FROM_HIDL_SERVICE();
    JNIEnv* env = getCallbackEnv();
    if (env == NULL) {
        ALOGE("JNIEnv NULL");
        return Void();
    }

    for (int32_t i = 0; i < count; i++) {
        uint32_t transition;
        if (breachType == (int32_t)LocHidlGeofenceBreachType::ENTER) {
            transition = LOC_GPS_GEOFENCE_ENTERED;
        } else if (breachType == (int32_t)LocHidlGeofenceBreachType::EXIT) {
            transition = LOC_GPS_GEOFENCE_EXITED;
        } else {
            // continue with other breach if transition is
            // nether LOC_GPS_GEOFENCE_ENTERED nor LOC_GPS_GEOFENCE_EXITED
            continue;
        }

        jobject locationObject = NULL;
        TranslateToObject(env, &location, locationObject);

        env->CallVoidMethod(sCallbacksObj,
                method_reportGeofenceTransition,
                idList[i],
                transition,
                locationObject);
        checkAndClearExceptionFromCallback(env, __FUNCTION__);
    }

    return Void();
}

Return<void> GnssGeofenceServiceProviderCallback::gnssGeofenceStatusCb(
        int32_t statusAvailable, int32_t /*locTechType*/) {

    FROM_HIDL_SERVICE();
    JNIEnv* env = getCallbackEnv();
    if (env == NULL) {
        ALOGE("JNIEnv NULL");
        return Void();
    }

    jobject locationObject = NULL;
    int32_t status = LOC_GPS_GEOFENCE_UNAVAILABLE;
    if (statusAvailable == (int32_t)LocHidlGeofenceStatus::AVAILABILE) {
        status = LOC_GPS_GEOFENCE_AVAILABLE;
    }

    env->CallVoidMethod(sCallbacksObj,
                        method_reportGeofenceEngineStatus,
                        status,
                        locationObject);
    checkAndClearExceptionFromCallback(env, __FUNCTION__);

    return Void();
}

Return<void> GnssGeofenceServiceProviderCallback::gnssAddGeofencesCb(
        int32_t count, const hidl_vec<int32_t>& locationErrorList,
        const hidl_vec<int32_t>& idList) {

    FROM_HIDL_SERVICE();
    JNIEnv* env = getCallbackEnv();
    if (env == NULL) {
        ALOGE("JNIEnv NULL");
        return Void();
    }

    for (int32_t i = 0; i < count; i++) {
        int32_t status = LOC_GPS_GEOFENCE_ERROR_GENERIC;
        if (locationErrorList[i] == (int32_t)LocHidlLocationError::SUCCESS)
            status = LOC_GPS_GEOFENCE_OPERATION_SUCCESS;
        env->CallVoidMethod(sCallbacksObj,
                method_reportGeofenceRequestStatus,
                GEOFENCE_REQUEST_TYPE_ADD,
                idList[i],
                status);
        checkAndClearExceptionFromCallback(env, __FUNCTION__);
    }

    return Void();
}

Return<void> GnssGeofenceServiceProviderCallback::gnssRemoveGeofencesCb(
        int32_t /*count*/, const hidl_vec<int32_t>& /*locationErrorList*/,
        const hidl_vec<int32_t>& /*idList*/) {

    return Void();
}

Return<void> GnssGeofenceServiceProviderCallback::gnssPauseGeofencesCb(
        int32_t count, const hidl_vec<int32_t>& locationErrorList,
        const hidl_vec<int32_t>& idList) {

    FROM_HIDL_SERVICE();
    JNIEnv* env = getCallbackEnv();
    if (env == NULL) {
        ALOGE("JNIEnv NULL");
        return Void();
    }

    for (int32_t i = 0; i < count; i++) {
        int32_t status = LOC_GPS_GEOFENCE_ERROR_GENERIC;
        if (locationErrorList[i] == (int32_t)LocHidlLocationError::SUCCESS)
            status = LOC_GPS_GEOFENCE_OPERATION_SUCCESS;
        else if (locationErrorList[i] == (int32_t)LocHidlLocationError::ID_UNKNOWN)
            status = LOC_GPS_GEOFENCE_ERROR_ID_UNKNOWN;
        env->CallVoidMethod(sCallbacksObj,
                method_reportGeofenceRequestStatus,
                GEOFENCE_REQUEST_TYPE_RESUME,
                idList[i],
                status);
        checkAndClearExceptionFromCallback(env, __FUNCTION__);
    }

    return Void();
}

Return<void> GnssGeofenceServiceProviderCallback::gnssResumeGeofencesCb(
        int32_t count, const hidl_vec<int32_t>& locationErrorList,
        const hidl_vec<int32_t>& idList) {

    FROM_HIDL_SERVICE();
    JNIEnv* env = getCallbackEnv();
    if (env == NULL) {
        ALOGE("JNIEnv NULL");
        return Void();
    }

    for (int32_t i = 0; i < count; i++) {
        int32_t status = LOC_GPS_GEOFENCE_ERROR_GENERIC;
        if (locationErrorList[i] == (int32_t)LocHidlLocationError::SUCCESS)
            status = LOC_GPS_GEOFENCE_OPERATION_SUCCESS;
        else if (locationErrorList[i] == (int32_t)LocHidlLocationError::ID_UNKNOWN)
            status = LOC_GPS_GEOFENCE_ERROR_ID_UNKNOWN;
        env->CallVoidMethod(sCallbacksObj,
                method_reportGeofenceRequestStatus,
                GEOFENCE_REQUEST_TYPE_PAUSE,
                idList[i],
                status);
        checkAndClearExceptionFromCallback(env, __FUNCTION__);
    }

    return Void();
}

/* ===================================================================
 *   HIDL Interface
 * ===================================================================*/
static sp<ILocHidlGeofenceService> gnssGeofenceServiceProviderIface = nullptr;
static sp<ILocHidlGeofenceServiceCallback> gnssGeofenceServiceProviderCb = nullptr;

struct LocHidlDeathRecipient : virtual public hidl_death_recipient
{
    // hidl_death_recipient interface
    virtual void serviceDied(uint64_t cookie, const wp<IBase>& /*who*/) override {
        gnssGeofenceServiceProviderIface = nullptr;
        ALOGE("%s] ILocHidlGeofenceService died with cookie: %" PRIu64, __FILE__, cookie);
    }
};
static sp<LocHidlDeathRecipient> locHidlDeathRecipient = nullptr;

static sp<ILocHidlGeofenceService>& getGeofenceServiceIface() {
    static uint64_t deathCount = 0;
    if (gnssGeofenceServiceProviderIface == nullptr) {
        auto gnssVendorHal = ILocHidlGnss::getService(GNSS_VENDOR_SERVICE_INSTANCE);
        if (gnssVendorHal == nullptr) {
            ALOGE("Unable to get GnssVendor service\n");
        } else {
            ALOGD("Got GnssVendor Service.");

            auto geofence = gnssVendorHal->getExtensionLocHidlGeofenceService();

            if (!geofence.isOk()) {
                ALOGE("Unable to get a handle to GeofenceServiceProvider extension !");
            } else {
                ALOGD("Got GnssGeofenceServiceProvider Extension.");
                if (locHidlDeathRecipient == nullptr) {
                    locHidlDeathRecipient = new LocHidlDeathRecipient();
                }
                gnssGeofenceServiceProviderIface = geofence;
                auto r = gnssGeofenceServiceProviderIface->linkToDeath(locHidlDeathRecipient,
                        deathCount++);
                if (!r.isOk() || r == false) {
                    ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
                    gnssGeofenceServiceProviderIface = nullptr;
                }
            }
        }
    }

    return gnssGeofenceServiceProviderIface;
}

static void GeofenceServiceProvider_class_init(JNIEnv* env, jclass clazz)
{
    ALOGD("GeofenceServiceProvider_class_init");

    // get references to the Java methods
    method_reportGeofenceTransition = env->GetMethodID(clazz,
                                            "reportGeofenceTransition",
                                            "(IILandroid/location/Location;)V");

    method_reportGeofenceRequestStatus = env->GetMethodID(clazz,
                                              "reportGeofenceRequestStatus",
                                              "(III)V");

    method_reportGeofenceEngineStatus = env->GetMethodID(clazz,
                                             "reportGeofenceStatus",
                                             "(ILandroid/location/Location;)V");

    getGeofenceServiceIface();
}

static void GeofenceServiceProvider_init(JNIEnv* env, jobject obj)
{
    ALOGD("GeofenceServiceProvider_init");

    auto iface = getGeofenceServiceIface();
    if (iface == nullptr) {
        ALOGE("Geofence Iface NULL !");
        return;
    }

    if (sCallbacksObj == NULL) {
        sCallbacksObj = env->NewGlobalRef(obj);
    }
    if (gnssGeofenceServiceProviderCb == nullptr) {
        gnssGeofenceServiceProviderCb =
                new GnssGeofenceServiceProviderCallback();
    }

    TO_HIDL_SERVICE();
    bool success = false;
    auto r = iface->init(gnssGeofenceServiceProviderCb);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    } else {
        success = r;
    }
    if (!success) {
        ALOGE("GnssGeofenceServiceProvider init failed !");
        return;
    }

    ALOGD("GnssGeofenceServiceProvider init success.");
}

static void GeofenceServiceProvider_add_geofence(JNIEnv* /*env*/,
                                                 jobject /*obj*/,
                                                 jint geofence_id,
                                                 jdouble latitude,
                                                 jdouble longitude,
                                                 jdouble radius_meters,
                                                 jint transition_types,
                                                 jint notification_responsiveness_ms,
                                                 jint confidence,
                                                 jint dwell_time_s,
                                                 jint dwell_time_mask) {

    ALOGD("%s", __FUNCTION__);

    auto iface = getGeofenceServiceIface();
    if (iface == nullptr) {
        ALOGE("Geofence Iface NULL !");
        return;
    }

    TO_HIDL_SERVICE();
    auto r = iface->addGeofence(
            geofence_id, latitude, longitude, radius_meters, transition_types,
            notification_responsiveness_ms, confidence, dwell_time_s, dwell_time_mask);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
}

static void GeofenceServiceProvider_remove_geofence(JNIEnv* /*env*/,
                                                    jobject /*obj*/,
                                                    jint geofence_id) {

    ALOGD("%s", __FUNCTION__);

    auto iface = getGeofenceServiceIface();
    if (iface == nullptr) {
        ALOGE("Geofence Iface NULL !");
        return;
    }

    TO_HIDL_SERVICE();
    auto r = iface->removeGeofence(geofence_id);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
}

static void GeofenceServiceProvider_update_geofence(JNIEnv* /*env*/,
                                                    jobject /*obj*/,
                                                    jint geofence_id,
                                                    jint transition_type,
                                                    jint notification_responsiveness_ms) {
    ALOGD("%s", __FUNCTION__);

    auto iface = getGeofenceServiceIface();
    if (iface == nullptr) {
        ALOGE("Geofence Iface NULL !");
        return;
    }

    TO_HIDL_SERVICE();
    auto r = iface->updateGeofence(
            geofence_id, transition_type, notification_responsiveness_ms);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
}

static void GeofenceServiceProvider_pause_geofence(JNIEnv* /*env*/,
                                                   jobject /*obj*/,
                                                   jint geofence_id) {

    ALOGD("%s", __FUNCTION__);

    auto iface = getGeofenceServiceIface();
    if (iface == nullptr) {
        ALOGE("Geofence Iface NULL !");
        return;
    }

    TO_HIDL_SERVICE();
    auto r = iface->pauseGeofence(geofence_id);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
}

static void GeofenceServiceProvider_resume_geofence(JNIEnv* /*env*/,
                                                    jobject /*obj*/,
                                                    jint geofence_id,
                                                    jint transition_type) {

    ALOGD("%s", __FUNCTION__);

    auto iface = getGeofenceServiceIface();
    if (iface == nullptr) {
        ALOGE("Geofence Iface NULL !");
        return;
    }

    TO_HIDL_SERVICE();
    auto r = iface->resumeGeofence(
            geofence_id, transition_type);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
}

static JNINativeMethod sMethods[] = {
    /* name, signature, funcPtr */
    {"native_geofence_class_init",
     "()V",
     reinterpret_cast<void*>(GeofenceServiceProvider_class_init)},
    {"native_geofence_init",
     "()V",
     reinterpret_cast<void*>(GeofenceServiceProvider_init)},
    {"native_add_geofence",
     "(IDDDIIIII)V",
     reinterpret_cast<void*>(GeofenceServiceProvider_add_geofence)},
    {"native_remove_geofence",
     "(I)V",
     reinterpret_cast<void*>(GeofenceServiceProvider_remove_geofence)},
    {"native_update_geofence",
     "(III)V",
     reinterpret_cast<void*>(GeofenceServiceProvider_update_geofence)},
    {"native_pause_geofence",
     "(I)V",
     reinterpret_cast<void*>(GeofenceServiceProvider_pause_geofence)},
    {"native_resume_geofence",
     "(II)V",
     reinterpret_cast<void*>(GeofenceServiceProvider_resume_geofence)}
};

int register_GeofenceServiceProvider(JNIEnv* env)
{
    ALOGD("register_GeofenceServiceProvider");
    return jniRegisterNativeMethods(env,
                                    "com/qualcomm/location/izat/geofence/GeofenceServiceProvider",
                                    sMethods,
                                    NELEM(sMethods));
}
