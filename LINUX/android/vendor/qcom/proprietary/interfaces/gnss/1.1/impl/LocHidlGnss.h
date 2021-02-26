/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_V1_1_LOCHIDLGNSS_H
#define VENDOR_QTI_GNSS_V1_1_LOCHIDLGNSS_H

#include <vendor/qti/gnss/1.1/ILocHidlGnss.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include <LocHidlAGnss.h>
#include <LocHidlDebugReportService.h>
#include <LocHidlFlpService.h>
#include <LocHidlGeofenceService.h>
#include <LocHidlIzatProvider.h>
#include <LocHidlGnssNi.h>
#include <LocHidlIzatOsNpGlue.h>
#include <LocHidlIzatSubscription.h>
#include <LocHidlRilInfoMonitor.h>
#include <LocHidlXT.h>
#include <LocHidlWiFiDBReceiver.h>
namespace vendor {
namespace qti {
namespace gnss {
namespace V1_1 {
namespace implementation {

using ::android::hardware::gnss::V1_0::IAGnss;
using ::android::hardware::gnss::V1_0::IAGnssRil;
using ::android::hardware::gnss::V1_0::IGnss;
using ::android::hardware::gnss::V1_0::IGnssBatching;
using ::android::hardware::gnss::V1_0::IGnssCallback;
using ::android::hardware::gnss::V1_0::IGnssConfiguration;
using ::android::hardware::gnss::V1_0::IGnssDebug;
using ::android::hardware::gnss::V1_0::IGnssGeofencing;
using ::android::hardware::gnss::V1_0::IGnssMeasurement;
using ::android::hardware::gnss::V1_0::IGnssNavigationMessage;
using ::android::hardware::gnss::V1_0::IGnssNi;
using ::android::hardware::gnss::V1_0::IGnssXtra;
using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::gnss::V1_1::ILocHidlAGnss;
using ::vendor::qti::gnss::V1_1::ILocHidlDebugReportService;
using ::vendor::qti::gnss::V1_1::ILocHidlFlpService;
using ::vendor::qti::gnss::V1_1::ILocHidlGeofenceService;
using ::vendor::qti::gnss::V1_1::ILocHidlGnss;
using ::vendor::qti::gnss::V1_1::ILocHidlGnssNi;
using ::vendor::qti::gnss::V1_1::ILocHidlIzatOsNpGlue;
using ::vendor::qti::gnss::V1_1::ILocHidlIzatProvider;
using ::vendor::qti::gnss::V1_1::ILocHidlIzatSubscription;
using ::vendor::qti::gnss::V1_1::ILocHidlRilInfoMonitor;
using ::vendor::qti::gnss::V1_1::ILocHidlWiFiDBReceiver;
using ::vendor::qti::gnss::V1_1::ILocHidlXT;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct LocHidlDeathRecipient;
struct LocHidlGnss : public ILocHidlGnss {
    LocHidlGnss(sp<IGnss> gnss);
    ~LocHidlGnss() = default;
    // Methods from ::android::hardware::gnss::V1_0::IGnss follow.
    Return<bool> setCallback(const sp<IGnssCallback>& callback) override;
    Return<bool> start() override;
    Return<bool> stop() override;
    Return<void> cleanup() override;
    Return<bool> injectTime(int64_t timeMs, int64_t timeReferenceMs, int32_t uncertaintyMs) override;
    Return<bool> injectLocation(double latitudeDegrees, double longitudeDegrees, float accuracyMeters) override;
    Return<void> deleteAidingData(IGnss::GnssAidingData aidingDataFlags) override;
    Return<bool> setPositionMode(
            IGnss::GnssPositionMode mode, IGnss::GnssPositionRecurrence recurrence,
            uint32_t minIntervalMs, uint32_t preferredAccuracyMeters,
            uint32_t preferredTimeMs) override;
    Return<sp<IAGnssRil>> getExtensionAGnssRil() override;
    Return<sp<IGnssGeofencing>> getExtensionGnssGeofencing() override;
    Return<sp<IAGnss>> getExtensionAGnss() override;
    Return<sp<IGnssNi>> getExtensionGnssNi() override;
    Return<sp<IGnssMeasurement>> getExtensionGnssMeasurement() override;
    Return<sp<IGnssNavigationMessage>> getExtensionGnssNavigationMessage() override;
    Return<sp<IGnssXtra>> getExtensionXtra() override;
    Return<sp<IGnssConfiguration>> getExtensionGnssConfiguration() override;
    Return<sp<IGnssDebug>> getExtensionGnssDebug() override;
    Return<sp<IGnssBatching>> getExtensionGnssBatching() override;

