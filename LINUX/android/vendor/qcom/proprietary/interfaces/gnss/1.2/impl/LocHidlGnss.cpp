/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_HIDL_Gnss"
#define LOG_NDEBUG 0
#include "LocHidlGnss.h"
#include <log_util.h>
#include <dlfcn.h>
#include <LocHidlUtils.h>

#define GNSS_LIB "android.hardware.gnss@1.0-impl-qti.so"

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_2 {
namespace implementation {

using ::vendor::qti::gnss::V1_0::LocHidlIzatStreamType;
using ::vendor::qti::gnss::V1_2::implementation::LocHidlAGnss;
using ::vendor::qti::gnss::V1_2::implementation::LocHidlDebugReportService;
using ::vendor::qti::gnss::V1_2::implementation::LocHidlFlpService;
using ::vendor::qti::gnss::V1_2::implementation::LocHidlGeofenceService;
using ::vendor::qti::gnss::V1_2::implementation::LocHidlGnssNi;
using ::vendor::qti::gnss::V1_2::implementation::LocHidlIzatOsNpGlue;
using ::vendor::qti::gnss::V1_2::implementation::LocHidlIzatProvider;
using ::vendor::qti::gnss::V1_2::implementation::LocHidlIzatSubscription;
using ::vendor::qti::gnss::V1_2::implementation::LocHidlRilInfoMonitor;
using ::vendor::qti::gnss::V1_2::implementation::LocHidlWiFiDBReceiver;
using ::vendor::qti::gnss::V1_2::implementation::LocHidlWWANDBReceiver;
using ::vendor::qti::gnss::V1_2::implementation::LocHidlXT;

LocHidlGnss::LocHidlGnss(const sp<IGnss> gnss) : mGnss(gnss) {

    ENTRY_LOG();
}
// Methods from ::android::hardware::gnss::V1_0::IGnss follow.
Return<bool> LocHidlGnss::setCallback(const sp<IGnssCallback>& callback) {
    static uint64_t deathCount = 0;
    FROM_HIDL_CLIENT();

    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = new LocHidlDeathRecipient([] {
                LOC_LOGE("%s]: IGnssCallback died.", __func__);
                });
    }
    mDeathRecipient->registerToPeer(callback, deathCount++);

    if (mGnss != nullptr) {
        return mGnss->setCallback(callback);
    }
    return bool {};
}

Return<bool> LocHidlGnss::start() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->start();
    }
    return bool {};
}

Return<bool> LocHidlGnss::stop() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->stop();
    }
    return bool {};
}

Return<void> LocHidlGnss::cleanup() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->cleanup();
    }
    return Void();
}

Return<bool> LocHidlGnss::injectTime(int64_t timeMs, int64_t timeReferenceMs, int32_t uncertaintyMs) {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->injectTime(timeMs, timeReferenceMs, uncertaintyMs);
    }
    return bool {};
}

Return<bool> LocHidlGnss::injectLocation(
        double latitudeDegrees, double longitudeDegrees, float accuracyMeters) {

    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->injectLocation(latitudeDegrees, longitudeDegrees, accuracyMeters);
    }
    return bool {};
}

Return<void> LocHidlGnss::deleteAidingData(IGnss::GnssAidingData aidingDataFlags) {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->deleteAidingData(aidingDataFlags);
    }
    return Void();
}

Return<bool> LocHidlGnss::setPositionMode(
        IGnss::GnssPositionMode mode, IGnss::GnssPositionRecurrence recurrence,
        uint32_t minIntervalMs, uint32_t preferredAccuracyMeters,
        uint32_t preferredTimeMs) {

    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->setPositionMode(mode, recurrence, minIntervalMs, preferredAccuracyMeters, preferredTimeMs);
    }
    return bool {};
}

Return<sp<IAGnssRil>> LocHidlGnss::getExtensionAGnssRil() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->getExtensionAGnssRil();
    }
    return ::android::sp<::android::hardware::gnss::V1_0::IAGnssRil> {};
}

Return<sp<IGnssGeofencing>> LocHidlGnss::getExtensionGnssGeofencing() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->getExtensionGnssGeofencing();
    }
    return ::android::sp<::android::hardware::gnss::V1_0::IGnssGeofencing> {};
}

