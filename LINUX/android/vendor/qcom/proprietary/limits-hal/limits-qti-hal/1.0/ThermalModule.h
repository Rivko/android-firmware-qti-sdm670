/******************************************************************************
  @file  ThermalModule.h
  @brief vendor.qti.hardware.limits@1.0 IThermal HAL ThermalModule header.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#ifndef THERMAL_MODULE_H_
#define THERMAL_MODULE_H_

#include <hidl/HidlSupport.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace limits {
namespace V1_0 {
namespace implementation {

// Provides a class to manage thermal lib module loading/unloading.
class ThermalModule {
public:
    ThermalModule() = default;
    ~ThermalModule()
    {
        UnloadThermalLib();
    }
    int LoadThermalLib();
    void UnloadThermalLib();
    int RegisterThermReportCallback();
    void UnregisterThermReportCallback();
    bool IsRegistered()
    {
        return mRegistered;
    }
    void SetRegister(bool val)
    {
        mRegistered = val;
    }
    struct Thermal *parent;
protected:
    bool mRegistered;
    void *dlhandle;
    int phandle;
    int (*Register)(int (*callback)(char *, void *, void*), void *data);
    void (*UnRegister)(int);
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace limits
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
#endif  // THERMAL_MODULE_H_
