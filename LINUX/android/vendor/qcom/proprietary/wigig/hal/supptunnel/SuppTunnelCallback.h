/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#ifndef VENDOR_QTI_HARDWARE_WIGIG_SUPPTUNNEL_V1_0_SUPPTUNNELCALLBACK_H
#define VENDOR_QTI_HARDWARE_WIGIG_SUPPTUNNEL_V1_0_SUPPTUNNELCALLBACK_H

#include <vendor/qti/hardware/wigig/supptunnel/1.0/ISuppTunnelCallback.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace wigig {
namespace supptunnel {
namespace V1_0 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::hardware::wigig::supptunnel::V1_0::ISuppTunnelCallback;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct SuppTunnelCallback : public ISuppTunnelCallback {
    // Methods from ::vendor::qti::hardware::wigig::supptunnel::V1_0::ISuppTunnelCallback follow.
    Return<void> onEvent(const hidl_string& evt) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.

};

extern "C" ISuppTunnelCallback* HIDL_FETCH_ISuppTunnelCallback(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace supptunnel
}  // namespace wigig
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_HARDWARE_WIGIG_SUPPTUNNEL_V1_0_SUPPTUNNELCALLBACK_H
