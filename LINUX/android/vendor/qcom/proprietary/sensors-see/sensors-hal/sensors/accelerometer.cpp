/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_resampler.pb.h"

static const char *SSC_DATATYPE_ACCEL = "accel";
static const uint32_t ACCEL_RESERVED_FIFO_COUNT = 3000;
static const float ONE_G = 9.80665f;

using namespace std;

class accelerometer : public ssc_sensor
{
public:
    accelerometer(sensor_uid suid, sensor_wakeup_type wakeup,
                                        sensor_cal_type calibrated);
    static const char* ssc_datatype() { return SSC_DATATYPE_ACCEL; }
    virtual bool is_calibrated() override;

private:
    virtual void handle_sns_std_sensor_event(
        const sns_client_event_msg_sns_client_event& pb_event) override;

    sensor_cal_type _cal_type;
};

accelerometer::accelerometer(sensor_uid suid,
                                sensor_wakeup_type wakeup,
                                sensor_cal_type cal_type):
    ssc_sensor(suid, wakeup)
{
    if (cal_type == SENSOR_UNCALIBRATED) {
        set_type(SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED);
        set_string_type(SENSOR_STRING_TYPE_ACCELEROMETER_UNCALIBRATED);
        set_sensor_typename("Accelerometer-Uncalibrated");
    } else {
        set_type(SENSOR_TYPE_ACCELEROMETER);
        set_string_type(SENSOR_STRING_TYPE_ACCELEROMETER);
        set_sensor_typename("Accelerometer");
    }
#ifdef SNS_DIRECT_REPORT_SUPPORT
    if (check_direct_channel_support()) {
        set_direct_channel_flag(true);
    }
#endif
    _cal_type = cal_type;
    set_fifo_reserved_count(ACCEL_RESERVED_FIFO_COUNT);
    set_resampling(true);
    set_nowk_msgid(SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG_EVENT);

    /* convert range from Gs to m/s^2 */
    set_max_range(get_sensor_info().maxRange * ONE_G);
    /* convert resolution from mG to m/s^2 */
    set_resolution(get_sensor_info().resolution * ONE_G / 1000.0);
    set_nowk_msgid(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT);
}

void accelerometer::handle_sns_std_sensor_event(
    const sns_client_event_msg_sns_client_event& pb_event)
{
    sns_std_sensor_event pb_sensor_event;
    pb_sensor_event.ParseFromString(pb_event.payload());

    if(pb_sensor_event.data_size()) {
        sensors_event_t hal_event = create_sensor_hal_event(pb_event.timestamp());

        if (_cal_type == SENSOR_CALIBRATED) {
            hal_event.acceleration.x = pb_sensor_event.data(0);
            hal_event.acceleration.y = pb_sensor_event.data(1);
            hal_event.acceleration.z = pb_sensor_event.data(2);
            hal_event.acceleration.status =
                sensors_hal_sample_status(pb_sensor_event.status());
            sns_logv("accel_sample: ts=%lld ns; value = [%f, %f, %f]",
                     (long long) hal_event.timestamp, hal_event.acceleration.x,
                     hal_event.acceleration.y, hal_event.acceleration.z);
        }

        if (_cal_type == SENSOR_UNCALIBRATED) {
            hal_event.uncalibrated_accelerometer.x_uncalib = pb_sensor_event.data(0);
            hal_event.uncalibrated_accelerometer.y_uncalib = pb_sensor_event.data(1);
            hal_event.uncalibrated_accelerometer.z_uncalib = pb_sensor_event.data(2);
            hal_event.uncalibrated_accelerometer.x_bias = 0;
            hal_event.uncalibrated_accelerometer.y_bias = 0;
            hal_event.uncalibrated_accelerometer.z_bias = 0;

            sns_logd("accel_sample_uncal: ts=%lld ns; value = [%f, %f, %f],"
                     " bias = [%f, %f, %f]", (long long) hal_event.timestamp,
                     hal_event.uncalibrated_accelerometer.x_uncalib,
                     hal_event.uncalibrated_accelerometer.y_uncalib,
                     hal_event.uncalibrated_accelerometer.z_uncalib,
                     hal_event.uncalibrated_accelerometer.x_bias,
                     hal_event.uncalibrated_accelerometer.y_bias,
                     hal_event.uncalibrated_accelerometer.z_bias);
        }

        submit_sensors_hal_event(hal_event);
    } else {
        sns_loge("empty data returned for accel");
    }
}

bool accelerometer::is_calibrated()
{
  return (_cal_type == SENSOR_CALIBRATED) ? true : false;
}

/* create sensor variants supported by this class and register the function
   to sensor_factory */
static vector<unique_ptr<sensor>> get_available_accel_calibrated()
{
    const vector<sensor_uid>& accel_suids =
         sensor_factory::instance().get_suids(SSC_DATATYPE_ACCEL);
    vector<unique_ptr<sensor>> sensors;
    for (const auto& suid : accel_suids) {
        if (!(sensor_factory::instance().get_settings()
                                    & DISABLE_WAKEUP_SENSORS_FLAG)) {
            try {
                sensors.push_back(make_unique<accelerometer>(suid, SENSOR_WAKEUP,
                                                         SENSOR_CALIBRATED));
            } catch (const exception& e) {
                sns_loge("failed for wakeup, %s", e.what());
            }
        }
        try {
            sensors.push_back(make_unique<accelerometer>(suid, SENSOR_NO_WAKEUP,
                                                     SENSOR_CALIBRATED));
        } catch (const exception& e) {
            sns_loge("failed for nowakeup, %s", e.what());
        }
    }
    return sensors;
}

static vector<unique_ptr<sensor>> get_available_accel_uncalibrated()
{
    const vector<sensor_uid>& accel_suids =
        sensor_factory::instance().get_suids(SSC_DATATYPE_ACCEL);
    vector<unique_ptr<sensor>> sensors;
    for (const auto& suid : accel_suids) {
        if (!(sensor_factory::instance().get_settings()
                                    & DISABLE_WAKEUP_SENSORS_FLAG)) {
            try {
                sensors.push_back(make_unique<accelerometer>(suid, SENSOR_WAKEUP,
                                                         SENSOR_UNCALIBRATED));
            } catch (const exception& e) {
                sns_loge("failed for wakeup,  %s", e.what());
            }
        }
        try {
            sensors.push_back(make_unique<accelerometer>(suid, SENSOR_NO_WAKEUP,
                                                     SENSOR_UNCALIBRATED));
        } catch (const exception& e) {
            sns_loge("failed for nowakeup, %s", e.what());
        }
    }
    return sensors;
}


static bool accelerometer_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(SENSOR_TYPE_ACCELEROMETER,
                                    get_available_accel_calibrated);
    sensor_factory::register_sensor(SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED,
                                    get_available_accel_uncalibrated);
    sensor_factory::request_datatype(SSC_DATATYPE_ACCEL);
    return true;
}

SENSOR_MODULE_INIT(accelerometer_module_init);
