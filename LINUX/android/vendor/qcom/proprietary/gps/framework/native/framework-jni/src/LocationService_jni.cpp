/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

  Not a Contribution, Apache license notifications and
  license are retained for attribution purposes only.
=============================================================================*/

/*
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (c) 2011,2012, The Linux Foundation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "LocSvc_HIDL_LocationService_jni"
#define LOG_NDEBUG 0

#include <inttypes.h>
#include "JNIHelp.h"
#include "jni.h"
#include "utils_jni.h"
#include "utils/Log.h"
#include "utils/misc.h"
#include "android_runtime/AndroidRuntime.h"

#include <string.h>
#include <pthread.h>

#include <vendor/qti/gnss/1.0/ILocHidlGnss.h>
#include <vendor/qti/gnss/1.0/ILocHidlAGnss.h>
#include <vendor/qti/gnss/1.0/ILocHidlGnssNi.h>
#include <vendor/qti/gnss/1.0/ILocHidlGnssNiCallback.h>

typedef int16_t AGpsBearerType;
#define AGPS_APN_BEARER_INVALID    -1
#define AGPS_APN_BEARER_IPV4        0
#define AGPS_APN_BEARER_IPV6        1
#define AGPS_APN_BEARER_IPV4V6      2

typedef int LocGpsUserResponseType;
#define LOC_GPS_NI_RESPONSE_ACCEPT  1
#define LOC_GPS_NI_RESPONSE_DENY    2
#define LOC_GPS_NI_RESPONSE_NORESP  3

typedef uint32_t LocGpsNiType;
#define LOC_GPS_NI_TYPE_VOICE              1
#define LOC_GPS_NI_TYPE_UMTS_SUPL          2
#define LOC_GPS_NI_TYPE_UMTS_CTRL_PLANE    3
#define LOC_GPS_NI_TYPE_EMERGENCY_SUPL     4

typedef uint32_t LocGpsNiNotifyFlags;
#define LOC_GPS_NI_NEED_NOTIFY          0x0001
#define LOC_GPS_NI_NEED_VERIFY          0x0002
#define LOC_GPS_NI_PRIVACY_OVERRIDE     0x0004

typedef int LocGpsNiEncodingType;
#define LOC_GPS_ENC_NONE                   0
#define LOC_GPS_ENC_SUPL_GSM_DEFAULT       1
#define LOC_GPS_ENC_SUPL_UTF8              2
#define LOC_GPS_ENC_SUPL_UCS2              3
#define LOC_GPS_ENC_UNKNOWN               -1

using ::android::hardware::gnss::V1_0::IAGnss;
using ::android::hardware::gnss::V1_0::IAGnssCallback;
using ::android::hardware::gnss::V1_0::IGnssNiCallback;
using ::vendor::qti::gnss::V1_0::ILocHidlGnss;
using ::vendor::qti::gnss::V1_0::ILocHidlAGnss;
using ::vendor::qti::gnss::V1_0::ILocHidlGnssNi;
using ::vendor::qti::gnss::V1_0::ILocHidlGnssNiCallback;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_death_recipient;
using ::android::hidl::base::V1_0::IBase;
using ::android::sp;

using namespace android;

static jmethodID method_reportAGpsStatus;
static jmethodID method_setEngineCapabilities;
static jmethodID method_reportNiNotification;

static jobject mCallbacksObj = NULL;

static void checkAndClearExceptionFromCallback(JNIEnv* env, const char* methodName) {
    if (env->ExceptionCheck()) {
        ALOGE("An exception was thrown by callback '%s'.", methodName);
        env->ExceptionClear();
    }
}

/* =================================================================
 *   HIDL Data Item Translation APIs
 * =================================================================*/
static IAGnss::ApnIpType jniToHidl_BearerType(int bearerType) {

    if (bearerType == AGPS_APN_BEARER_IPV4)
        return IAGnss::ApnIpType::IPV4;
    else if (bearerType == AGPS_APN_BEARER_IPV6)
        return IAGnss::ApnIpType::IPV6;
    else if (bearerType == AGPS_APN_BEARER_IPV4V6)
        return IAGnss::ApnIpType::IPV4V6;
    else
        return IAGnss::ApnIpType::INVALID;
}

