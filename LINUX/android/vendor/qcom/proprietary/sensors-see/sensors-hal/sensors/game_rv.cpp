/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_std_sensor.pb.h"

static const char *SSC_DATATYPE_GAME_RV = "game_rv";

static const uint32_t GAME_RV_RESERVED_FIFO_COUNT = 300;

class game_rv : public ssc_sensor
{
public:
    game_rv(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_GAME_RV; }
};

game_rv::game_rv(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_GAME_ROTATION_VECTOR);
    set_string_type(SENSOR_STRING_TYPE_GAME_ROTATION_VECTOR);
    set_fifo_reserved_count(GAME_RV_RESERVED_FIFO_COUNT);
}

static bool game_rv_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(SENSOR_TYPE_GAME_ROTATION_VECTOR,
        ssc_sensor::get_available_sensors<game_rv>);
    sensor_factory::request_datatype(SSC_DATATYPE_GAME_RV);
    return true;
}

SENSOR_MODULE_INIT(game_rv_module_init);
