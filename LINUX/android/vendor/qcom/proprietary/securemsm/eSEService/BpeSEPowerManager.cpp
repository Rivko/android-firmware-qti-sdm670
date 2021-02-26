/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef LOG_TAG
#define LOG_TAG "eSEPowerManager"
#endif

#include <binder/Parcel.h>
#include "BpeSEPowerManager.h"

namespace android {

    int BpeSEPowerManager::powerOn(const sp<IeSEPowerManagerCb> &notifier) {
        Parcel data, reply;
        int32_t res;
        data.writeInterfaceToken(BpeSEPowerManager::getInterfaceDescriptor());
        data.writeStrongBinder(asBinder(notifier));
        res = remote()->transact(TRANSACTION_POWERON, data, &reply);
        if (res != NO_ERROR) {
          ALOGE("%s: Couldn't contact remote: %d",__func__, res);
          return res;
        }
        res = reply.readExceptionCode();
        if (res < 0) {
            ALOGE("%s: ESEPowerOn : exception caught : %d",__func__, res);
            return res;
        }
        res = reply.readInt32();
        return res;
    }

    int BpeSEPowerManager::powerOff() {

        Parcel data, reply;
        int32_t res;
        data.writeInterfaceToken(BpeSEPowerManager::getInterfaceDescriptor());
        res = remote()->transact(TRANSACTION_POWEROFF, data, &reply);
        if (res != NO_ERROR) {
          ALOGE("%s: Couldn't contact remote: %d",__func__, res);
          return res;
        }
        res = reply.readExceptionCode();
        if (res < 0) {
            ALOGE("%s: ESEPowerOff : exception caught : %d",__func__, res);
            return res;
        }
        res = reply.readInt32();
        return res;
    }

    int BpeSEPowerManager::getState() {

        Parcel data, reply;
        int32_t res;
        data.writeInterfaceToken(BpeSEPowerManager::getInterfaceDescriptor());
        res = remote()->transact(TRANSACTION_GETSTATE, data, &reply);
        if (res != NO_ERROR) {
          ALOGE("%s: Couldn't contact remote: %d",__func__, res);
          return res;
        }
        res = reply.readExceptionCode();
        if (res < 0) {
            ALOGE("%s: ESEgetState : exception caught : %d",__func__, res);
            return res;
        }
        res = reply.readInt32();
        return res;
    }

    int BpeSEPowerManager::killall() {

        Parcel data, reply;
        int32_t res;
        data.writeInterfaceToken(BpeSEPowerManager::getInterfaceDescriptor());
        res = remote()->transact(TRANSACTION_KILLALL, data, &reply);
        if (res != NO_ERROR) {
          ALOGE("%s: Couldn't contact remote: %d",__func__, res);
          return res;
        }
        res = reply.readExceptionCode();
        if (res < 0) {
            ALOGE("%s: ESEkillall : exception caught : %d",__func__, res);
            return res;
        }
        res = reply.readInt32();
        return res;
    }

    BpeSEPowerManager::BpeSEPowerManager(const sp<IBinder>& impl) : BpInterface<IeSEPowerManager>(impl){}

    status_t BneSEPowerManagerCb::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
    {
        status_t ret = UNKNOWN_ERROR;

        switch(code) {

            case NOTIFY_CALLBACK: {
                CHECK_INTERFACE(IeSEPowerManagerCb, data, reply);
                int32_t event = data.readInt32();
                notifyCallback(event);
                reply->writeNoException();
            }
            break;

            default:
                ALOGD("Unknown binder command ID: %d", code);
                return BBinder::onTransact(code, data, reply, flags);
        }
        return ret;
    }
};