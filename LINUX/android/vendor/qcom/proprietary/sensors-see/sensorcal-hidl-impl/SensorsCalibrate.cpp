/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <hardware/sensors.h>
#include <SensorsCalibrate.h>
#include <android-base/logging.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace sensorscalibrate {
namespace V1_0 {
namespace implementation {

SensorsCalibrate::SensorsCalibrate()
    : mInitCheck(android::NO_INIT),
      SensorCalModule(nullptr) {
    status_t err = android::OK;

    libsnscal = dlopen("libsensorcal.so", RTLD_NOW);
    if(!libsnscal) {
        LOG(ERROR) << "Couldn't dlopen"
                   << " sensorcal lib "
                   << strerror(-err);
        err = android::UNKNOWN_ERROR;
    }
    //Fix for static analysis error - uninitialized variable.
    if (err != android::OK) {
      LOG(ERROR) << "Couldn't load "
          << " sensorcal module - dlopen failed"
          << strerror(-err);

      mInitCheck = err;
      return;
    }
    SensorCalModule = (struct sensor_cal_module_t *) dlsym(libsnscal, SENSOR_CAL_MODULE_INFO_STR);
    if(!SensorCalModule) {
        LOG(ERROR) << "Couldn't loading"
                   << "symbol"
                   <<SENSOR_CAL_MODULE_INFO_STR
                   << strerror(-err);
        dlclose(libsnscal);
        err = android::UNKNOWN_ERROR;
    }

    if (err != android::OK) {
        LOG(ERROR) << "Couldn't load "
                   << " sensorcal module "
                   << strerror(-err);

        mInitCheck = err;
        return;
    }

    mInitCheck = android::OK;
}

std::string SensorsCalibrate::get_datatype(int32_t sensor_type) {

    std::string data_type;
    switch (sensor_type) {
    case SENSOR_TYPE_ACCELEROMETER:
         data_type = "accel";
         break;
    case SENSOR_TYPE_MAGNETIC_FIELD:
         data_type = "mag";
         break;
    case SENSOR_TYPE_GYROSCOPE:
         data_type = "gyro";
         break;
    case SENSOR_TYPE_LIGHT:
         data_type = "ambient_light";
         break;
    case SENSOR_TYPE_AMBIENT_TEMPERATURE:
         data_type ="ambient_temperature";
         break;
    case SENSOR_TYPE_PRESSURE:
         data_type = "pressure";
         break;
    case SENSOR_TYPE_PROXIMITY:
         data_type = "proximity";
         break;
    case SENSOR_TYPE_RELATIVE_HUMIDITY:
         data_type = "humidity";
         break;
    case SENSOR_TYPE_HEART_RATE:
         data_type = "heart_rate";
         break;
    case SENSOR_TYPE_HEART_BEAT:
         data_type = "heart_beat";
         break;
    default:
         break;
    }

    return data_type;

}

Return<void> SensorsCalibrate::SensorsCal(int32_t sensor_type, int8_t test_type,
                        SensorsCal_cb _hidl_cb) {

    hidl_string str_result;
    hidl_string data_type_tmp = get_datatype(sensor_type);
    if (!data_type_tmp.empty()) {
           str_result = SensorCalModule->methods->calibrate(data_type_tmp, test_type);
           _hidl_cb(str_result);
        }
        else {
           _hidl_cb("sensor type error");
        }
    return Void();
}


status_t SensorsCalibrate::initCheck() const {
    return mInitCheck;
}

ISensorsCalibrate *HIDL_FETCH_ISensorsCalibrate(const char *) {
    SensorsCalibrate *sensorscal = new SensorsCalibrate;
    if (sensorscal->initCheck() != android::OK) {
        delete sensorscal;
        sensorscal = nullptr;

        return nullptr;
    }

    return sensorscal;
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace sensorscalibrate
}  // namespace hardware
}  // namespace qti
}  // namespace vendor


