/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_TAG "LocSvc_HIDL_Subscription_jni"
#define LOG_NDEBUG 0

#include <inttypes.h>
#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include "utils/Log.h"
#include "utils_jni.h"
#include <list>

#if defined LOC_HIDL_VERSION_1_0
#include <vendor/qti/gnss/1.0/ILocHidlGnss.h>
#else
#include <vendor/qti/gnss/1.1/ILocHidlGnss.h>
#endif
#include <vendor/qti/gnss/1.0/ILocHidlIzatSubscriptionCallback.h>
using namespace android;

#if defined LOC_HIDL_VERSION_1_0
using vendor::qti::gnss::V1_0::ILocHidlGnss;
using vendor::qti::gnss::V1_0::ILocHidlIzatSubscription;
#else
using vendor::qti::gnss::V1_1::ILocHidlGnss;
using vendor::qti::gnss::V1_1::ILocHidlIzatSubscription;
#endif
using vendor::qti::gnss::V1_0::ILocHidlIzatSubscriptionCallback;
using vendor::qti::gnss::V1_0::LocHidlSubscriptionDataItemId;
using android::hardware::hidl_vec;
using android::hardware::Return;
using android::hardware::Void;
using android::hardware::hidl_death_recipient;
using android::hidl::base::V1_0::IBase;
using android::sp;

/* JMethods */
static jobject mOsAgentObj = NULL;
static jmethodID method_subscribe;
static jmethodID method_unsubscribe;
static jmethodID method_unsubscribeAll;
static jmethodID method_requestData;
static jmethodID method_turnOnModule;
static jmethodID method_turnOffModule;

#define BREAK_IF_ZERO(ERR,X,MSG) if(0==(X)) { \
    result = (ERR); \
    ALOGE(MSG); \
    break; \
}
#define MAC_ADDRESS_LENGTH 6
#define SRN_MAC_ADDRESS_LENGTH 6
#define SSID_LENGTH 32

/* ===================================================================
 *   Callbacks to Java layer
 * ===================================================================*/
void updateSubscribeJNI(
        const hidl_vec<LocHidlSubscriptionDataItemId>& list, bool subscribe);
void requestDataJNI(const hidl_vec<LocHidlSubscriptionDataItemId>& list);
void unsubscribeAllJNI();
void turnOnModuleJNI(LocHidlSubscriptionDataItemId dit,int timeOut);
void turnOffModuleJNI(LocHidlSubscriptionDataItemId dit);

/* ===========================================================
 *   HIDL Callbacks : ILocHidlIzatSubscriptionCallback.hal
 * ===========================================================*/
struct IzatSubscriptionCallback : public ILocHidlIzatSubscriptionCallback {

    Return<void> requestData(
            const hidl_vec<LocHidlSubscriptionDataItemId>& list) override;
    Return<void> updateSubscribe(
            const hidl_vec<LocHidlSubscriptionDataItemId>& list,
            bool subscribe) override;
    Return<void> unsubscribeAll() override;
    Return<void> turnOnModule(
            LocHidlSubscriptionDataItemId di, int32_t timeout) override;
    Return<void> turnOffModule(LocHidlSubscriptionDataItemId di) override;
};

Return<void> IzatSubscriptionCallback::requestData(
        const hidl_vec<LocHidlSubscriptionDataItemId>& list) {

    FROM_HIDL_SERVICE();
    requestDataJNI(list);
    return Void();
}

Return<void> IzatSubscriptionCallback::updateSubscribe(
        const hidl_vec<LocHidlSubscriptionDataItemId>& list, bool subscribe) {

    FROM_HIDL_SERVICE();
    updateSubscribeJNI(list, subscribe);
    return Void();
}

Return<void> IzatSubscriptionCallback::unsubscribeAll() {

    FROM_HIDL_SERVICE();
    unsubscribeAllJNI();
    return Void();
}

Return<void> IzatSubscriptionCallback::turnOnModule(
        LocHidlSubscriptionDataItemId di, int32_t timeout) {

    FROM_HIDL_SERVICE();
    turnOnModuleJNI(di, timeout);
    return Void();
}

