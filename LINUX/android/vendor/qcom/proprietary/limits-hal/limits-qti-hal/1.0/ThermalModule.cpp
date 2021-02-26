/******************************************************************************
  @file  ThermalModule.cpp
  @brief vendor.qti.hardware.limits@1.0 IThermal HAL module to
         communicate between thermal framework and HAL service.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#include <dlfcn.h>
#include <cutils/log.h>
#include "ThermalModule.h"
#include "Thermal.h"

#define LOG_TAG "vendor.qti.hardware.limits@1.0-ThermalModule"

namespace vendor {
namespace qti {
namespace hardware {
namespace limits {
namespace V1_0 {
namespace implementation {

#define THERMAL_CLIENT_LIB   "libthermalclient.so"

Thermal *g_thermal_ptr;

//ThermalModule methods implementation
int ThermalModule::LoadThermalLib() {
    const char *rc = NULL;
    int ret = 0;

    /* Initialize service class object pointer */
    g_thermal_ptr = parent;

    SetRegister(false);
    dlhandle = dlopen(THERMAL_CLIENT_LIB, RTLD_NOW | RTLD_LOCAL);
    if (dlhandle == NULL) {
        ALOGE("%s Failed to (dl)open %s error:%s\n",
		__func__, THERMAL_CLIENT_LIB, dlerror());
        ret = -1;
        return ret;
    }
    dlerror();

    *(void **) (&Register) = dlsym(dlhandle,
                    "thermal_client_register_report_callback");
    if ((rc = dlerror()) != NULL) {
        ALOGE("%s Failed to get register callback API symbol:%s\n",
                __func__, rc);
        dlclose(dlhandle);
        dlhandle = NULL;
        ret = -1;
        return ret;
    }

    *(void **) (&UnRegister) = dlsym(dlhandle,
                    "thermal_client_unregister_callback");
    if ((rc = dlerror()) != NULL) {
        ALOGE("%s Failed to get unregister callback API symbol:%s\n",
               __func__, rc);
        dlclose(dlhandle);
        dlhandle = NULL;
        ret = -1;
        return ret;
    }

    return 0;
}

void ThermalModule::UnloadThermalLib() {
    /* Uninitialize thermal client library related */
    g_thermal_ptr = nullptr;
    if (IsRegistered())
        UnregisterThermReportCallback();
    dlclose(dlhandle);
    dlhandle = NULL;
    Register = NULL;
    UnRegister = NULL;
    phandle = 0;
}

static int GlueThermalclientReportCallback(char *buf, void *data,
                                             void *reserved) {
    if (g_thermal_ptr)
        g_thermal_ptr->ThermalClientReportCallback(buf, data, reserved);
    return 0;
}

int ThermalModule::RegisterThermReportCallback() {
    int ret = 0;

    if (!dlhandle || !Register) {
        ALOGE("%s: %s is not loaded\n", __func__, "libthermalclient.so");
        ret = -1;
        return ret;
    }
    if (IsRegistered())
        return ret;

    phandle = Register(GlueThermalclientReportCallback, NULL);
    if (phandle <= 0) {
        ALOGE("%s registration failed\n", __func__);
        ret = -1;
        return ret;
    }
    SetRegister(true);

    return ret;
}

void ThermalModule::UnregisterThermReportCallback() {
    if (!dlhandle || !Register || !UnRegister|| phandle <= 0)
        return;

    UnRegister(phandle);
    SetRegister(false);
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace limits
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
