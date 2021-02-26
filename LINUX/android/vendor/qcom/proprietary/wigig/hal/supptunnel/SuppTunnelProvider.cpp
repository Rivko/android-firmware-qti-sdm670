/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#include "SuppTunnelProvider.h"
#include "WigigSuppConnector.h"

namespace vendor {
namespace qti {
namespace hardware {
namespace wigig {
namespace supptunnel {
namespace V1_0 {
namespace implementation {

SuppTunnelProvider::SuppTunnelProvider()
{
    // tunnel for the wigig_supplicant
    WigigSuppConnector *wigigSuppConnector = new WigigSuppConnector();
    wigigSuppConnector->init();
    mTunnels[TunnelType::WIGIG_SUPPLICANT] = new SuppTunnel(wigigSuppConnector);
    // tunnel for wigig_hostapd
    WigigHostapdConnector *wigigHostapdConnector = new WigigHostapdConnector();
    wigigHostapdConnector->init();
    mTunnels[TunnelType::WIGIG_HOSTAPD] = new SuppTunnel(wigigHostapdConnector);
}

// Methods from ::vendor::qti::hardware::wigig::supptunnel::V1_0::ISuppTunnelProvider follow.
Return<void> SuppTunnelProvider::getTunnel(TunnelType type, getTunnel_cb _hidl_cb) {
    android::sp<SuppTunnel> tunnel;
    SuppTunnelStatus status = {SuppTunnelStatusCode::FAILURE_UNKNOWN, ""};

    auto tunnel_iter = mTunnels.find(type);
    if (tunnel_iter == mTunnels.end()) {
        status = {SuppTunnelStatusCode::FAILURE_TUNNEL_UNKNOWN, ""};
    } else {
        tunnel = tunnel_iter->second;
        status = {SuppTunnelStatusCode::SUCCESS, ""};
    }

    _hidl_cb(status, tunnel);
    return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

ISuppTunnelProvider* HIDL_FETCH_ISuppTunnelProvider(const char* /* name */) {
    return new SuppTunnelProvider();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace supptunnel
}  // namespace wigig
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
