/**
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "EsePowerManager.h"
#include "QSEEComAPI.h"
#include <utils/Log.h>
#define LOG_TAG "vendor.qti.esepowermanager@1.0-service"

#include <vendor/qti/esepowermanager/1.0/IEsePowerManager.h>
#include <hidl/LegacySupport.h>

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using vendor::qti::esepowermanager::V1_0::IEsePowerManager;
using android::hardware::registerPassthroughServiceImplementation;
using android::OK;
static char const app_name[] = "eseservice";
static char const app_path[] = "/firmware/image";
static struct QSEECom_handle* qseeComHandleeSEService = NULL;

int main()
{
    ALOGD("loading eseservice");
    int32_t ret = QSEECom_start_app(&qseeComHandleeSEService, app_path, app_name, 1024);
    if (ret) {
        ALOGD("loading eseservice failed");
        //Some customers may want to use the esepowermanager without the eseservice (SPI through HLOS)
    }
    ALOGD("eSEPowerManager is starting");
    configureRpcThreadpool(10, true /*callerWillJoin*/);
    android::status_t status;
    status = registerPassthroughServiceImplementation<IEsePowerManager>();
    LOG_ALWAYS_FATAL_IF(status != OK, "Error while registering nfc AOSP service: %d", status);
    joinRpcThreadpool();
    return status;
}
