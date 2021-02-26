/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_HARDWARE_FACTORY_V1_0_IFACTORY_H
#define VENDOR_QTI_HARDWARE_FACTORY_V1_0_IFACTORY_H

#include <vendor/qti/hardware/factory/1.0/IFactory.h>
#include <hardware/hardware.h>
#include <hidl/Status.h>
#include <hidl/MQDescriptor.h>
#include <cutils/properties.h>

#define MMID_SERVICE_STATUS "init.svc.mmid"

namespace vendor {
namespace qti {
namespace hardware {
namespace factory {
namespace V1_0 {
namespace implementation {

using ::vendor::qti::hardware::factory::V1_0::IFactory;
using ::vendor::qti::hardware::factory::V1_0::FactoryResult;
using ::vendor::qti::hardware::factory::V1_0::IResultType;
using ::android::hidl::base::V1_0::IBase;
using ::android::hardware::hidl_string;
using ::android::hardware::Return;
using ::android::hardware::Void;

struct Factory : public IFactory {
    Factory();
    ~Factory();
    Return<void> runApp(const hidl_string& name,const hidl_string& params, bool isStart, runApp_cb _hidl_cb) override;
    Return<void> getSmbStatus(getSmbStatus_cb _hidl_cb) override;
    Return<bool> enterShipMode() override;
    Return<bool> chargerEnable(bool enable) override;
    Return<bool> wifiEnable(bool enable) override;

private:
};

extern "C" IFactory* HIDL_FETCH_IFactory(const char* name);

}
}  // namespace V1_0
}  // namespace factory
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif // VENDOR_QTI_HARDWARE_FACTORY_V1_0_IFACTORY_H