Return<void> IzatSubscriptionCallback::turnOffModule(
        LocHidlSubscriptionDataItemId di) {

    FROM_HIDL_SERVICE();
    turnOffModuleJNI(di);
    return Void();
}

/* ===================================================================
 *   HIDL Interface
 * ===================================================================*/
static sp<ILocHidlIzatSubscription> gnssSubscriptionIface = nullptr;
static sp<ILocHidlIzatSubscriptionCallback> gnssSubscriptionCb = nullptr;

struct LocHidlDeathRecipient : virtual public hidl_death_recipient
{
    // hidl_death_recipient interface
    virtual void serviceDied(uint64_t cookie, const wp<IBase>& /*who*/) override {
        gnssSubscriptionIface = nullptr;
        ALOGE("%s] ILocHidlIzatSubscription died with cookie: %" PRIu64, __FILE__, cookie);
    }
};
static sp<LocHidlDeathRecipient> locHidlDeathRecipient = nullptr;

static sp<ILocHidlIzatSubscription>& getSubscriptionIface() {
    static uint64_t deathCount = 0;
    if (gnssSubscriptionIface == nullptr) {
        auto gnssVendorHal = ILocHidlGnss::getService(GNSS_VENDOR_SERVICE_INSTANCE);
        if (gnssVendorHal == nullptr) {
            ALOGE("Unable to get GnssVendor service\n");
        } else {
#if defined LOC_HIDL_VERSION_1_0
            auto subscription = gnssVendorHal->getExtensionLocHidlIzatSubscription();
#else
            auto subscription = gnssVendorHal->getExtensionLocHidlIzatSubscription_1_1();
#endif
            if (!subscription.isOk()) {
                ALOGE("Unable to get a handle to IzatSubscription");
            } else {
                if (locHidlDeathRecipient == nullptr) {
                    locHidlDeathRecipient = new LocHidlDeathRecipient();
                }
                gnssSubscriptionIface = subscription;
                auto r = gnssSubscriptionIface->linkToDeath(locHidlDeathRecipient, deathCount++);
                if (!r.isOk() || r == false) {
                    ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
                    gnssSubscriptionIface = nullptr;
                }
            }
        }
    }
    return gnssSubscriptionIface;
}

static void classInit(JNIEnv* env, jclass clazz)
{
    ALOGV("%s", __FUNCTION__);

    method_subscribe = env->GetMethodID(clazz, "subscribe", "([I)V");
    method_requestData = env->GetMethodID(clazz, "requestData", "([I)V");
    method_unsubscribe = env->GetMethodID(clazz, "unsubscribe", "([I)V");
    method_unsubscribeAll = env->GetMethodID(clazz, "unsubscribeAll", "()V");
    method_turnOnModule = env->GetMethodID(clazz, "turnOn", "(II)V");
    method_turnOffModule = env->GetMethodID(clazz, "turnOff", "(I)V");

    getSubscriptionIface();
}

static void instanceInit(JNIEnv *env, jobject obj)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    // cache the Java callback object
    if (!mOsAgentObj)
        mOsAgentObj = env->NewGlobalRef(obj);

    do {
        // init subscription.
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        if (gnssSubscriptionCb == nullptr) {
            gnssSubscriptionCb = new IzatSubscriptionCallback();
        }
        TO_HIDL_SERVICE();
        auto r = iface->init(gnssSubscriptionCb);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while (0);

    ALOGV("Exit Result %d", result);
}

static void instanceDeinit(JNIEnv *env, jobject /*obj*/)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    // clear Global References if any
    env->DeleteGlobalRef(mOsAgentObj);

    mOsAgentObj = NULL;

    do {
        // deinit subscription.
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");
        TO_HIDL_SERVICE();
        auto r = iface->deinit();
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while (0);

    ALOGV("Exit Result %d", result);
}

