/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_V1_1_LOCHIDLDEBUGREPORTSERVICE_H
#define VENDOR_QTI_GNSS_V1_1_LOCHIDLDEBUGREPORTSERVICE_H

#include <vendor/qti/gnss/1.1/ILocHidlDebugReportService.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include "SystemStatus.h"

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_1 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::gnss::V1_1::ILocHidlDebugReportService;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

using namespace loc_core;
struct LocHidlDebugReportService : public ILocHidlDebugReportService {
    // Methods from ::vendor::qti::gnss::V1_1::ILocHidlDebugReportService follow.
    Return<bool> init() override;
    Return<bool> deinit() override;
    Return<void> getReport(int32_t maxReports, getReport_cb _hidl_cb) override;
    Return<void> getReport_1_1(int32_t maxReports, getReport_1_1_cb _hidl_cb) override;

private:
    SystemStatus* mSystemStatus;

private:
    void getReport(LocHidlSystemStatusReports& hidlReports,
            int32_t maxReports);
    void populateLocationReport(
            LocHidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateTimeAndClockReport(
            LocHidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateXoStateReport(
            LocHidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateRfParamsReport(
            LocHidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateErrRecoveryReport(
            LocHidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateInjectedPositionReport(
            LocHidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateBestPositionReport(
            LocHidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateXtraReport(
            LocHidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateEphemerisReport(
            LocHidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateSvHealthReport(
            LocHidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populatePdrReport(
            LocHidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateNavDataReport(
            LocHidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populatePositionFailureReport(
            LocHidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
};

}  // namespace implementation
}  // namespace V1_1
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_GNSS_V1_1_LOCHIDLDEBUGREPORTSERVICE_H
