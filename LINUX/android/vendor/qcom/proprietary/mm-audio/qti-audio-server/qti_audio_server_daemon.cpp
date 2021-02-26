/* Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdlib.h>
#include <utils/RefBase.h>
#include <utils/Log.h>
#include <assert.h>

#include <cutils/properties.h>

#include "qti_audio_server.h"

using namespace audiohal;

int main(int argc, char **argv) {
    bool g_binder_enabled = false;
    g_binder_enabled = property_get_bool("persist.vendor.audio.qas.enabled", false);
    if (g_binder_enabled) {
        sp<IServiceManager> sm = defaultServiceManager();
        assert(sm != 0);
        sm->addService(String16(QTI_AUDIO_SERVER), new qti_audio_server(), false);
        ALOGD("QTI Audio Server is alive");
        android::ProcessState::self()->startThreadPool();
        IPCThreadState::self()->joinThreadPool();
    } else {
        ALOGD("QTI Audio Server is disabled");
    }
}
