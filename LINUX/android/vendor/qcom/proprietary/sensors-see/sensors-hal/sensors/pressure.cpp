/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_std_sensor.pb.h"
#include "sns_physical_sensor_test.pb.h"

static const char *SSC_DATATYPE_PRESSURE = "pressure";
static const uint32_t PRESSURE_RESERVED_FIFO_COUNT = 300;

class pressure : public ssc_sensor
{
public:
    pressure(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_PRESSURE; }
};

pressure::pressure(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_PRESSURE);
    set_string_type(SENSOR_STRING_TYPE_PRESSURE);
    set_fifo_reserved_count(PRESSURE_RESERVED_FIFO_COUNT);
    set_sensor_typename("Pressure Sensor");
    set_nowk_msgid(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT);
    set_nowk_msgid(SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT);
}

static bool pressure_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(
        SENSOR_TYPE_PRESSURE,
        ssc_sensor::get_available_sensors<pressure>);
    sensor_factory::request_datatype(SSC_DATATYPE_PRESSURE);
    return true;
}

SENSOR_MODULE_INIT(pressure_module_init);