static IGnssNiCallback::GnssUserResponseType
jniToHidl_NiResponseType(LocGpsUserResponseType niResponse) {

    if (niResponse == LOC_GPS_NI_RESPONSE_ACCEPT)
        return IGnssNiCallback::GnssUserResponseType::RESPONSE_ACCEPT;
    else if (niResponse == LOC_GPS_NI_RESPONSE_DENY)
        return IGnssNiCallback::GnssUserResponseType::RESPONSE_DENY;
    else if (niResponse == LOC_GPS_NI_RESPONSE_NORESP)
        return IGnssNiCallback::GnssUserResponseType::RESPONSE_NORESP;
    else
        return static_cast<IGnssNiCallback::GnssUserResponseType>(0);
}

static LocGpsNiType hidlToJni_NiType(
        ILocHidlGnssNiCallback::GnssNiTypeExt niType) {

    if (niType == ILocHidlGnssNiCallback::GnssNiTypeExt::VOICE)
        return LOC_GPS_NI_TYPE_VOICE;
    else if (niType == ILocHidlGnssNiCallback::GnssNiTypeExt::UMTS_SUPL)
        return LOC_GPS_NI_TYPE_UMTS_SUPL;
    else if (niType == ILocHidlGnssNiCallback::GnssNiTypeExt::UMTS_CTRL_PLANE)
        return LOC_GPS_NI_TYPE_UMTS_CTRL_PLANE;
    else if (niType == ILocHidlGnssNiCallback::GnssNiTypeExt::EMERGENCY_SUPL)
        return LOC_GPS_NI_TYPE_EMERGENCY_SUPL;
    else
        return 0;
}

static LocGpsNiNotifyFlags hidlToJni_NotifyFlags(uint32_t notifyFlags) {

    LocGpsNiNotifyFlags flags = 0;
    if (notifyFlags &
            static_cast<uint32_t>(IGnssNiCallback::GnssNiNotifyFlags::NEED_NOTIFY))
        flags |= LOC_GPS_NI_NEED_NOTIFY;
    if (notifyFlags &
            static_cast<uint32_t>(IGnssNiCallback::GnssNiNotifyFlags::NEED_VERIFY))
        flags |= LOC_GPS_NI_NEED_VERIFY;
    if (notifyFlags &
            static_cast<uint32_t>(IGnssNiCallback::GnssNiNotifyFlags::PRIVACY_OVERRIDE))
        flags |= LOC_GPS_NI_PRIVACY_OVERRIDE;
    return flags;
}

static LocGpsUserResponseType hidlToJni_ResponseType(
        IGnssNiCallback::GnssUserResponseType responseType) {

    if (responseType == IGnssNiCallback::GnssUserResponseType::RESPONSE_ACCEPT)
        return LOC_GPS_NI_RESPONSE_ACCEPT;
    else if (responseType == IGnssNiCallback::GnssUserResponseType::RESPONSE_DENY)
        return LOC_GPS_NI_RESPONSE_DENY;
    else if (responseType == IGnssNiCallback::GnssUserResponseType::RESPONSE_NORESP)
        return LOC_GPS_NI_RESPONSE_NORESP;
    else
        return 0;
}

static LocGpsNiEncodingType hidlToJni_EncodingType(
        IGnssNiCallback::GnssNiEncodingType encodingType) {

    if (encodingType == IGnssNiCallback::GnssNiEncodingType::ENC_NONE)
        return LOC_GPS_ENC_NONE;
    else if (encodingType == IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_GSM_DEFAULT)
        return LOC_GPS_ENC_SUPL_GSM_DEFAULT;
    else if (encodingType == IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_UTF8)
        return LOC_GPS_ENC_SUPL_UTF8;
    else if (encodingType == IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_UCS2)
        return LOC_GPS_ENC_SUPL_UCS2;
    else
        return LOC_GPS_ENC_UNKNOWN;
}

/* =================================================
 *   HIDL Callbacks : IAGnssCallback.hal
 * =================================================*/
struct AGnssCallback : public IAGnssCallback {

    Return<void> agnssStatusIpV6Cb(
      const IAGnssCallback::AGnssStatusIpV6& agps_status) override;

    Return<void> agnssStatusIpV4Cb(
      const IAGnssCallback::AGnssStatusIpV4& agps_status) override;
};

Return<void> AGnssCallback::agnssStatusIpV6Cb(
        const IAGnssCallback::AGnssStatusIpV6&) {

    FROM_HIDL_SERVICE();
    ALOGE("%s] Not supported !", __FUNCTION__);
    return Void();
}

