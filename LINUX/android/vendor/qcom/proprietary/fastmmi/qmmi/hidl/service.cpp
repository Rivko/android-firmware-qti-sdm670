/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <vendor/qti/hardware/factory/1.0/IFactory.h>
#include <hidl/LegacySupport.h>

#include "Factory.h"

using vendor::qti::hardware::factory::V1_0::IFactory;
using android::hardware::defaultPassthroughServiceImplementation;

int main() {
    return defaultPassthroughServiceImplementation<IFactory>();
}
