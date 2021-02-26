/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_V1_2_LOCHIDLXT_H
#define VENDOR_QTI_GNSS_V1_2_LOCHIDLXT_H

#include <vendor/qti/gnss/1.1/ILocHidlXT.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_2 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::gnss::V1_1::ILocHidlXT;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct LocHidlXT : public ILocHidlXT {
    // Methods from ::vendor::qti::gnss::V1_1::ILocHidlXT follow.
    Return<void> init() override;
    Return<void> setUserPref(bool userPref) override;

private:
    typedef void (createXtAdapterInstance)();
    typedef void (setXtAdapterUserPref)(bool userPref);

};

}  // namespace implementation
}  // namespace V1_2
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_GNSS_V1_2_LOCHIDLXT_H