    // Methods from ::vendor::qti::gnss::V1_0::ILocHidlGnss follow.
    Return<sp<V1_0::ILocHidlAGnss>> getExtensionLocHidlAGnss() override;
    Return<sp<V1_0::ILocHidlDebugReportService>> getExtensionLocHidlDebugReportService() override;
    Return<sp<V1_0::ILocHidlFlpService>> getExtensionLocHidlFlpService() override;
    Return<sp<V1_0::ILocHidlGeofenceService>> getExtensionLocHidlGeofenceService() override;
    Return<sp<V1_0::ILocHidlIzatProvider>> getExtensionLocHidlIzatNetworkProvider() override;
    Return<sp<V1_0::ILocHidlIzatProvider>> getExtensionLocHidlIzatFusedProvider() override;
    Return<sp<V1_0::ILocHidlGnssNi>> getExtensionLocHidlGnssNi() override;
    Return<sp<V1_0::ILocHidlIzatOsNpGlue>> getExtensionLocHidlIzatOsNpGlue() override;
    Return<sp<V1_0::ILocHidlIzatSubscription>> getExtensionLocHidlIzatSubscription() override;
    Return<sp<V1_0::ILocHidlRilInfoMonitor>> getExtensionLocHidlRilInfoMonitor() override;
    Return<sp<V1_0::ILocHidlXT>> getExtensionLocHidlXT() override;
    Return<sp<V1_0::ILocHidlWiFiDBReceiver>> getExtensionLocHidlWiFiDBReceiver() override;

    // Methods from ::vendor::qti::gnss::V1_1::ILocHidlGnss follow.
    Return<sp<ILocHidlAGnss>> getExtensionLocHidlAGnss_1_1() override;
    Return<sp<ILocHidlDebugReportService>> getExtensionLocHidlDebugReportService_1_1() override;
    Return<sp<ILocHidlFlpService>> getExtensionLocHidlFlpService_1_1() override;
    Return<sp<ILocHidlGeofenceService>> getExtensionLocHidlGeofenceService_1_1() override;
    Return<sp<ILocHidlIzatProvider>> getExtensionLocHidlIzatNetworkProvider_1_1() override;
    Return<sp<ILocHidlIzatProvider>> getExtensionLocHidlIzatFusedProvider_1_1() override;
    Return<sp<ILocHidlGnssNi>> getExtensionLocHidlGnssNi_1_1() override;
    Return<sp<ILocHidlIzatOsNpGlue>> getExtensionLocHidlIzatOsNpGlue_1_1() override;
    Return<sp<ILocHidlIzatSubscription>> getExtensionLocHidlIzatSubscription_1_1() override;
    Return<sp<ILocHidlRilInfoMonitor>> getExtensionLocHidlRilInfoMonitor_1_1() override;
    Return<sp<ILocHidlXT>> getExtensionLocHidlXT_1_1() override;
    Return<sp<ILocHidlWiFiDBReceiver>> getExtensionLocHidlWiFiDBReceiver_1_1() override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.

private:
    sp<IGnss> mGnss = nullptr;
    sp<ILocHidlAGnss> mAGnssVendor = nullptr;
    sp<ILocHidlDebugReportService> mDebugReportService = nullptr;
    sp<ILocHidlFlpService> mGnssFlpServiceProvider = nullptr;
    sp<ILocHidlGeofenceService> mGnssGeofenceServiceProvider = nullptr;
    sp<ILocHidlIzatProvider> mIzatNetworkProvider = nullptr;
    sp<ILocHidlIzatProvider> mIzatFusedProvider = nullptr;
    sp<ILocHidlGnssNi> mGnssNiVendor = nullptr;
    sp<ILocHidlIzatOsNpGlue> mIzatOsNpGlue = nullptr;
    sp<ILocHidlIzatSubscription> mIzatSubscription = nullptr;
    sp<ILocHidlRilInfoMonitor> mGnssRilInfoMonitor = nullptr;
    sp<ILocHidlXT> mGnssXT = nullptr;
    sp<ILocHidlWiFiDBReceiver> mIzatWiFiDBReceiver = nullptr;

    sp<LocHidlDeathRecipient> mDeathRecipient;
};

extern "C" ILocHidlGnss* HIDL_FETCH_ILocHidlGnss(const char* name);

}  // namespace implementation
}  // namespace V1_1
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_GNSS_V1_1_LOCHIDLGNSS_H