Return<sp<IAGnss>> LocHidlGnss::getExtensionAGnss() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->getExtensionAGnss();
    }
    return ::android::sp<::android::hardware::gnss::V1_0::IAGnss> {};
}

Return<sp<IGnssNi>> LocHidlGnss::getExtensionGnssNi() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->getExtensionGnssNi();
    }
    return ::android::sp<::android::hardware::gnss::V1_0::IGnssNi> {};
}

Return<sp<IGnssMeasurement>> LocHidlGnss::getExtensionGnssMeasurement() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->getExtensionGnssMeasurement();
    }
    return ::android::sp<::android::hardware::gnss::V1_0::IGnssMeasurement> {};
}

Return<sp<IGnssNavigationMessage>> LocHidlGnss::getExtensionGnssNavigationMessage() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->getExtensionGnssNavigationMessage();
    }
    return ::android::sp<::android::hardware::gnss::V1_0::IGnssNavigationMessage> {};
}

Return<sp<IGnssXtra>> LocHidlGnss::getExtensionXtra() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->getExtensionXtra();
    }
    return ::android::sp<::android::hardware::gnss::V1_0::IGnssXtra> {};
}

Return<sp<IGnssConfiguration>> LocHidlGnss::getExtensionGnssConfiguration() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->getExtensionGnssConfiguration();
    }
    return ::android::sp<::android::hardware::gnss::V1_0::IGnssConfiguration> {};
}

Return<sp<IGnssDebug>> LocHidlGnss::getExtensionGnssDebug() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->getExtensionGnssDebug();
    }
    return ::android::sp<::android::hardware::gnss::V1_0::IGnssDebug> {};
}

Return<sp<IGnssBatching>> LocHidlGnss::getExtensionGnssBatching() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr) {
        return mGnss->getExtensionGnssBatching();
    }
    return ::android::sp<::android::hardware::gnss::V1_0::IGnssBatching> {};
}


// Methods from ::vendor::qti::gnss::V1_0::ILocHidlGnss follow.
Return<sp<V1_0::ILocHidlAGnss>> LocHidlGnss::getExtensionLocHidlAGnss() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr && mAGnssVendor == nullptr) {
        sp<IAGnss> agnss = mGnss->getExtensionAGnss();
        mAGnssVendor = new LocHidlAGnss(agnss);
    }
    return mAGnssVendor;
}

Return<sp<V1_0::ILocHidlDebugReportService>> LocHidlGnss::getExtensionLocHidlDebugReportService() {
    FROM_HIDL_CLIENT();
    if (mDebugReportService == nullptr) {
        mDebugReportService = new LocHidlDebugReportService();
    }
    return mDebugReportService;
}

Return<sp<V1_0::ILocHidlFlpService>> LocHidlGnss::getExtensionLocHidlFlpService() {
    FROM_HIDL_CLIENT();
    if (mGnssFlpServiceProvider == nullptr) {
        mGnssFlpServiceProvider = new LocHidlFlpService();
    }
    return mGnssFlpServiceProvider;
}

Return<sp<V1_0::ILocHidlGeofenceService>> LocHidlGnss::getExtensionLocHidlGeofenceService() {
    FROM_HIDL_CLIENT();
    if (mGnssGeofenceServiceProvider == nullptr) {
        mGnssGeofenceServiceProvider = new LocHidlGeofenceService();
    }
    return mGnssGeofenceServiceProvider;
}

Return<sp<V1_0::ILocHidlIzatProvider>> LocHidlGnss::getExtensionLocHidlIzatNetworkProvider() {
    FROM_HIDL_CLIENT();
    if (mIzatNetworkProvider == nullptr) {
        mIzatNetworkProvider = new LocHidlIzatProvider(LocHidlIzatStreamType::NETWORK);
    }
    return mIzatNetworkProvider;
}

Return<sp<V1_0::ILocHidlIzatProvider>> LocHidlGnss::getExtensionLocHidlIzatFusedProvider() {
    FROM_HIDL_CLIENT();
    if (mIzatFusedProvider == nullptr) {
        mIzatFusedProvider = new LocHidlIzatProvider(LocHidlIzatStreamType::FUSED);
    }
    return mIzatFusedProvider;
}

