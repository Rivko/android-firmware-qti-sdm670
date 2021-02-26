/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#ifndef VENDOR_QTI_HARDWARE_WIGIG_SUPPTUNNEL_V1_0_SUPPTUNNELPROVIDER_H
#define VENDOR_QTI_HARDWARE_WIGIG_SUPPTUNNEL_V1_0_SUPPTUNNELPROVIDER_H

#include <vendor/qti/hardware/wigig/supptunnel/1.0/ISuppTunnelProvider.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include "SuppTunnel.h"

namespace vendor {
namespace qti {
namespace hardware {
namespace wigig {
namespace supptunnel {
namespace V1_0 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::hardware::wigig::supptunnel::V1_0::ISuppTunnel;
using ::vendor::qti::hardware::wigig::supptunnel::V1_0::ISuppTunnelProvider;
using ::vendor::qti::hardware::wigig::supptunnel::V1_0::SuppTunnelStatus;
using ::vendor::qti::hardware::wigig::supptunnel::V1_0::TunnelType;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

class SuppTunnelProvider : public ISuppTunnelProvider {
public:
    SuppTunnelProvider();

    // Methods from ::vendor::qti::hardware::wigig::supptunnel::V1_0::ISuppTunnelProvider follow.
    Return<void> getTunnel(TunnelType type, getTunnel_cb _hidl_cb) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.
private:
    std::map<TunnelType, android::sp<SuppTunnel>> mTunnels;
};

extern "C" ISuppTunnelProvider* HIDL_FETCH_ISuppTunnelProvider(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace supptunnel
}  // namespace wigig
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_HARDWARE_WIGIG_SUPPTUNNEL_V1_0_SUPPTUNNELPROVIDER_H