Return<void> AGnssCallback::agnssStatusIpV4Cb(
        const IAGnssCallback::AGnssStatusIpV4& agps_status) {

    FROM_HIDL_SERVICE();

    JNIEnv* env = getCallbackEnv();
    if (env == NULL) {
        ALOGE("JNIEnv NULL");
        return Void();
    }
    env->CallVoidMethod(mCallbacksObj,
            method_reportAGpsStatus,
            (int)agps_status.type, (int)agps_status.status);
    checkAndClearExceptionFromCallback(env, __FUNCTION__);

    return Void();
}

/* =================================================
 *   HIDL Callbacks : ILocHidlGnssNiCallback.hal
 * =================================================*/
struct GnssNiVendorCallback : public ILocHidlGnssNiCallback {

    Return<void> niNotifyCb(
            const IGnssNiCallback::GnssNiNotification& notification) override;
    Return<void> niNotifyCbExt(
            const ILocHidlGnssNiCallback::GnssNiNotificationExt& notification) override;

    Return<void> gnssCapabilitiesCb(uint32_t capabilitiesMask) override;
};

Return<void> GnssNiVendorCallback::niNotifyCb(
        const IGnssNiCallback::GnssNiNotification&) {

    FROM_HIDL_SERVICE();
    ALOGE("%s] Not supported !", __FUNCTION__);
    return Void();
}

Return<void> GnssNiVendorCallback::niNotifyCbExt(
        const ILocHidlGnssNiCallback::GnssNiNotificationExt& gnssNiNotification) {

    FROM_HIDL_SERVICE();
    JNIEnv* env = getCallbackEnv();
    if (env == NULL) {
        ALOGE("JNIEnv NULL");
        return Void();
    }

    uint32_t id = gnssNiNotification.notificationId;

    LocGpsNiType type = hidlToJni_NiType(gnssNiNotification.niType);

    LocGpsNiNotifyFlags flags = hidlToJni_NotifyFlags(
            static_cast<uint32_t>(gnssNiNotification.notifyFlags));

    LocGpsUserResponseType rsp =
            hidlToJni_ResponseType(gnssNiNotification.defaultResponse);

    jstring requestor_id = env->NewStringUTF(gnssNiNotification.requestorId.c_str());
    jstring text = env->NewStringUTF(gnssNiNotification.notificationMessage.c_str());
    jstring extras = env->NewStringUTF(gnssNiNotification.extras.c_str());

    LocGpsNiEncodingType requestor_id_encoding =
            hidlToJni_EncodingType(gnssNiNotification.requestorIdEncoding);

    LocGpsNiEncodingType text_encoding =
            hidlToJni_EncodingType(gnssNiNotification.notificationIdEncoding);

    if (requestor_id && text && extras) {
        env->CallVoidMethod(mCallbacksObj, method_reportNiNotification,
                id, type, flags, (int)gnssNiNotification.timeoutSec, rsp,
                requestor_id, text, requestor_id_encoding, text_encoding,
                extras, gnssNiNotification.esEnabled);
    } else {
        ALOGE("out of memory in gps_ni_notify\n");
    }

    if (requestor_id)
        env->DeleteLocalRef(requestor_id);
    if (text)
        env->DeleteLocalRef(text);
    if (extras)
        env->DeleteLocalRef(extras);
    checkAndClearExceptionFromCallback(env, __FUNCTION__);

    return Void();
}

Return<void> GnssNiVendorCallback::gnssCapabilitiesCb(
        uint32_t capabilitiesMask) {

    FROM_HIDL_SERVICE();
    JNIEnv* env = getCallbackEnv();
    if (env == NULL) {
        ALOGE("JNIEnv NULL");
        return Void();
    }

    env->CallVoidMethod(mCallbacksObj,
            method_setEngineCapabilities, capabilitiesMask);
    checkAndClearExceptionFromCallback(env, __FUNCTION__);

    return Void();
}

/* =================================================
 *   HIDL Interface
 * =================================================*/
static sp<ILocHidlGnss>   gnssVendorHal = nullptr;
static sp<ILocHidlGnssNi> gnssNiVendorIface = nullptr;
static sp<ILocHidlGnssNiCallback> gnssNiVendorCb = nullptr;
static sp<ILocHidlAGnss>  agnssVendorIface = nullptr;
static sp<IAGnssCallback> agnssCb = nullptr;

struct LocHidlDeathRecipient : virtual public hidl_death_recipient
{
    // hidl_death_recipient interface
    virtual void serviceDied(uint64_t cookie, const wp<IBase>& /*who*/) override {
        gnssVendorHal = nullptr;
        gnssNiVendorIface = nullptr;
        agnssVendorIface = nullptr;
        ALOGE("%s] ILocHidlGnss died with cookie: %" PRIu64, __FILE__, cookie);
    }
};
static sp<LocHidlDeathRecipient> locHidlDeathRecipient = nullptr;

