/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <vendor/qti/hardware/alarm/1.0/IAlarm.h>
#include <hidl/LegacySupport.h>

#include "Alarm.h"

using vendor::qti::hardware::alarm::V1_0::IAlarm;
using android::hardware::defaultPassthroughServiceImplementation;

int main() {
    return defaultPassthroughServiceImplementation<IAlarm>();
}