static void bool_dataitem_update(JNIEnv* env, jobject /*obj*/,
    jintArray dataItemIdArray, jbooleanArray updatedValueArray)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        jint * dataitem_arr = env->GetIntArrayElements(dataItemIdArray, 0);
        jboolean * updatedvalue_arr = env->GetBooleanArrayElements(updatedValueArray, 0);
        int array_len = env->GetArrayLength(dataItemIdArray);

        std::vector<ILocHidlIzatSubscription::BoolDataItem> dataItemArray = { };

        for (int idx = 0; idx < array_len; idx++) {
            ILocHidlIzatSubscription::BoolDataItem dataItem;
            dataItem.id = dataitem_arr[idx];
            dataItem.enabled = updatedvalue_arr[idx];
            dataItemArray.push_back(dataItem);
        }

        if (dataItemArray.size() != 0) {
            TO_HIDL_SERVICE();
            auto r = iface->boolDataItemUpdate(dataItemArray);
            if (!r.isOk()) {
                ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
            }
        }
    } while(0);

    ALOGV("Exit Result %d", result);
}

static void string_dataitem_update(JNIEnv* env, jobject /*obj*/,
    jint dataItemId, jstring updatedValue)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {

        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        const char *dataItem_string_val = env->GetStringUTFChars(updatedValue, NULL);
        BREAK_IF_ZERO(2, dataItem_string_val,
                "env->GetStringUTFChars returned null !");

        ILocHidlIzatSubscription::StringDataItem dataItem;
        dataItem.id = dataItemId;
        dataItem.str = dataItem_string_val;

        TO_HIDL_SERVICE();
        auto r = iface->stringDataItemUpdate(dataItem);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
        env->ReleaseStringUTFChars(updatedValue, dataItem_string_val);
    } while(0);

    ALOGV("Exit Result %d", result);
}

static void networkinfo_update(JNIEnv* env, jobject /*obj*/,
    jboolean is_connected, jint type, jstring type_name,
    jstring subtype_name, jboolean is_available, jboolean is_roaming)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        ILocHidlIzatSubscription::NetworkInfoDataItem dataItem;

        dataItem.type = type;
        dataItem.available = is_available;
        dataItem.connected = is_connected;
        dataItem.roaming = is_roaming;

        const char *type_name_string = env->GetStringUTFChars(type_name, NULL);
        if (type_name_string != NULL) {
            dataItem.typeName = type_name_string;
            env->ReleaseStringUTFChars(type_name, type_name_string);
        }

        const char *subtype_name_string = env->GetStringUTFChars(subtype_name, NULL);
        if (subtype_name_string != NULL) {
            dataItem.subTypeName = subtype_name_string;
            env->ReleaseStringUTFChars(subtype_name, subtype_name_string);
        }

        TO_HIDL_SERVICE();
        auto r = iface->networkinfoUpdate(dataItem);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }

    } while(0);

    ALOGV("Exit Result %d", result);
}

static void serviceinfo_update(JNIEnv* env, jobject /*obj*/,
    jint air_interface_type, jstring carrierName, jint Mcc, jint Mnc)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        ILocHidlIzatSubscription::RilServiceInfoDataItem dataItem;
        dataItem.type = air_interface_type;
        dataItem.mcc = Mcc;
        dataItem.mnc = Mnc;
        const char *carrier_name_string = env->GetStringUTFChars(carrierName, NULL);
        if (carrier_name_string != NULL) {
            dataItem.name = carrier_name_string;
            env->ReleaseStringUTFChars(carrierName, carrier_name_string);
        }

        TO_HIDL_SERVICE();
        auto r = iface->serviceinfoUpdate(dataItem);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while(0);

    ALOGV("Exit Result %d", result);
}

static void cell_update_gw(JNIEnv* /*env*/, jobject /*obj*/,
        jint air_interface_type, jint nwStatus, jint iMcc,
        jint iMnc, jint lac, jint cid, jint valid_mask)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        ILocHidlIzatSubscription::CellGwDataItem dataItem;
        dataItem.type = air_interface_type;
        dataItem.status = nwStatus;
        dataItem.mcc = iMcc;
        dataItem.mnc = iMnc;
        dataItem.lac = lac;
        dataItem.cid = cid;
        dataItem.mask = valid_mask;

        TO_HIDL_SERVICE();
        auto r = iface->cellGwUpdate(dataItem);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while(0);

    ALOGV("Exit Result %d", result);
}

