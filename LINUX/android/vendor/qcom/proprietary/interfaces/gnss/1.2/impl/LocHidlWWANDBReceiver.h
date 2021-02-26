/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_V1_2_LOCHIDLWWANDBRECEIVER_H
#define VENDOR_QTI_GNSS_V1_2_LOCHIDLWWANDBRECEIVER_H

#include <vendor/qti/gnss/1.2/ILocHidlWWANDBReceiver.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <location_interface.h>
#include "LBSAdapter.h"

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_2 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::gnss::V1_2::ILocHidlWWANDBReceiver;
using ::vendor::qti::gnss::V1_2::ILocHidlWWANDBReceiverCallback;
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
struct LocHidlWWANDBReceiver : public ILocHidlWWANDBReceiver {

    LocHidlWWANDBReceiver();
    ~LocHidlWWANDBReceiver();

    // Methods from ::vendor::qti::gnss::V1_2::ILocHidlWWANDBReceiver follow.
    Return<bool> init(const sp<ILocHidlWWANDBReceiverCallback>& callback) override;
    Return<void> registerWWANDBUpdater(const sp<ILocHidlWWANDBReceiverCallback>& callback) override;
    Return<void> unregisterWWANDBUpdater() override;
    Return<void> sendBSListRequest(int32_t expireInDays) override;
    Return<void> pushBSWWANDB(
            const hidl_vec<ILocHidlWWANDBReceiver::LocHidlBsLocationData>& bsLocationDataList,
            int16_t bsLocationDataListSize,
            const hidl_vec<ILocHidlWWANDBReceiver::LocHidlBsSpecialInfo>& bsSpecialInfoList,
            int16_t bsSpecialInfoListSize, int32_t daysValid) override;

private:
    LBSAdapter* mLBSAdapter = nullptr;
    sp<ILocHidlWWANDBReceiverCallback> mCallbackIface = nullptr;
    sp<LocHidlDeathRecipient> mDeathRecipient = nullptr;
};

extern "C" ILocHidlWWANDBReceiver* HIDL_FETCH_ILocHidlWWANDBReceiver(const char* name);

}  // namespace implementation
}  // namespace V1_2
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_GNSS_V1_2_LOCHIDLWWANDBRECEIVER_H
