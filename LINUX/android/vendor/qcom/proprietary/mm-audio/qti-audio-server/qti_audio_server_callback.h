/* Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#pragma once

#include <stdlib.h>
#include <utils/RefBase.h>
#include <utils/Log.h>
#include <binder/TextOutput.h>

#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>

#include <binder/IPCThreadState.h>

#include "qahw.h"

namespace audiohal {

using namespace android;

class IStreamOutCallback : public ::android::IInterface {
    public:
        DECLARE_META_INTERFACE(StreamOutCallback);
    virtual int event_cb(qahw_stream_callback_event_t event,
                                   void *param,
                                   void *cookie1,
                                   void *cookie2) = 0;
};


class QTIStreamOutCallback : public ::android::BnInterface<IStreamOutCallback> {
public:
    QTIStreamOutCallback(){};
    ~QTIStreamOutCallback(){};
private:
    int32_t onTransact(uint32_t code,
                                       const Parcel& data,
                                       Parcel* reply,
                                       uint32_t flags) override;
    int event_cb(qahw_stream_callback_event_t event,
                                   void *param,
                                   void *cookie1,
                                   void *cookie2) override;
};
}; // namespace audiohal

