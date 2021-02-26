/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_V1_2_LOCHIDLRILINFOMONITOR_H
#define VENDOR_QTI_GNSS_V1_2_LOCHIDLRILINFOMONITOR_H

#include <vendor/qti/gnss/1.1/ILocHidlRilInfoMonitor.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include "LBSAdapter.h"

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_2 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::gnss::V1_1::ILocHidlRilInfoMonitor;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct LocHidlRilInfoMonitor : public ILocHidlRilInfoMonitor {
    // Methods from ::vendor::qti::gnss::V1_1::ILocHidlRilInfoMonitor follow.
    Return<void> init() override;
    Return<void> cinfoInject(
            int32_t cid, int32_t lac, int32_t mnc, int32_t mcc,
            bool roaming) override;
    Return<void> oosInform() override;
    Return<void> niSuplInit(const hidl_string& str) override;
    Return<void> chargerStatusInject(int32_t status) override;

private:
    LBSAdapter* mLBSAdapter = NULL;

};

}  // namespace implementation
}  // namespace V1_2
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_GNSS_V1_2_LOCHIDLRILINFOMONITOR_H
