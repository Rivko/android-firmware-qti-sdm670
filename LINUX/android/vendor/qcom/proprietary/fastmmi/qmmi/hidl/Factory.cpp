/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "Factory.h"
#include "socket.h"

namespace vendor {
namespace qti {
namespace hardware {
namespace factory {
namespace V1_0 {
namespace implementation {

Factory::Factory(){

}

Factory::~Factory() {
}

/*
* Start App test via command
*
*/
Return<void> Factory::runApp(const hidl_string& name,const hidl_string& params, bool isStart, runApp_cb _hidl_cb) {
    ALOGI("start to runApp with: %s, start: %d",name.c_str(),isStart);
    msg_t *msg = NULL;
    FactoryResult factoryResult;

    if(check_socket_ready()){

        if(isStart)
            msg = run_app(name.c_str(),params.c_str(),params.size());
        else
            msg = stop_app(name.c_str());
    }else{
           ALOGE("socket not ready");
           factoryResult.result = IResultType::FAILED;
    }

    factoryResult.result = IResultType::SUCCESS;
     _hidl_cb(factoryResult);
    return Void();
}

/*
* Get SMB status
*
*/
Return<void> Factory::getSmbStatus(getSmbStatus_cb _hidl_cb) {
    ALOGI("start to getSmbStatus");
    FactoryResult factoryResult;
    msg_t *t = smb_status();
    if(t->result == 0)
        factoryResult.result = IResultType::SUCCESS;
    else
        factoryResult.result = IResultType::FAILED;

    _hidl_cb(factoryResult);
    return Void();
}

/*
* purpose
*     enter ship mode
*
* requires
*
* guarantees
*       set /sys/module/qpnp_power_on/parameters/ship_mode_en = 1
*       return true when the value set
*/
Return<bool> Factory::enterShipMode() {
    ALOGI("start to enterShipMode");
    return enter_ship_mode();
}
/*
* purpose
*     enable/diable charger
*
* requires
*
* guarantees
*       set charger diable prop = 1
*       return true when the value set
*/
Return<bool> Factory::chargerEnable(bool enable) {
    ALOGI("start to chargerEnable enable: %d",enable);
    charger_enable(enable);
    return true;
}
/*
* purpose
*     enable/diable charger
*
* requires
*
* guarantees
*       set charger diable prop = 1
*       return true when the value set
*/
Return<bool> Factory::wifiEnable(bool enable) {
    ALOGI("start to wifiEnable enable: %d",enable);
    wifi_enable(enable);
    return true;
}

IFactory* HIDL_FETCH_IFactory(const char* /* name */) {
    ALOGI("start to HIDL_FETCH_IFactory");
    create_socket_thread();
    return new Factory();
}

}
}  // namespace V1_0
}  // namespace factory
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

