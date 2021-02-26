/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <android/log.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <string>
#include <unistd.h>
#include <iostream>

#include <eSEPowerManager.h>

using namespace android;

#undef LOG_TAG
#define LOG_TAG "eSEPowerManagerClient"


// Helper function to get a hold of the service.
sp<IeSEPowerManager> getServ(std::string const &name) {

    sp<IeSEPowerManager> proxy = 0;
    do {
        sp<IServiceManager> sm = defaultServiceManager();
        if (sm == 0) {
            ALOGE("%s: NULL sm",__func__);
            break;
        }
        sp<IBinder> binder = sm->getService(String16(name.c_str()));
        if (binder == 0) {
            ALOGE("%s: NULL binder for service %s",__func__, name.c_str());
            break;
        }
        proxy = interface_cast<IeSEPowerManager>(binder);
        if (proxy == 0) {
            ALOGE("%s: NULL proxy",__func__);
            break;
        }
    } while (0);
    ALOGD("Returning proxy");

    return proxy;
}

int main(int argc, char **argv)
{
    ProcessState::initWithDriver("/dev/vndbinder");
    sp<IeSEPowerManager> proxy = getServ("eSEPowerManagerService");
    sp<eSEPowerManagerCb> cb = new eSEPowerManagerCb();
    ALOGD("eSEPowerClient called with %d arguments.", argc);
    if (argc == 1) {
        ALOGD("service->powerOn = %d", proxy->powerOn(cb));
        ALOGD("service->powerOff = %d", proxy->powerOff());
        return 0;
    }

    if (argv[1] == std::string("poweron")) {
        ALOGD("service->powerOn = %d", proxy->powerOn(cb));
        std::cin.ignore();
    } else if (argv[1] == std::string("poweroff")) {
        ALOGD("service->powerOff = %d", proxy->powerOff());
    } else if (argv[1] == std::string("killall")) {
        ALOGD("service->killall = %d", proxy->killall());
    } else if (argv[1] == std::string("getstate")) {
        ALOGD("service->getState = %d", proxy->getState());
    } else {
        ALOGD("service->powerOn = %d", proxy->powerOn(cb));
        ALOGD("service->powerOff = %d", proxy->powerOff());
    }

    return 0;
}
