/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_V1_0_IZATOSNPGLUE_H
#define VENDOR_QTI_GNSS_V1_0_IZATOSNPGLUE_H

#include <vendor/qti/gnss/1.0/ILocHidlIzatOsNpGlue.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <OsNpGlue.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_0 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::gnss::V1_0::LocHidlIzatLocation;
using ::vendor::qti::gnss::V1_0::LocHidlIzatRequest;
using ::vendor::qti::gnss::V1_0::LocHidlIzatProviderStatus;
using ::vendor::qti::gnss::V1_0::ILocHidlIzatOsNpGlue;
using ::vendor::qti::gnss::V1_0::ILocHidlIzatOsNpGlueCallback;
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
struct LocHidlIzatOsNpGlue : public ILocHidlIzatOsNpGlue {
    LocHidlIzatOsNpGlue() = default;
    ~LocHidlIzatOsNpGlue() = default;
    // Methods from ::vendor::qti::gnss::V1_0::ILocHidlIzatOsNpGlue follow.
    Return<bool> setCallback(const sp<ILocHidlIzatOsNpGlueCallback>& callback) override;
    Return<void> locationChanged(const LocHidlIzatLocation& location) override;

    sp<LocHidlDeathRecipient> mDeathRecipient = nullptr;
    sp<ILocHidlIzatOsNpGlueCallback> mCallback = nullptr;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_GNSS_V1_0_IZATOSNPGLUE_H
