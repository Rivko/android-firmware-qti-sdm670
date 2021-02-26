/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "BpeSEPowerManager.h"
#include "BneSEPowerManager.h"

namespace android {
    const char* IeSEPowerManager::getServiceName() {
        return "IeSEPowerManager";
    }

    IMPLEMENT_META_INTERFACE(eSEPowerManager, IeSEPowerManager::getServiceName());

    const char* IeSEPowerManagerCb::getServiceName() {
        return "IeSEPowerManagercb";
    }

    IMPLEMENT_META_INTERFACE(eSEPowerManagerCb, IeSEPowerManagerCb::getServiceName());

}; // namespace android