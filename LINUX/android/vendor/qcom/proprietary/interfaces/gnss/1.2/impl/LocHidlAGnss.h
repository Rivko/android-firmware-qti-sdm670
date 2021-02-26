/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_V1_2_LOCHIDLAGNSS_H
#define VENDOR_QTI_GNSS_V1_2_LOCHIDLAGNSS_H

#include <vendor/qti/gnss/1.1/ILocHidlAGnss.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <gps_extended_c.h>
#include <location_interface.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_2 {
namespace implementation {

using ::android::hardware::gnss::V1_0::IAGnss;
using ::android::hardware::gnss::V1_0::IAGnssCallback;
using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::gnss::V1_1::ILocHidlAGnss;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct LocHidlDeathRecipient;
struct LocHidlAGnss : public ILocHidlAGnss {
    LocHidlAGnss(sp<IAGnss> agnss);
    ~LocHidlAGnss() = default;
    // Methods from ::android::hardware::gnss::V1_0::IAGnss follow.
    Return<void> setCallback(const sp<IAGnssCallback>& callback) override;
    Return<bool> dataConnClosed() override;
    Return<bool> dataConnFailed() override;
    Return<bool> setServer(IAGnssCallback::AGnssType type, const hidl_string& hostname, int32_t port) override;
    Return<bool> dataConnOpen(const hidl_string& apn, IAGnss::ApnIpType apnIpType) override;

    // Methods from ::vendor::qti::gnss::V1_1::ILocHidlAGnss follow.
    Return<bool> dataConnOpenExt(const hidl_string& apn, IAGnss::ApnIpType apnIpType, IAGnssCallback::AGnssType agnssType) override;
    Return<bool> dataConnClosedExt(IAGnssCallback::AGnssType agnssType) override;
    Return<bool> dataConnFailedExt(IAGnssCallback::AGnssType agnssType) override;
    // Method to fetch GNSS Interface
    GnssInterface* getGnssInterface();

    /* Data call setup callback passed down to GNSS HAL implementation */
    static void agnssStatusIpV4Cb(AGnssExtStatusIpV4 status);

    // Methods from ::android::hidl::base::V1_0::IBase follow.

private:
    sp<IAGnss> mAGnss = nullptr;
    GnssInterface* mGnssInterface = nullptr;
    static sp<LocHidlDeathRecipient> mDeathRecipient;
    static sp<IAGnssCallback> sAGnssCbIface;
};

}  // namespace implementation
}  // namespace V1_2
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_GNSS_V1_2_LOCHIDLAGNSS_H