static void cell_update_lte(
        JNIEnv* /*env*/, jobject /*obj*/, jint air_interface_type,
        jint nwStatus, jint iMcc, jint iMnc, jint cid, jint pci, jint tac,
        jint valid_mask)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        ILocHidlIzatSubscription::CellLteDataItem dataItem;
        dataItem.type = air_interface_type;
        dataItem.status = nwStatus;
        dataItem.mcc = iMcc;
        dataItem.mnc = iMnc;
        dataItem.cid = cid;
        dataItem.pci = pci;
        dataItem.tac = tac;
        dataItem.mask = valid_mask;

        TO_HIDL_SERVICE();
        auto r = iface->cellLteUpdate(dataItem);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while(0);

    ALOGV("Exit Result %d", result);
}

static void cell_update_cdma(JNIEnv* /*env*/, jobject /*obj*/,
        jint air_interface_type, jint nwStatus,
        jint sid, jint nid, jint bsid, jint bslat,
        jint bslong, jboolean inDST, jint UTCTimeOffset, jint valid_mask)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        ILocHidlIzatSubscription::CellCdmaDataItem dataItem;
        dataItem.type = air_interface_type;
        dataItem.status = nwStatus;
        dataItem.sid = sid;
        dataItem.nid = nid;
        dataItem.bsid = bsid;
        dataItem.bslat = bslat;
        dataItem.bslong = bslong;
        dataItem.inDST = inDST;
        dataItem.timeOffset = UTCTimeOffset;
        dataItem.mask = valid_mask;

        TO_HIDL_SERVICE();
        auto r = iface->cellCdmaUpdate(dataItem);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while(0);

    ALOGV("Exit Result %d", result);
}

static void cell_update_ooo(
        JNIEnv* /*env*/, jobject /*obj*/, jint nwStatus, jint valid_mask)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        ILocHidlIzatSubscription::CellOooDataItem dataItem;
        dataItem.status = nwStatus;
        dataItem.mask = valid_mask;

        TO_HIDL_SERVICE();
        auto r = iface->cellOooUpdate(dataItem);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while(0);

    ALOGV("Exit Result %d", result);
}

static void service_state_update(JNIEnv* /*env*/, jobject /*obj*/, jint nwStatus)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        ILocHidlIzatSubscription::ServiceStateDataItem dataItem;
        dataItem.status = nwStatus;

        TO_HIDL_SERVICE();
        auto r = iface->serviceStateUpdate(dataItem);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while(0);

    ALOGV("Exit Result %d", result);
}

static void screen_status_update(
        JNIEnv* /*env*/, jobject /*obj*/, jboolean status)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        ILocHidlIzatSubscription::ScreenStatusDataItem dataItem;
        dataItem.status = status;

        TO_HIDL_SERVICE();
        auto r = iface->screenStatusUpdate(dataItem);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while (0);

    ALOGV("Exit Result %d", result);
}

static void power_connect_status_update(
        JNIEnv* /*env*/, jobject /*obj*/, jboolean status)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        ILocHidlIzatSubscription::PowerConnectStatusDataItem dataItem;
        dataItem.status = status;

        TO_HIDL_SERVICE();
        auto r = iface->powerConnectStatusUpdate(dataItem);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while (0);

    ALOGV("Exit Result %d", result);
}

static void timezone_change_update(JNIEnv* /*env*/, jobject /*obj*/,
    jlong curTimeMillis, jint rawOffset, jint dstOffset)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        ILocHidlIzatSubscription::TimeZoneChangeDataItem dataItem;
        dataItem.curTimeMillis = curTimeMillis;
        dataItem.rawOffset = rawOffset;
        dataItem.dstOffset = dstOffset;

        TO_HIDL_SERVICE();
        auto r = iface->timezoneChangeUpdate(dataItem);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while (0);

    ALOGV("Exit Result %d", result);
}

static void time_change_update(JNIEnv* /*env*/, jobject /*obj*/,
    jlong curTimeMillis, jint rawOffset, jint dstOffset)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        ILocHidlIzatSubscription::TimeChangeDataItem dataItem;
        dataItem.curTimeMillis = curTimeMillis;
        dataItem.rawOffset = rawOffset;
        dataItem.dstOffset = dstOffset;

        TO_HIDL_SERVICE();
        auto r = iface->timeChangeUpdate(dataItem);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while (0);

    ALOGV("Exit Result %d", result);
}

