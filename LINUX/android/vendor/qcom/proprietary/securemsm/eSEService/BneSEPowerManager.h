/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <binder/Parcel.h>
#include "IeSEPowerManager.h"

namespace android {
    class BneSEPowerManager: public BnInterface<IeSEPowerManager> {
        public:
            virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flag = 0);
    };

    class BpeSEPowerManagerCb: public BpInterface<IeSEPowerManagerCb> {
        public:
            BpeSEPowerManagerCb(const sp<IBinder>& impl);
            virtual void notifyCallback(int32_t event);
    };
}; // namespace android