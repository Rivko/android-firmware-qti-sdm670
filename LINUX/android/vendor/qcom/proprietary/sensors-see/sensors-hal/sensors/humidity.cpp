/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_std_sensor.pb.h"
#include "sns_physical_sensor_test.pb.h"

static const char *SSC_DATATYPE_HUMIDITY = "humidity";

class humidity : public ssc_sensor
{
public:
    humidity(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_HUMIDITY; }
};

humidity::humidity(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_RELATIVE_HUMIDITY);
    set_string_type(SENSOR_STRING_TYPE_RELATIVE_HUMIDITY);
    set_sensor_typename("Humidity Sensor");
    set_nowk_msgid(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT);
    set_nowk_msgid(SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT);
}

static bool humidity_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(
        SENSOR_TYPE_RELATIVE_HUMIDITY,
        ssc_sensor::get_available_sensors<humidity>);
    sensor_factory::request_datatype(SSC_DATATYPE_HUMIDITY);
    return true;
}

SENSOR_MODULE_INIT(humidity_module_init);
