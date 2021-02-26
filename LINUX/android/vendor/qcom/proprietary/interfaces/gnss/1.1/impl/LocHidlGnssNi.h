/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_V1_1_LOCHIDLGNSSNI_H
#define VENDOR_QTI_GNSS_V1_1_LOCHIDLGNSSNI_H

#include <vendor/qti/gnss/1.1/ILocHidlGnssNi.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <LocHidlNiClient.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_1 {
namespace implementation {

using ::android::hardware::gnss::V1_0::IGnssNi;
using ::android::hardware::gnss::V1_0::IGnssNiCallback;
using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::gnss::V1_1::ILocHidlGnssNi;
using ::vendor::qti::gnss::V1_0::ILocHidlGnssNiCallback;
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
struct LocHidlGnssNi : public ILocHidlGnssNi {
    LocHidlGnssNi(sp<IGnssNi> gnssNi);
    ~LocHidlGnssNi();
    // Methods from ::android::hardware::gnss::V1_0::IGnssNi follow.
    Return<void> setCallback(const sp<IGnssNiCallback>& callback) override;
    Return<void> respond(int32_t notifId, IGnssNiCallback::GnssUserResponseType userResponse) override;

    // Methods from ::vendor::qti::gnss::V1_1::ILocHidlGnssNi follow.
    Return<void> setVendorCallback(const sp<ILocHidlGnssNiCallback>& callback) override;

private:
    LocHidlNiClient* mNiClient = nullptr;
    sp<IGnssNi> mGnssNi = nullptr;
    sp<ILocHidlGnssNiCallback> mCallbackIface = nullptr;
    sp<LocHidlDeathRecipient> mDeathRecipient = nullptr;
};

}  // namespace implementation
}  // namespace V1_1
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_GNSS_V1_1_LOCHIDLGNSSNI_H