static void shutdown_update(JNIEnv* /*env*/, jobject /*obj*/)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        TO_HIDL_SERVICE();
        auto r = iface->shutdownUpdate();
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while (0);

    ALOGV("Exit Result %d", result);
}

static void wifi_supplicant_status_update(JNIEnv *env, jobject /*obj*/, jint state,
                                     jint ap_mac_valid,
                                     jbyteArray ap_mac_array,
                                     jint ssid_valid,
                                     jcharArray ssid_array)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        ILocHidlIzatSubscription::WifiSupplicantStatusDataItem dataItem;
        jbyte * mac_arr = 0;
        jchar * ssid_arr = 0;
        int ssid_arr_len = 0;
        if (ap_mac_valid) {
            mac_arr = env->GetByteArrayElements(ap_mac_array, 0);
        }
        if (ssid_valid) {
            ssid_arr = env->GetCharArrayElements(ssid_array, 0);
            ssid_arr_len = env->GetArrayLength(ssid_array);
        }

        dataItem.state = state;
        dataItem.apMacAddressValid = ap_mac_valid;
        if ((mac_arr != NULL) && (dataItem.apMacAddressValid == true)) {
            std::vector<uint8_t> vec;
            for(int i = 0; i < MAC_ADDRESS_LENGTH; i++) {
                vec.push_back(mac_arr[i]);
            }
            dataItem.apMacAddress = vec;
        }
        dataItem.apSsidValid = ssid_valid;
        if ((ssid_arr != NULL) && (dataItem.apSsidValid == true)) {
            char str[SSID_LENGTH];
            memset(str, 0, SSID_LENGTH);
            /* Received SSID can be encapsulated as "SSID" or as
             * <Unknown_ssid> or a string of hex digits.
             * If SSID is encapsulated in quotes, remove the quotes;
             * or else pass it on as it is. */
            int str_idx = 0, ssid_arr_idx = 0;
            if (ssid_arr[0] == '"' && ssid_arr_len > 0) {
                ssid_arr_idx = 1;
                ssid_arr_len--;
            }
            for (; str_idx < SSID_LENGTH-1 && ssid_arr_idx < ssid_arr_len;
                    str_idx++, ssid_arr_idx++) {
                str[str_idx] = ssid_arr[ssid_arr_idx];
            }
            dataItem.apSsid = str;
        }

        if (NULL != mac_arr) {
            env->ReleaseByteArrayElements(ap_mac_array, mac_arr, 0);
        }

        if (NULL != ssid_arr) {
            env->ReleaseCharArrayElements(ssid_array, ssid_arr, 0);
        }

        TO_HIDL_SERVICE();
        auto r = iface->wifiSupplicantStatusUpdate(dataItem);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while (0);

    ALOGV("Exit Result %d", result);
}

static void btle_scan_data_inject(JNIEnv* env, jobject /*obj*/, jboolean is_valid,
                jint rssi, jbyteArray  btle_mac_array, jlong btle_scan_req_timestamp,
                jlong scan_start_timestamp, jlong scan_recv_timestamp, jint error_code)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        ILocHidlIzatSubscription::BtLeDeviceScanDetailsDataItem dataItem;
        dataItem.validSrnData = is_valid;
        dataItem.errorCause = error_code;
        if (is_valid) {
            jbyte * mac_arr = 0;
            mac_arr = env->GetByteArrayElements(btle_mac_array, 0);
            if (mac_arr != NULL) {
                std::vector<uint8_t> vec;
                for(int i = 0; i < SRN_MAC_ADDRESS_LENGTH; i++) {
                    vec.push_back(mac_arr[i]);
                }
                dataItem.apSrnMacAddress = vec;
            }
            dataItem.apSrnRssi = rssi;
            dataItem.apSrnTimestamp = btle_scan_req_timestamp;
            dataItem.requestTimestamp = scan_start_timestamp;
            dataItem.receiveTimestamp = scan_recv_timestamp;

            if(NULL != mac_arr) {
                env->ReleaseByteArrayElements(btle_mac_array, mac_arr, 0);
            }
        }

        TO_HIDL_SERVICE();
        auto r = iface->btleScanDataInject(dataItem);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while (0);

    ALOGV("Exit Result %d", result);
}

