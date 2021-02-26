/* Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "qti_audio_server_callback"

#include <stdlib.h>
#include <utils/RefBase.h>
#include <utils/Log.h>
#include <binder/TextOutput.h>

#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/MemoryDealer.h>

#include <pthread.h>
#include "qti_audio_server_callback.h"

namespace audiohal {

using namespace android;

const android::String16 IStreamOutCallback::descriptor("IStreamOutCallback");
const android::String16& IStreamOutCallback::getInterfaceDescriptor() const {
    return IStreamOutCallback::descriptor;
}

class BpStreamOutCallback: public ::android:: BpInterface<IStreamOutCallback> {
public:
    BpStreamOutCallback(const sp<IBinder>& impl) :
        BpInterface<IStreamOutCallback>(impl) {
    ALOGD("BpStreamOutCallback::BpStreamOutCallback()");
        }

    int event_cb(qahw_stream_callback_event_t event,
                                   void *param,
                                   void *cookie1,
                                   void *cookie2) {
        ALOGV("%s %d", __func__,__LINE__);
        Parcel data, reply;
        data.writeInterfaceToken(IStreamOutCallback::getInterfaceDescriptor());
        data.write(&cookie1, sizeof(qahw_stream_callback_t *));
        data.write(&cookie2, sizeof(qahw_stream_callback_t *));
        data.write(&param, sizeof(qahw_stream_callback_t *));

        return remote()->transact(event, data, &reply);
    }
};

android::sp<IStreamOutCallback> IStreamOutCallback::asInterface
    (const android::sp<android::IBinder>& obj) {
    ALOGD("IStreamOutCallback::asInterface()");
    android::sp<IStreamOutCallback> intr;
    if (obj != NULL) {
        intr = static_cast<IStreamOutCallback*>(obj->queryLocalInterface(IStreamOutCallback::descriptor).get());
        ALOGD("IStreamOutCallback::asInterface() interface %s",
            ((intr == 0)?"zero":"non zero"));
        if (intr == NULL)
            intr = new BpStreamOutCallback(obj);
    }
    return intr;
}

IStreamOutCallback::IStreamOutCallback()
    { ALOGD("IStreamOutCallback::IStreamOutCallback()"); }
IStreamOutCallback::~IStreamOutCallback()
    { ALOGD("IStreamOutCallback::~IStreamOutCallback()"); }



int32_t QTIStreamOutCallback::onTransact(uint32_t code,
                                   const Parcel& data,
                                   Parcel* reply __unused,
                                   uint32_t flags) {
    ALOGD("QTIStreamOutCallback::onTransact(%i) %i", code, flags);
    data.checkInterface(this);
    void *cookie1, *cookie2, *param;
    data.read(&cookie1, sizeof(qahw_stream_callback_t *));
    data.read(&cookie2, sizeof(qahw_stream_callback_t *));
    data.read(&param, sizeof(qahw_stream_callback_t *));
    return
        event_cb((qahw_stream_callback_event_t)code, param, cookie1, cookie2);
}

int QTIStreamOutCallback::event_cb(qahw_stream_callback_event_t event,
                                   void *param,
                                   void *cookie1,
                                   void *cookie2) {
    qahw_stream_callback_t *callback = (qahw_stream_callback_t *)cookie1;
    ALOGD("Calling Client Registered Callback(%p) for event %d",
        callback, event);
    return callback(event, param, cookie2);
}

}; // namespace audiohal

