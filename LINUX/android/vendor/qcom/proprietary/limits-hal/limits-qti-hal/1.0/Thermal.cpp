/******************************************************************************
  @file  Thermal.cpp
  @brief vendor.qti.hardware.limits@1.0 IThermal HAL module

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#include <cutils/log.h>
#include "Thermal.h"

#define LOG_TAG "vendor.qti.hardware.limits@1.0-Thermal"

namespace vendor {
namespace qti {
namespace hardware {
namespace limits {
namespace V1_0 {
namespace implementation {

using ::android::hardware::thermal::V1_0::ThermalStatusCode;

Thermal::Thermal() {
    /* Initialize global ptr */
    thermModule.parent = this;
    thermModule.LoadThermalLib();
}

Thermal::~Thermal() {
    /* Clear global pointer */
    thermModule.parent = nullptr;
    /* Remove all callback */
    thermal_cb_handler.removeAllCallback();
    thermModule.UnloadThermalLib();
}

// Methods from ::vendor::qti::hardware::limits::V1_0::IThermal follow.
Return<void> Thermal::registerCallback(const sp<IThermalCallback>& callback,
                                       registerCallback_cb _hidl_cb) {
    ThermalStatus status;

    status.code = ThermalStatusCode::SUCCESS;

    if (thermal_cb_handler.getCallbacks().empty()) {
        if (thermModule.RegisterThermReportCallback()) {
            status.code = ThermalStatusCode::FAILURE;
            ALOGE("Register thermal report callback failed\n");
        }
    }

    if (status.code == ThermalStatusCode::SUCCESS) {
        if (!thermal_cb_handler.addCallback(callback)) {
            ALOGE("failed to add callback");
            status.code = ThermalStatusCode::FAILURE;
        }
    }

    _hidl_cb(status);
    return Void();
}

Return<void> Thermal::unregisterCallback(const sp<IThermalCallback>& callback) {

    thermal_cb_handler.removeCallback(callback);
    if (thermal_cb_handler.getCallbacks().empty())
        thermModule.UnregisterThermReportCallback();

    return Void();
}

int Thermal::ThermalClientReportCallback(char *buf, void *data,
                                         void *reserved) {
    ThermalReport report;
    hidl_string evt;
    std::string sensor, temp, level, trigger, rule_name;
    std::istringstream is_buf(buf);

    if (!buf) {
        ALOGE("invalid input buffer\n");
        return 0;
    }

    evt = hidl_string(buf, strlen(buf));

    /* Parse buffer and fill out ThermalReport */
    std::getline(is_buf, sensor);
    std::getline(is_buf, temp);
    std::getline(is_buf, level);
    std::getline(is_buf, trigger);
    std::getline(is_buf, rule_name);
    report.sensor = hidl_string(sensor.c_str(), strlen(sensor.c_str()));
    report.rule_name = hidl_string(rule_name.c_str(), strlen(rule_name.c_str()));
    report.temperature = atoi(temp.c_str());
    report.level = atoi(level.c_str());
    if (!strcmp(trigger.c_str(), "true"))
        report.is_trigger = 1;
    else
        report.is_trigger = 0;

    ALOGD("Report data received is:%s\n", evt.c_str());
    // Invoke the |ThermalCalback| method on all registered callbacks.
    notifyEachThermalClient(std::bind(
        &IThermalCallback::onThermReport, std::placeholders::_1,
        evt, report));

    return 0;
}

void Thermal::notifyEachThermalClient(
    const std::function<Return<void>(sp<IThermalCallback>)> &method) {
    thermal_cb_handler.lock ();
    for (const auto &callback : thermal_cb_handler.getCallbacks()) {
        if (!method(callback).isOk()) {
            ALOGE("Failed to invoke HIDL callback\n");
        }
    }
    thermal_cb_handler.unlock ();
}

/* To support passthrough mode */
IThermal* HIDL_FETCH_IThermal(const char* /* name */) {
    return new Thermal();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace limits
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
