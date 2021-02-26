/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_V1_0_LOCHIDLGEOFENCESERVICE_H
#define VENDOR_QTI_GNSS_V1_0_LOCHIDLGEOFENCESERVICE_H

#include <vendor/qti/gnss/1.0/ILocHidlGeofenceService.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <LocHidlGeofenceClient.h>
#include <location_interface.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_0 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::gnss::V1_0::ILocHidlGeofenceService;
using ::vendor::qti::gnss::V1_0::ILocHidlGeofenceServiceCallback;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_death_recipient;
using ::android::sp;
using ::android::wp;

struct LocHidlDeathRecipient;
struct LocHidlGeofenceService : public ILocHidlGeofenceService {

    LocHidlGeofenceService();
    ~LocHidlGeofenceService();

    // Methods from ::vendor::qti::gnss::V1_0::ILocHidlGeofenceService follow.
    Return<bool> init(const sp<ILocHidlGeofenceServiceCallback>& callback) override;
    Return<void> addGeofence(
            int32_t id, double latitude, double longitude, double radius,
            uint32_t transitionTypes, int32_t responsiveness, int32_t confidence,
            int32_t dwellTime, uint32_t dwellTimeMask) override;
    Return<void> removeGeofence(int32_t id) override;
    Return<void> updateGeofence(int32_t id, uint32_t transitionTypes, int32_t responsiveness) override;
    Return<void> pauseGeofence(int32_t id) override;
    Return<void> resumeGeofence(int32_t id, uint32_t transitionTypes) override;

private:
    // This method is not part of the ILocHidl base class.
    // It is called by LocHidlGeofenceServiceDeathRecipient to remove all geofences added so far.
    Return<void> removeAllGeofences();

private:
    LocHidlGeofenceClient* mIzatGeofenceApiClient = nullptr;
    sp<ILocHidlGeofenceServiceCallback> mCallbackIface = nullptr;
    sp<LocHidlDeathRecipient> mDeathRecipient = nullptr;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_GNSS_V1_0_LOCHIDLGEOFENCESERVICE_H
