/* Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdlib.h>
#include <utils/RefBase.h>
#include <utils/Log.h>
#include <assert.h>
#include <binder/TextOutput.h>

#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/MemoryDealer.h>

#include "qti_audio_server.h"

using namespace android;
using namespace audiohal;



// Helper function to get a hold of the "qti_audio_server" service.
sp<Iqti_audio_server> get_qti_audio_server_() {
    sp<IServiceManager> sm = defaultServiceManager();
    assert(sm != 0);
    sp<IBinder> binder = sm->getService(String16(QTI_AUDIO_SERVER));
    // TODO: If the "qti_audio_server" service is not running, getService times out and binder == 0.
    assert(binder != 0);
    sp<Iqti_audio_server> qas = interface_cast<Iqti_audio_server>(binder);
    assert(qas != 0);
    return qas;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        defaultServiceManager()->addService(String16(QTI_AUDIO_SERVER), new qti_audio_server());
        android::ProcessState::self()->startThreadPool();
        printf("qti_audio_server service is added\n");
        IPCThreadState::self()->joinThreadPool();
        printf("qti_audio_server service thread joined\n");
    } else if (argc == 2) {
        sp<Iqti_audio_server> qas = get_qti_audio_server_();
        void *handle = qas->qahw_load_module("audio_hal");
        printf("qahw_load_module %d\n", (int *)handle);

        sp<MemoryDealer> mDealer = new MemoryDealer(1024, "qas");
        sp<IMemory> mMem = mDealer->allocate(1024);
        assert(mMem->size() !=0 );
        printf("allocated mem from memdealer %d\n", mMem->size());
        char *ptr = (char *)mMem->pointer();
        printf("allocated mem from memdealer %d %p\n", mMem->size(),ptr);
//        qas->prepare_for_writing(ptr);
//        printf("qahw_load_module %d", (int *)handle);
    }
    return 0;
}
