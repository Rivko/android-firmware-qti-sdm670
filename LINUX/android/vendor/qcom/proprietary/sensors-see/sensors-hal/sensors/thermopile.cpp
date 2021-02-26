/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sensors_qti.h"
#include "sns_physical_sensor_test.pb.h"

static const char *SSC_DATATYPE_THERMOPILE = "thermopile";

class thermopile : public ssc_sensor
{
public:
    thermopile(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_THERMOPILE; }
};

thermopile::thermopile(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(QTI_SENSOR_TYPE_THERMOPILE);
    set_string_type(QTI_SENSOR_STRING_TYPE_THERMOPILE);
    set_sensor_typename("Thermopile");
    set_nowk_msgid(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT);
    set_nowk_msgid(SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT);
}

static bool thermopile_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(
        QTI_SENSOR_TYPE_THERMOPILE,
        ssc_sensor::get_available_sensors<thermopile>);
    sensor_factory::request_datatype(SSC_DATATYPE_THERMOPILE);
    return true;
}

SENSOR_MODULE_INIT(thermopile_module_init);
