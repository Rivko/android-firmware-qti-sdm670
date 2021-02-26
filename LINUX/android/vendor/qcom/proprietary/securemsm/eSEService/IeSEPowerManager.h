/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <map>

#include <binder/IInterface.h>

namespace android {

    class IeSEPowerManagerCb: public IInterface {
        public:
            static const char* getServiceName();
        enum {
            NOTIFY_CALLBACK = IBinder::FIRST_CALL_TRANSACTION,
        };

        virtual void notifyCallback(int32_t event) = 0;
        DECLARE_META_INTERFACE(eSEPowerManagerCb);
    };

    class IeSEPowerManager: public IInterface {
        public:
            static const char* getServiceName();
        enum {
            TRANSACTION_POWERON = IBinder::FIRST_CALL_TRANSACTION,
            TRANSACTION_POWEROFF,
            TRANSACTION_GETSTATE,
            TRANSACTION_KILLALL,
        };
        virtual int powerOn(const sp<IeSEPowerManagerCb> &notifier) = 0;
        virtual int powerOff() = 0;
        virtual int getState() = 0;
        virtual int killall() = 0;

        DECLARE_META_INTERFACE(eSEPowerManager);
    };
}; // namespace android