/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sensors_qti.h"
#include "sns_physical_sensor_test.pb.h"

using namespace std;

static const char *SSC_DATATYPE_RGB = "rgb";

class rgb : public ssc_sensor
{
public:
    rgb(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_RGB; }
};

rgb::rgb(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(QTI_SENSOR_TYPE_RGB);
    set_string_type(QTI_SENSOR_STRING_TYPE_RGB);
    set_sensor_typename("RGB Sensor");
    set_nowk_msgid(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT);
    set_nowk_msgid(SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT);
}

static vector<unique_ptr<sensor>> get_available_rgb_sensors()
{
    const vector<sensor_uid>& rgb_suids =
         sensor_factory::instance().get_suids(SSC_DATATYPE_RGB);
    vector<unique_ptr<sensor>> sensors;
    for (const auto& suid : rgb_suids) {
        const sensor_attributes& attr =
            sensor_factory::instance().get_attributes(suid);
        /* publish only on-change sensor */
        if (attr.get_stream_type() != SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE) {
            continue;
        }
        try {
            sensors.push_back(make_unique<rgb>(suid, SENSOR_WAKEUP));
        } catch (const exception& e) {
            sns_loge("failed for wakeup, %s", e.what());
        }
        try {
            sensors.push_back(make_unique<rgb>(suid, SENSOR_NO_WAKEUP));
        } catch (const exception& e) {
            sns_loge("failed for nowakeup, %s", e.what());
        }
    }
    return sensors;
}

static bool rgb_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(QTI_SENSOR_TYPE_RGB,
                                    get_available_rgb_sensors);
    sensor_factory::request_datatype(SSC_DATATYPE_RGB);
    return true;
}

SENSOR_MODULE_INIT(rgb_module_init);
