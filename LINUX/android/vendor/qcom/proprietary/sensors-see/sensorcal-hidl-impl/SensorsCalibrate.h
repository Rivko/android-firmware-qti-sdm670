/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef VENDOR_QTI_HARDWARE_SENSORSCAIBRATE_V1_0_ISENSORSCALIBRATE_H
#define VENDOR_QTI_HARDWARE_SENSORSCAIBRATE_V1_0_ISENSORSCALIBRATE_H

#include <vendor/qti/hardware/sensorscalibrate/1.0/ISensorsCalibrate.h>
#include <hidl/Status.h>
#include <hidl/MQDescriptor.h>
#include <log/log.h>
#include "sensors_calibrate.h"
#include <dlfcn.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.hardware.sensorscalibrate@1.0-impl"

namespace vendor {
namespace qti {
namespace hardware {
namespace sensorscalibrate {
namespace V1_0 {
namespace implementation {

using ::vendor::qti::hardware::sensorscalibrate::V1_0::ISensorsCalibrate;
using ::android::hidl::base::V1_0::IBase;
using ::android::hardware::hidl_string;
using ::android::hardware::Return;
using ::android::hardware::Void;
using android::status_t;


struct SensorsCalibrate : public ISensorsCalibrate {
    SensorsCalibrate();
    status_t initCheck() const;

    Return<void> SensorsCal(int32_t sensor_type, int8_t test_type, SensorsCal_cb _hidl_cb) override;

private:
    status_t mInitCheck;
    sensor_cal_module_t *SensorCalModule;
    void *libsnscal;
    std::string get_datatype(int32_t sensor_type);

};

extern "C" ISensorsCalibrate* HIDL_FETCH_ISensorsCalibrate(const char *name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace sensorscalibrate
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_HARDWARE_SENSORSCAIBRATE_V1_0_ISENSORSCALIBRATE_H
