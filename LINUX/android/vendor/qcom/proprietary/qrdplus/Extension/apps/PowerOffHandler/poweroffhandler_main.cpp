/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "PowerOffHandler"

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <cutils/properties.h>
#include <sys/resource.h>
#include <utils/Log.h>
#include <utils/threads.h>

#include "PowerOffHandler.h"

using namespace android;

// ---------------------------------------------------------------------------

int main() {
    setpriority(PRIO_PROCESS, 0, ANDROID_PRIORITY_URGENT_DISPLAY);

    sp <ProcessState> proc(ProcessState::self());
    ProcessState::self()->startThreadPool();
    // create the powerhandler object
    sp <PowerOffHandler> boot = new PowerOffHandler();
    IPCThreadState::self()->joinThreadPool();

    return 0;
}