static sp<ILocHidlGnss>& getVendorHal() {
    static uint64_t deathCount = 0;
    if (gnssVendorHal == nullptr) {
        auto vendorGnss = ILocHidlGnss::getService(GNSS_VENDOR_SERVICE_INSTANCE);
        if (vendorGnss == nullptr) {
            ALOGE("Unable to get GnssVendor service\n");
        } else {
            ALOGD("Got GnssVendor Service.");
            if (locHidlDeathRecipient == nullptr) {
                locHidlDeathRecipient = new LocHidlDeathRecipient();
            }
            gnssVendorHal = vendorGnss;
            auto r = gnssVendorHal->linkToDeath(locHidlDeathRecipient, deathCount++);
            if (!r.isOk() || r == false) {
                ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
                gnssVendorHal = nullptr;
            }
        }
    }
    return gnssVendorHal;
}

static sp<ILocHidlGnssNi>& getGnssNiVendorIface() {
    if (gnssNiVendorIface == nullptr) {
        auto vendorGnss = getVendorHal();
        if (vendorGnss == nullptr) {
            ALOGE("failed to get gnss vendor hal\n");
        } else {
            auto gnssNiVendor = vendorGnss->getExtensionLocHidlGnssNi();

            if (!gnssNiVendor.isOk()) {
                ALOGE("Unable to get a handle to GnssNiVendor extension !");
            } else {
                gnssNiVendorIface = gnssNiVendor;
            }
        }
    }
    return gnssNiVendorIface;
}

static sp<ILocHidlAGnss>& getAGnssVenodrIface() {
    if (agnssVendorIface == nullptr) {
        auto vendorGnss = getVendorHal();
        if (vendorGnss == nullptr) {
            ALOGE("failed to get gnss vendor hal\n");
        } else {
            auto agnssVendor = vendorGnss->getExtensionLocHidlAGnss();

            if (!agnssVendor.isOk()) {
                ALOGE("Unable to get a handle to AGnssVendor extension !");
            } else {
                agnssVendorIface = agnssVendor;
            }
        }
    }
    return agnssVendorIface;
}

static void LocationService_class_init_native(JNIEnv* env, jclass clazz) {

    ALOGD("%s", __FUNCTION__);

    method_reportAGpsStatus = env->GetMethodID(clazz, "reportAGpsStatus", "(II)V");
    method_setEngineCapabilities = env->GetMethodID(clazz, "setEngineCapabilities", "(I)V");
    method_reportNiNotification = env->GetMethodID(clazz, "reportNiNotification",
            "(IIIIILjava/lang/String;Ljava/lang/String;IILjava/lang/String;Z)V");
}

static jboolean LocationService_init(JNIEnv* env, jobject obj)
{
    ALOGD("%s", __FUNCTION__);

    // this must be set before calling into the HAL library
    if (!mCallbacksObj) {
        mCallbacksObj = env->NewGlobalRef(obj);
    }

    auto niIface = getGnssNiVendorIface();
    if (niIface == nullptr) {
        ALOGE("NULL GnssNiVendor Iface");
        return false;
    }
    auto agnssIface = getAGnssVenodrIface();
    if (agnssIface == nullptr) {
        ALOGE("NULL AGnssVendor Iface");
        return false;
    }

    if (gnssNiVendorCb == nullptr) {
        gnssNiVendorCb = new GnssNiVendorCallback();
    }
    TO_HIDL_SERVICE();
    auto r = niIface->setVendorCallback(gnssNiVendorCb);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }

    if (agnssCb == nullptr) {
        agnssCb = new AGnssCallback();
    }
    TO_HIDL_SERVICE();
    r = agnssIface->setCallback(agnssCb);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }

    return true;
}

static void LocationService_cleanup(JNIEnv* /*env*/, jobject /*obj*/)
{
    ALOGD("%s", __FUNCTION__);

    // TODO : Cleanup
}