Return<sp<V1_0::ILocHidlGnssNi>> LocHidlGnss::getExtensionLocHidlGnssNi() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr && mGnssNiVendor == nullptr) {
        sp<IGnssNi> gnssNi = mGnss->getExtensionGnssNi();
        mGnssNiVendor = new LocHidlGnssNi(gnssNi);
    }
    return mGnssNiVendor;
}

Return<sp<V1_0::ILocHidlIzatOsNpGlue>> LocHidlGnss::getExtensionLocHidlIzatOsNpGlue() {
    FROM_HIDL_CLIENT();
    if (mIzatOsNpGlue == nullptr) {
        mIzatOsNpGlue = new LocHidlIzatOsNpGlue();
    }
    return mIzatOsNpGlue;
}

Return<sp<V1_0::ILocHidlIzatSubscription>> LocHidlGnss::getExtensionLocHidlIzatSubscription() {
    FROM_HIDL_CLIENT();
    if (mIzatSubscription == nullptr) {
        mIzatSubscription = new LocHidlIzatSubscription();
    }
    return mIzatSubscription;
}

Return<sp<V1_0::ILocHidlRilInfoMonitor>> LocHidlGnss::getExtensionLocHidlRilInfoMonitor() {
    FROM_HIDL_CLIENT();
    if (mGnssRilInfoMonitor == nullptr) {
        mGnssRilInfoMonitor = new LocHidlRilInfoMonitor();
    }
    return mGnssRilInfoMonitor;
}

Return<sp<V1_0::ILocHidlXT>> LocHidlGnss::getExtensionLocHidlXT() {
    FROM_HIDL_CLIENT();
    if (mGnssXT == nullptr) {
        mGnssXT = new LocHidlXT();
    }
    return mGnssXT;
}

Return<sp<V1_0::ILocHidlWiFiDBReceiver>> LocHidlGnss::getExtensionLocHidlWiFiDBReceiver() {
    FROM_HIDL_CLIENT();
    if (mIzatWiFiDBReceiver == nullptr) {
        mIzatWiFiDBReceiver = new LocHidlWiFiDBReceiver();
    }
    return mIzatWiFiDBReceiver;
}

// Methods from ::vendor::qti::gnss::V1_1::ILocHidlGnss follow.
Return<sp<V1_1::ILocHidlAGnss>> LocHidlGnss::getExtensionLocHidlAGnss_1_1() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr && mAGnssVendor == nullptr) {
        sp<IAGnss> agnss = mGnss->getExtensionAGnss();
        mAGnssVendor = new LocHidlAGnss(agnss);
    }
    return mAGnssVendor;
}

Return<sp<V1_1::ILocHidlDebugReportService>> LocHidlGnss::getExtensionLocHidlDebugReportService_1_1() {
    FROM_HIDL_CLIENT();
    if (mDebugReportService == nullptr) {
        mDebugReportService = new LocHidlDebugReportService();
    }
    return mDebugReportService;
}

Return<sp<V1_1::ILocHidlFlpService>> LocHidlGnss::getExtensionLocHidlFlpService_1_1() {
    FROM_HIDL_CLIENT();
    if (mGnssFlpServiceProvider == nullptr) {
        mGnssFlpServiceProvider = new LocHidlFlpService();
    }
    return mGnssFlpServiceProvider;
}

Return<sp<V1_1::ILocHidlGeofenceService>> LocHidlGnss::getExtensionLocHidlGeofenceService_1_1() {
    FROM_HIDL_CLIENT();
    if (mGnssGeofenceServiceProvider == nullptr) {
        mGnssGeofenceServiceProvider = new LocHidlGeofenceService();
    }
    return mGnssGeofenceServiceProvider;
}

Return<sp<V1_1::ILocHidlIzatProvider>> LocHidlGnss::getExtensionLocHidlIzatNetworkProvider_1_1() {
    FROM_HIDL_CLIENT();
    if (mIzatNetworkProvider == nullptr) {
        mIzatNetworkProvider = new LocHidlIzatProvider(LocHidlIzatStreamType::NETWORK);
    }
    return mIzatNetworkProvider;
}

