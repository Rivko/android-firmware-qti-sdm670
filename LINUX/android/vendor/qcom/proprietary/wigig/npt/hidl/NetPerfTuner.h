/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/
#ifndef VENDOR_QTI_HARDWARE_WIGIG_NETPERFTUNER_V1_0_NETPERFTUNER_H
#define VENDOR_QTI_HARDWARE_WIGIG_NETPERFTUNER_V1_0_NETPERFTUNER_H

#include <vendor/qti/hardware/wigig/netperftuner/1.0/INetPerfTuner.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include "CommandListener.h"

namespace vendor {
namespace qti {
namespace hardware {
namespace wigig {
namespace netperftuner {
namespace V1_0 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::hardware::wigig::netperftuner::V1_0::INetPerfTuner;
using ::vendor::qti::hardware::wigig::netperftuner::V1_0::NetPerfTunerStatus;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

class NetPerfTuner : public INetPerfTuner {
public:
    NetPerfTuner();
    NetPerfTuner(CommandListener *commandListener);
    // Methods from ::vendor::qti::hardware::wigig::netperftuner::V1_0::INetPerfTuner follow.
    Return<void> setTuningParameter(const hidl_string& param, const hidl_string& client, const hidl_string& value, setTuningParameter_cb _hidl_cb) override;
    Return<void> unsetTuningParameter(const hidl_string& param, const hidl_string& client, unsetTuningParameter_cb _hidl_cb) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.
private:
    NetPerfTunerStatusCode responseCodeToStatusCode(int code);
private:
    CommandListener *mCommandListener;
};

extern "C" INetPerfTuner* HIDL_FETCH_INetPerfTuner(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace netperftuner
}  // namespace wigig
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_HARDWARE_WIGIG_NETPERFTUNER_V1_0_NETPERFTUNER_H
