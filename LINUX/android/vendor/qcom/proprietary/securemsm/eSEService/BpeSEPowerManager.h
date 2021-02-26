/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <binder/Parcel.h>
#include "IeSEPowerManager.h"

namespace android {
    class BpeSEPowerManager: public BpInterface<IeSEPowerManager> {
        public:
            BpeSEPowerManager(const sp<IBinder>& impl);
            virtual int powerOn(const sp<IeSEPowerManagerCb> &notifier);
            virtual int powerOff();
            virtual int getState();
            virtual int killall();
    };

    class BneSEPowerManagerCb: public BnInterface<IeSEPowerManagerCb> {
        public:
            virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags = 0);
    };
}; // namespace android