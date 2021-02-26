/******************************************************************************
  @file  Thermal.h
  @brief vendor.qti.hardware.limits@1.0 IThermal HAL module header

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#ifndef VENDOR_QTI_HARDWARE_LIMITS_V1_0_THERMAL_H
#define VENDOR_QTI_HARDWARE_LIMITS_V1_0_THERMAL_H

#include <vendor/qti/hardware/limits/1.0/IThermal.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include "ThermalModule.h"
#include "ThermalCallbackHandler.h"


namespace vendor {
namespace qti {
namespace hardware {
namespace limits {
namespace V1_0 {
namespace implementation {

using ::android::hardware::thermal::V1_0::ThermalStatus;
using ::vendor::qti::hardware::limits::V1_0::IThermal;
using ::vendor::qti::hardware::limits::V1_0::IThermalCallback;
using ::android::hardware::hidl_string;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct Thermal : public IThermal {
private:
    ThermalCallbackHandler<IThermalCallback> thermal_cb_handler;
    ThermalModule thermModule;
    void notifyEachThermalClient(const std::function<Return<void>(sp<IThermalCallback>)> &method);
public:
    Thermal();
    ~Thermal();
    // Methods from IThermal follow.
    Return<void> registerCallback(const sp<IThermalCallback>& callback, registerCallback_cb _hidl_cb) override;
    Return<void> unregisterCallback(const sp<IThermalCallback>& callback) override;
    // Methods from ::android::hidl::base::V1_0::IBase follow.
    int ThermalClientReportCallback(char *buf, void *data, void *reserved);
};

extern "C" IThermal* HIDL_FETCH_IThermal(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace limits
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_HARDWARE_LIMITS_V1_0_THERMAL_H
