/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "BneSEPowerManager.h"
#include <binder/IPCThreadState.h>
#include <mutex>

#ifndef LOG_TAG
#define LOG_TAG "eSEPowerManager"
#endif

namespace android {

    status_t BneSEPowerManager::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
    {
        static std::mutex m;
        std::lock_guard<std::mutex> lock(m);
        status_t retStatus = INVALID_OPERATION;
        int32_t ret = -1;
        IPCThreadState* self = IPCThreadState::self();
        int pid = self->getCallingPid();

        switch(code) {
            case TRANSACTION_POWERON: {
                CHECK_INTERFACE(IeSEPowerManager, data, reply);
                sp<IeSEPowerManagerCb> notifier = interface_cast <IeSEPowerManagerCb> (data.readStrongBinder());
                ret = powerOn(notifier);
                reply->writeNoException();
                reply->writeInt32(ret);
                break;
            }
            case TRANSACTION_POWEROFF: {
                CHECK_INTERFACE(IeSEPowerManager, data, reply);
                ret = powerOff();
                reply->writeNoException();
                reply->writeInt32(ret);
                break;
            }
            case TRANSACTION_GETSTATE: {
                CHECK_INTERFACE(IeSEPowerManager, data, reply);
                ret = getState();
                reply->writeNoException();
                reply->writeInt32(ret);
                break;
            }
#ifdef eSEClient_Debug
            case TRANSACTION_KILLALL: {
                CHECK_INTERFACE(IeSEPowerManager, data, reply);
                ret = killall();
                reply->writeNoException();
                reply->writeInt32(ret);
                break;
            }
#endif
            default: {
                retStatus = BBinder::onTransact(code, data, reply, flags);
                break;
            }
        }
        return NO_ERROR;
    }

    BpeSEPowerManagerCb::BpeSEPowerManagerCb(const sp<IBinder>& impl) : BpInterface<IeSEPowerManagerCb>(impl){}

    void BpeSEPowerManagerCb::notifyCallback(int32_t event) {
        Parcel data, reply;
        status_t rv = NO_ERROR;
        data.writeInterfaceToken(BpeSEPowerManagerCb::getInterfaceDescriptor());
        data.writeInt32(event);
        // binder call
        rv = remote()->transact(NOTIFY_CALLBACK, data, &reply);
        if (rv != NO_ERROR) {
          ALOGE("%s : Couldn't contact remote: %d",__func__, rv);
          return;
        }
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
          ALOGE("%s: remote exception: %d",__func__, err);
          return;
        }
    }

};