/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_heart_beat.pb.h"

static const char *SSC_DATATYPE_HEART_BEAT = "heart_beat";

class heart_beat : public ssc_sensor
{
public:
    heart_beat(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_HEART_BEAT; }
};

heart_beat::heart_beat(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_HEART_BEAT);
    set_string_type(SENSOR_STRING_TYPE_HEART_BEAT);
}

static bool heart_beat_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(SENSOR_TYPE_HEART_BEAT,
        ssc_sensor::get_available_sensors<heart_beat>);
    sensor_factory::request_datatype(SSC_DATATYPE_HEART_BEAT);
    return true;
}

SENSOR_MODULE_INIT(heart_beat_module_init);