static void bt_classic_scan_data_inject(JNIEnv* env, jobject /*obj*/,jboolean is_valid,
            jint rssi, jbyteArray  bt_mac_array, jlong bt_scan_req_timestamp,
            jlong scan_start_timestamp, jlong scan_recv_timestamp, jint error_code)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        ILocHidlIzatSubscription::BtDeviceScanDetailsDataItem dataItem;

        dataItem.validSrnData = is_valid;
        dataItem.errorCause = error_code;
        if (is_valid) {
            jbyte * mac_arr = 0;
            mac_arr = env->GetByteArrayElements(bt_mac_array, 0);

            if (mac_arr != NULL) {
                std::vector<uint8_t> vec;
                for(int i = 0; i < SRN_MAC_ADDRESS_LENGTH; i++) {
                    vec.push_back(mac_arr[i]);
                }
                dataItem.apSrnMacAddress = vec;
            }
            dataItem.apSrnRssi = rssi;
            dataItem.apSrnTimestamp = bt_scan_req_timestamp;
            dataItem.requestTimestamp = scan_start_timestamp;
            dataItem.receiveTimestamp = scan_recv_timestamp;

            if(NULL != mac_arr) {
                env->ReleaseByteArrayElements(bt_mac_array, mac_arr, 0);
            }
        }

        TO_HIDL_SERVICE();
        auto r = iface->btClassicScanDataInject(dataItem);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while (0);

    ALOGV("Exit Result %d", result);
}

/* ===================================================================
 *   Callbacks to Java layer
 * ===================================================================*/
void updateSubscribeJNI(
        const hidl_vec<LocHidlSubscriptionDataItemId>& list, bool subscribe)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    jintArray dataItemsArray = NULL;

    do {
        JNIEnv* env = getCallbackEnv();
        BREAK_IF_ZERO(2, env, "getCallbackEnv() returned null");

        int listSize = list.size();
        jintArray dataItemsArray = (jintArray)env->NewIntArray(listSize);
        BREAK_IF_ZERO(3, dataItemsArray, "env->NewIntArray returned null");

        for (int i = 0; i < listSize; i++) {
            jint dataItem = (int)list[i];
            env->SetIntArrayRegion(dataItemsArray, i, 1, &dataItem);
        }

        if (subscribe) {
            env->CallVoidMethod(mOsAgentObj, method_subscribe, dataItemsArray);
        } else {
            env->CallVoidMethod(mOsAgentObj, method_unsubscribe, dataItemsArray);
        }

        if (dataItemsArray) env->DeleteLocalRef(dataItemsArray);

        checkAndClearExceptionsFromCallback(env, __FUNCTION__);

    } while (0);

    ALOGV("Exit Result %d", result);
}

void requestDataJNI(const hidl_vec<LocHidlSubscriptionDataItemId>& list)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    jintArray dataItemsArray = NULL;

    do {
        JNIEnv* env = getCallbackEnv();
        BREAK_IF_ZERO(2, env, "getCallbackEnv() returned null");

        int listSize = list.size();
        jintArray dataItemsArray = (jintArray)env->NewIntArray(listSize);
        BREAK_IF_ZERO(3, dataItemsArray, "env->NewIntArray returned null");

        for (int i = 0; i < listSize; i++) {
            jint dataItem = (int)list[i];
            env->SetIntArrayRegion(dataItemsArray, i, 1, &dataItem);
        }

        env->CallVoidMethod(mOsAgentObj, method_requestData, dataItemsArray);

        if (dataItemsArray) env->DeleteLocalRef(dataItemsArray);

        checkAndClearExceptionsFromCallback(env, __FUNCTION__);

    } while (0);

    ALOGV("Exit Result %d", result);
}

