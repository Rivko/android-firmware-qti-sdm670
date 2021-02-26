/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_HARDWARE_ALARM_V1_0_IALARM_H
#define VENDOR_QTI_HARDWARE_ALARM_V1_0_IALARM_H

#include <vendor/qti/hardware/alarm/1.0/IAlarm.h>
#include <hardware/hardware.h>
#include <hidl/Status.h>
#include <hidl/MQDescriptor.h>
#include <log/log.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.hardware.alarm@1.0-impl"

namespace vendor {
namespace qti {
namespace hardware {
namespace alarm {
namespace V1_0 {
namespace implementation {

using ::vendor::qti::hardware::alarm::V1_0::IAlarm;

using ::android::hidl::base::V1_0::IBase;
using ::android::hardware::Return;
using ::android::hardware::Void;

struct Alarm : public IAlarm {
    Return<int32_t> setAlarm(int64_t time) override;
    Return<int64_t> getAlarm() override;
    Return<int32_t> cancelAlarm() override;
    Return<int64_t> getRtcTime() override;
};

extern "C" IAlarm* HIDL_FETCH_IAlarm(const char* name);

}
}  // namespace V1_0
}  // namespace alarm
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif // VENDOR_QTI_HARDWARE_ALARM_V1_0_IALARM_H
