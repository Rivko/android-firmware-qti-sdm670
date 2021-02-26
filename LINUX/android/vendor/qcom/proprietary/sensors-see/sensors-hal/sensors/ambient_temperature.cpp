/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_std_sensor.pb.h"

static const char *SSC_DATATYPE_AMBIENT_TEMP = "ambient_temperature";

class ambient_temperature : public ssc_sensor
{
public:
    ambient_temperature(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_AMBIENT_TEMP; }
};

ambient_temperature::ambient_temperature(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_AMBIENT_TEMPERATURE);
    set_string_type(SENSOR_STRING_TYPE_AMBIENT_TEMPERATURE);
    set_sensor_typename("Ambient Temperature Sensor");
    set_nowk_msgid(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT);
}

static bool ambient_temperature_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(
        SENSOR_TYPE_AMBIENT_TEMPERATURE,
        ssc_sensor::get_available_sensors<ambient_temperature>);
    sensor_factory::request_datatype(SSC_DATATYPE_AMBIENT_TEMP);
    return true;
}

SENSOR_MODULE_INIT(ambient_temperature_module_init);
