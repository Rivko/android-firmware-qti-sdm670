/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_V1_2_LOCHIDLWIFIDBRECEIVER_H
#define VENDOR_QTI_GNSS_V1_2_LOCHIDLWIFIDBRECEIVER_H

#include <vendor/qti/gnss/1.1/ILocHidlWiFiDBReceiver.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_2 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::gnss::V1_1::ILocHidlWiFiDBReceiver;
using ::vendor::qti::gnss::V1_0::ILocHidlWiFiDBReceiverCallback;
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
struct LocHidlWiFiDBReceiver : public ILocHidlWiFiDBReceiver {
    LocHidlWiFiDBReceiver();
    ~LocHidlWiFiDBReceiver();
    // Methods from ::vendor::qti::gnss::V1_1::ILocHidlWiFiDBReceiver follow.
    Return<bool> init(const sp<ILocHidlWiFiDBReceiverCallback>& callback) override;
    Return<void> registerWiFiDBUpdater(const sp<ILocHidlWiFiDBReceiverCallback>& callback) override;
    Return<void> unregisterWiFiDBUpdater() override;
    Return<void> sendAPListRequest(int32_t expireInDays) override;
    Return<void> pushAPWiFiDB(const hidl_vec<ILocHidlWiFiDBReceiver::LocHidlApLocationData>& apLocationDataList,
                              int16_t apLocationDataListSize,
                              const hidl_vec<ILocHidlWiFiDBReceiver::LocHidlApSpecialInfo>& apSpecialInfoList,
                              int16_t apSpecialInfoListSize, int32_t daysValid) override;

private:
    void* mIzatWiFiDBReceiverApiClient = nullptr;
    sp<ILocHidlWiFiDBReceiverCallback> mCallbackIface = nullptr;
    sp<LocHidlDeathRecipient> mDeathRecipient = nullptr;
};

extern "C" ILocHidlWiFiDBReceiver* HIDL_FETCH_ILocHidlWiFiDBReceiver(const char* name);

}  // namespace implementation
}  // namespace V1_2
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_GNSS_V1_2_LOCHIDLWIFIDBRECEIVER_H
