/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#include "SuppTunnelCallback.h"

namespace vendor {
namespace qti {
namespace hardware {
namespace wigig {
namespace supptunnel {
namespace V1_0 {
namespace implementation {

// Methods from ::vendor::qti::hardware::wigig::supptunnel::V1_0::ISuppTunnelCallback follow.
Return<void> SuppTunnelCallback::onEvent(const hidl_string& evt) {
    // TODO implement
    return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

ISuppTunnelCallback* HIDL_FETCH_ISuppTunnelCallback(const char* /* name */) {
    return new SuppTunnelCallback();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace supptunnel
}  // namespace wigig
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
