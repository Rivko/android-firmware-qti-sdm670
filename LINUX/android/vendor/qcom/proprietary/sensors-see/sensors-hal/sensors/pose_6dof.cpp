/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_std_sensor.pb.h"

static const char *SSC_DATATYPE_POSE_6DOF = "pose_6dof";

class pose_6dof : public ssc_sensor
{
public:
    pose_6dof(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_POSE_6DOF; }
};

pose_6dof::pose_6dof(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_POSE_6DOF);
    set_string_type(SENSOR_STRING_TYPE_POSE_6DOF);
}

static bool pose_6dof_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(SENSOR_TYPE_POSE_6DOF,
        ssc_sensor::get_available_sensors<pose_6dof>);
    sensor_factory::request_datatype(SSC_DATATYPE_POSE_6DOF);
    return true;
}

SENSOR_MODULE_INIT(pose_6dof_module_init);
