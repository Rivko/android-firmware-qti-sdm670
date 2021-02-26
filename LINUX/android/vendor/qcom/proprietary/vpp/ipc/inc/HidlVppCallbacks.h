/*!
 * @file HidlVppCallbacks.h
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services
 */

#ifndef VENDOR_QTI_HARDWARE_VPP_V1_1_HIDLVPPCALLBACKS_H
#define VENDOR_QTI_HARDWARE_VPP_V1_1_HIDLVPPCALLBACKS_H

#include <vendor/qti/hardware/vpp/1.1/IHidlVppCallbacks.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include "VppClient.h"

namespace vendor {
namespace qti {
namespace hardware {
namespace vpp {
namespace V1_1 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::hardware::vpp::V1_1::IHidlVppCallbacks;
using ::vendor::qti::hardware::vpp::V1_1::VppBuffer;
using ::vendor::qti::hardware::vpp::V1_1::VppEvent;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::VppClient;

struct HidlVppCallbacks : public IHidlVppCallbacks
{
    HidlVppCallbacks(VppClient *client);
    ~HidlVppCallbacks();
    // Methods from ::vendor::qti::hardware::vpp::V1_1::IHidlVppCallbacks follow.
    Return<uint32_t> inputBufferDone(const VppBuffer& buf) override;
    Return<uint32_t> outputBufferDone(const VppBuffer& buf) override;
    Return<uint32_t> vppEvent(const VppEvent& e) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.
    VppClient *mClient;
};

}  // namespace implementation
}  // namespace V1_1
}  // namespace vpp
}  // namespace qti
}  // namespace qualcomm
}  // namespace com

#endif  // VENDOR_QTI_HARDWARE_VPP_V1_1_HIDLVPPCALLBACKS_H
