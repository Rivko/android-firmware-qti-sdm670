/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "eSEPowerManagerServer"

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

#include "eSEPowerManager.h"
#include "QSEEComAPI.h"

using namespace android;

static char const app_name[] = "eseservice";
static char const app_path[] = "/system/etc/firmware";
static struct QSEECom_handle* qseeComHandleeSEService = NULL;
int32_t ret = 0;

int main()
{
    ALOGD("loading eseservice");
    ret = QSEECom_start_app(&qseeComHandleeSEService, app_path, app_name, 1024);
    if (ret) {
        ALOGE("loading eseservice failed");
    }
    ALOGD("%s is starting", eSEPowerManager::getServiceName());
    ProcessState::initWithDriver("/dev/vndbinder");
    sp<ProcessState> proc(ProcessState::self());
    ProcessState::self()->startThreadPool();
    defaultServiceManager()->addService(String16("eSEPowerManagerService"), new eSEPowerManager);
    IPCThreadState::self()->joinThreadPool();
    return 0;
}
