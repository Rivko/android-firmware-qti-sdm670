/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sensors_qti.h"
#include "sns_physical_sensor_test.pb.h"

static const char *SSC_DATATYPE_ULTRAVIOLET = "ultra_violet";

class ultraviolet : public ssc_sensor
{
public:
    ultraviolet(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_ULTRAVIOLET; }
};

ultraviolet::ultraviolet(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(QTI_SENSOR_TYPE_ULTRAVIOLET);
    set_string_type(QTI_SENSOR_STRING_TYPE_ULTRAVIOLET);
    set_sensor_typename("Ultraviolet Sensor");
    set_nowk_msgid(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT);
    set_nowk_msgid(SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT);
}

static bool ultraviolet_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(
        QTI_SENSOR_TYPE_ULTRAVIOLET,
        ssc_sensor::get_available_sensors<ultraviolet>);
    sensor_factory::request_datatype(SSC_DATATYPE_ULTRAVIOLET);
    return true;
}

SENSOR_MODULE_INIT(ultraviolet_module_init);
