/*!
 * @file HidlVppService.h
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services
 */

#ifndef VENDOR_QTI_HARDWARE_VPP_V1_1_HIDLVPPSERVICE_H
#define VENDOR_QTI_HARDWARE_VPP_V1_1_HIDLVPPSERVICE_H

#include <vendor/qti/hardware/vpp/1.1/IHidlVppService.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace vpp {
namespace V1_1 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::hardware::vpp::V1_1::IHidlVpp;
using ::vendor::qti::hardware::vpp::V1_1::IHidlVppService;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct HidlVppService : public IHidlVppService
{
    // Methods from ::vendor::qti::hardware::vpp::V1_1::IHidlVppService follow.
    Return<sp<IHidlVpp>> getNewVppSession(uint32_t flags) override;
};

}  // namespace implementation
}  // namespace V1_1
}  // namespace vpp
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_HARDWARE_VPP_V1_1_HIDLVPPSERVICE_H
