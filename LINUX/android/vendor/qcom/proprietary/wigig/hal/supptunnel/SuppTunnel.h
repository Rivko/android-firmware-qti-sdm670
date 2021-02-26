/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#ifndef VENDOR_QTI_HARDWARE_WIGIG_SUPPTUNNEL_V1_0_SUPPTUNNEL_H
#define VENDOR_QTI_HARDWARE_WIGIG_SUPPTUNNEL_V1_0_SUPPTUNNEL_H

#include <vendor/qti/hardware/wigig/supptunnel/1.0/ISuppTunnel.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include "SuppConnector.h"

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
using ::vendor::qti::hardware::wigig::supptunnel::V1_0::ISuppTunnelCallback;
using ::vendor::qti::hardware::wigig::supptunnel::V1_0::SuppTunnelStatus;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

class SuppTunnel : public ISuppTunnel, public ISuppConnectorCallback {
public:
    SuppTunnel(SuppConnector *connector);
    SuppTunnel();
    virtual ~SuppTunnel();

    // Methods from ::vendor::qti::hardware::wigig::supptunnel::V1_0::ISuppTunnel follow.
    Return<void> startSupplicant(startSupplicant_cb _hidl_cb) override;
    Return<void> killSupplicant(killSupplicant_cb _hidl_cb) override;
    Return<void> doCommand(const hidl_string& command, doCommand_cb _hidl_cb) override;
    Return<void> registerCallback(const sp<ISuppTunnelCallback>& callback, registerCallback_cb _hidl_cb) override;
    Return<void> setConfigOption(const hidl_string& name, const hidl_string& value, setConfigOption_cb _hidl_cb) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.

    // ISuppConnectorCallback
    void onEvent(char *buf, size_t buflen);
private:
    int addCallback(const sp<ISuppTunnelCallback> &callback);
    void removeCallback(const sp<ISuppTunnelCallback> &callback);
    void callWithEachCallback(const std::function<Return<void>(sp<ISuppTunnelCallback>)> &method);
private:
    sp<SuppConnector> mConnector;
    std::vector<sp<ISuppTunnelCallback>> mCallbacks;
};

extern "C" ISuppTunnel* HIDL_FETCH_ISuppTunnel(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace supptunnel
}  // namespace wigig
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_HARDWARE_WIGIG_SUPPTUNNEL_V1_0_SUPPTUNNEL_H