void unsubscribeAllJNI()
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        JNIEnv* env = getCallbackEnv();
        BREAK_IF_ZERO(2, env, "getCallbackEnv() returned null");

        env->CallVoidMethod(mOsAgentObj, method_unsubscribeAll);
        checkAndClearExceptionsFromCallback(env, __FUNCTION__);

    } while (0);

    ALOGV("Exit Result %d", result);
}

void turnOnModuleJNI(LocHidlSubscriptionDataItemId dit, int timeOut)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        JNIEnv* env = getCallbackEnv();
        BREAK_IF_ZERO(2, env, "getCallbackEnv() returned null");
        env->CallVoidMethod(mOsAgentObj, method_turnOnModule, dit, timeOut);
        checkAndClearExceptionsFromCallback(env, __FUNCTION__);
    } while (0);

    ALOGV("Exit Result %d", result);
}

void turnOffModuleJNI(LocHidlSubscriptionDataItemId dit)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);

    do {
        JNIEnv* env = getCallbackEnv();
        BREAK_IF_ZERO(2, env, "getCallbackEnv() returned null");
        env->CallVoidMethod(mOsAgentObj, method_turnOffModule, dit);
        checkAndClearExceptionsFromCallback(env, __FUNCTION__);

    } while (0);

    ALOGV("Exit Result %d", result);
}

static void battery_level_update(
        JNIEnv* /*env*/, jobject /*obj*/, jint batterypct)
{
    int result = 0;
    ALOGV("%s", __FUNCTION__);
#ifndef LOC_HIDL_VERSION_1_0
    do {
        auto iface = getSubscriptionIface();
        BREAK_IF_ZERO(1, iface != nullptr,
                "gnssSubscriptionIface null !");

        ILocHidlIzatSubscription::BatteryLevelDataItem dataItem;
        dataItem.batteryPct = batterypct;

        TO_HIDL_SERVICE();
        auto r = iface->batteryLevelUpdate(dataItem);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    } while (0);
#endif
    ALOGV("Exit Result %d", result);
}

static JNINativeMethod sMethods[] = {
     /* name, signature, funcPtr */
    {"native_subscription_class_init", "()V", (void *)classInit},
    {"native_subscription_init", "()V", (void *)instanceInit},
    {"native_subscription_deinit", "()V", (void *)instanceDeinit},
    {"native_bool_dataitem_update", "([I[Z)V", (void *)bool_dataitem_update},
    {"native_string_dataitem_update", "(ILjava/lang/String;)V", (void *)string_dataitem_update},
    {"native_networkinfo_update", "(ZILjava/lang/String;Ljava/lang/String;ZZ)V", (void *)networkinfo_update},
    {"native_serviceinfo_update", "(ILjava/lang/String;II)V", (void *)serviceinfo_update},
    {"native_cell_update_lte", "(IIIIIIII)V", (void *)cell_update_lte},
    {"native_cell_update_gw", "(IIIIIII)V", (void *)cell_update_gw},
    {"native_cell_update_cdma", "(IIIIIIIZII)V", (void *)cell_update_cdma},
    {"native_cell_update_ooo", "(II)V", (void *)cell_update_ooo},
    {"native_service_state_update", "(I)V", (void *)service_state_update},
    {"native_screen_status_update", "(Z)V", (void*) screen_status_update},
    {"native_power_connect_status_update", "(Z)V", (void*) power_connect_status_update},
    {"native_timezone_change_update", "(JII)V", (void*) timezone_change_update},
    {"native_time_change_update", "(JII)V", (void*) time_change_update},
    {"native_shutdown_update", "()V", (void*) shutdown_update},
    {"native_wifi_supplicant_status_update", "(II[BI[C)V", (void*) wifi_supplicant_status_update},
    {"native_btle_scan_data_inject", "(ZI[BJJJI)V", (void*) btle_scan_data_inject},
    {"native_bt_classic_scan_data_inject", "(ZI[BJJJI)V", (void*) bt_classic_scan_data_inject},
    {"native_battery_level_update", "(I)V", (void*) battery_level_update},
};

int register_Subscription(JNIEnv* env) {
    return jniRegisterNativeMethods(env, "com/qualcomm/location/osagent/OsAgent",
                                    sMethods, NELEM(sMethods));
}
