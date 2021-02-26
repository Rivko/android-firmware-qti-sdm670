/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_std_sensor.pb.h"

static const char *SSC_DATATYPE_ROTV = "rotv";

class rotv : public ssc_sensor
{
public:
    rotv(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_ROTV; }
};

rotv::rotv(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_ROTATION_VECTOR);
    set_string_type(SENSOR_STRING_TYPE_ROTATION_VECTOR);
}

static bool rotv_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(
        SENSOR_TYPE_ROTATION_VECTOR,
        ssc_sensor::get_available_sensors<rotv>);
    sensor_factory::request_datatype(SSC_DATATYPE_ROTV);
    return true;
}

SENSOR_MODULE_INIT(rotv_module_init);