static void LocationService_agps_data_conn_open(JNIEnv* env, jobject /*obj*/,
        jint agpsType, jstring apn, jint bearerType)
{
    ALOGD("%s", __FUNCTION__);

    auto iface = getAGnssVenodrIface();
    if (iface == nullptr) {
        ALOGE("AGNSS Iface NULL");
        return;
    }
    if (apn == NULL) {
        ALOGE("NULL APN");
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return;
    }

    const char *apnStr = env->GetStringUTFChars(apn, NULL);
    IAGnss::ApnIpType apnIpType = jniToHidl_BearerType(bearerType);

    TO_HIDL_SERVICE();
    auto result = iface->dataConnOpenExt(
            apnStr, apnIpType, static_cast<IAGnssCallback::AGnssType>(agpsType));
    if (!result.isOk() || !result){
        ALOGE("dataConnOpenExt() API failed !");
    }
    env->ReleaseStringUTFChars(apn, apnStr);
}

static void LocationService_agps_data_conn_closed(JNIEnv* /*env*/, jobject /*obj*/,
        jint agpsType)
{
    ALOGD("%s", __FUNCTION__);

    auto iface = getAGnssVenodrIface();
    if (iface == nullptr) {
        ALOGE("AGNSS Iface NULL");
        return;
    }

    TO_HIDL_SERVICE();
    auto result = iface->dataConnClosedExt(
            static_cast<IAGnssCallback::AGnssType>(agpsType));
    if (!result.isOk() || !result) {
        ALOGE("dataConnClosedExt() API failed !");
    }
}

static void LocationService_agps_data_conn_failed(JNIEnv* /*env*/, jobject /*obj*/,
        jint agpsType)
{
    ALOGD("%s", __FUNCTION__);

    auto iface = getAGnssVenodrIface();
    if (iface == nullptr) {
        ALOGE("AGNSS Iface NULL");
        return;
    }

    TO_HIDL_SERVICE();
    auto result = iface->dataConnFailedExt(
            static_cast<IAGnssCallback::AGnssType>(agpsType));
    if (!result.isOk() || !result) {
        ALOGE("dataConnFailedExt() API failed !");
    }
}

static void LocationService_send_ni_response(JNIEnv* /*env*/, jobject /*obj*/,
      jint notifId, jint response)
{
    ALOGD("%s", __FUNCTION__);

    auto iface = getGnssNiVendorIface();
    if (iface == nullptr) {
        ALOGE("GNSS NI Iface NULL");
        return;
    }

    IGnssNiCallback::GnssUserResponseType niResponse =
            jniToHidl_NiResponseType(response);
    TO_HIDL_SERVICE();
    auto r = iface->respond(notifId, niResponse);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
}

static JNINativeMethod sMethods[] = {
     /* name, signature, funcPtr */
    {"class_init_native", "()V", (void *)LocationService_class_init_native},
    {"native_init", "()Z", (void*)LocationService_init},
    {"native_cleanup", "()V", (void*)LocationService_cleanup},
    {"native_agps_data_conn_open", "(ILjava/lang/String;I)V",
        (void*)LocationService_agps_data_conn_open},
    {"native_agps_data_conn_closed", "(I)V", (void*)LocationService_agps_data_conn_closed},
    {"native_agps_data_conn_failed", "(I)V", (void*)LocationService_agps_data_conn_failed},
    {"native_send_ni_response", "(II)V", (void*)LocationService_send_ni_response},
};

int register_LocationService(JNIEnv* env)
{
    return jniRegisterNativeMethods(env,
            "com/qualcomm/location/LocationService", sMethods, NELEM(sMethods));
}


extern int register_Subscription(JNIEnv* env);
extern int register_RilInfoMonitor(JNIEnv* env);
extern int register_OsNpGlue (JNIEnv* env);
extern int register_IzatProvider (JNIEnv* env);
extern int register_FlpServiceProvider(JNIEnv* env);
extern int register_GeofenceServiceProvider(JNIEnv* env);
extern int register_DebugReportService(JNIEnv *env);
extern int register_WiFiDBReceiver(JNIEnv *env);

#if defined LOC_HIDL_VERSION_1_2
extern int register_WWANDBReceiver(JNIEnv *env);
#endif

extern "C" jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("GetEnv failed!");
        return result;
    }
    ALOG_ASSERT(env, "Could not retrieve the env!");

    ALOGD("LocationService_jni: JNI_OnLoad");

    register_Subscription(env);
    register_LocationService(env);
    register_RilInfoMonitor(env);
    register_OsNpGlue(env);
    register_IzatProvider(env);
    register_FlpServiceProvider(env);
    register_GeofenceServiceProvider(env);
    register_DebugReportService(env);
    register_WiFiDBReceiver(env);
#if defined LOC_HIDL_VERSION_1_2
    register_WWANDBReceiver(env);
#endif
    return JNI_VERSION_1_4;
}