Return<sp<V1_1::ILocHidlIzatProvider>> LocHidlGnss::getExtensionLocHidlIzatFusedProvider_1_1() {
    FROM_HIDL_CLIENT();
    if (mIzatFusedProvider == nullptr) {
        mIzatFusedProvider = new LocHidlIzatProvider(LocHidlIzatStreamType::FUSED);
    }
    return mIzatFusedProvider;
}

Return<sp<V1_1::ILocHidlGnssNi>> LocHidlGnss::getExtensionLocHidlGnssNi_1_1() {
    FROM_HIDL_CLIENT();
    if (mGnss != nullptr && mGnssNiVendor == nullptr) {
        sp<IGnssNi> gnssNi = mGnss->getExtensionGnssNi();
        mGnssNiVendor = new LocHidlGnssNi(gnssNi);
    }
    return mGnssNiVendor;
}

Return<sp<V1_1::ILocHidlIzatOsNpGlue>> LocHidlGnss::getExtensionLocHidlIzatOsNpGlue_1_1() {
    FROM_HIDL_CLIENT();
    if (mIzatOsNpGlue == nullptr) {
        mIzatOsNpGlue = new LocHidlIzatOsNpGlue();
    }
    return mIzatOsNpGlue;
}

Return<sp<V1_1::ILocHidlIzatSubscription>> LocHidlGnss::getExtensionLocHidlIzatSubscription_1_1() {
    FROM_HIDL_CLIENT();
    if (mIzatSubscription == nullptr) {
        mIzatSubscription = new LocHidlIzatSubscription();
    }
    return mIzatSubscription;
}

Return<sp<V1_1::ILocHidlRilInfoMonitor>> LocHidlGnss::getExtensionLocHidlRilInfoMonitor_1_1() {
    FROM_HIDL_CLIENT();
    if (mGnssRilInfoMonitor == nullptr) {
        mGnssRilInfoMonitor = new LocHidlRilInfoMonitor();
    }
    return mGnssRilInfoMonitor;
}

Return<sp<V1_1::ILocHidlXT>> LocHidlGnss::getExtensionLocHidlXT_1_1() {
    FROM_HIDL_CLIENT();
    if (mGnssXT == nullptr) {
        mGnssXT = new LocHidlXT();
    }
    return mGnssXT;
}

Return<sp<V1_1::ILocHidlWiFiDBReceiver>> LocHidlGnss::getExtensionLocHidlWiFiDBReceiver_1_1() {
    FROM_HIDL_CLIENT();
    if (mIzatWiFiDBReceiver == nullptr) {
        mIzatWiFiDBReceiver = new LocHidlWiFiDBReceiver();
    }
    return mIzatWiFiDBReceiver;
}

// Methods from ::vendor::qti::gnss::V1_2::ILocHidlGnss follow.

Return<sp<ILocHidlWWANDBReceiver>> LocHidlGnss::getExtensionLocHidlWWANDBReceiver() {
    FROM_HIDL_CLIENT();
    if (mIzatWWANDBReceiver == nullptr) {
        mIzatWWANDBReceiver = new LocHidlWWANDBReceiver();
    }
    return mIzatWWANDBReceiver;
}

// Methods from ::android::hidl::base::V1_0::IBase follow.

ILocHidlGnss* HIDL_FETCH_ILocHidlGnss(const char* /* name */) {
    ENTRY_LOG();
    const char* error = nullptr;
    dlerror();
    void *handle = dlopen(GNSS_LIB, RTLD_NOW);
    if (nullptr == handle || (error = dlerror()) != nullptr)  {
        LOC_LOGE("dlopen for " GNSS_LIB " failed, error = %s", error == nullptr ? "" : error);
        return nullptr;
    }

    IGnss* (*getter)(const char* name);
    *(void **)(&getter) = dlsym(handle, "HIDL_FETCH_IGnss");
    if ((error = dlerror()) != nullptr || getter == NULL)  {
        LOC_LOGE("dlsym for " GNSS_LIB " failed, error = %s", error == nullptr ? "" : error);
        return nullptr;
    }
    sp<IGnss> gnss = (IGnss*)(*getter)(nullptr);
    if (gnss == nullptr) {
        LOC_LOGE("failed to get IGnss from " GNSS_LIB);
        return nullptr;
    }
    return new LocHidlGnss(gnss);
}

}  // namespace implementation
}  // namespace V1_2
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
