/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cinttypes>
#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_tilt.pb.h"

static const char *SSC_DATATYPE_WRIST_TILT_DETECTOR = "wrist_tilt_gesture";

class wrist_tilt : public ssc_sensor
{
public:
    wrist_tilt(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_WRIST_TILT_DETECTOR; }

private:
    virtual void handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event) override;
};

wrist_tilt::wrist_tilt(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_WRIST_TILT_GESTURE);
    set_string_type(SENSOR_STRING_TYPE_WRIST_TILT_GESTURE);
    set_reporting_mode(SENSOR_FLAG_SPECIAL_REPORTING_MODE);
}

void wrist_tilt::handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event)
{
    if (SNS_TILT_MSGID_SNS_TILT_EVENT == pb_event.msg_id()) {
        sensors_event_t hal_event =
             create_sensor_hal_event(pb_event.timestamp());

        hal_event.data[0] = 1.0f;
        submit_sensors_hal_event(hal_event);

        sns_logv("wrist_tilt_event: ts=%" PRIi64, hal_event.timestamp);
    }
}

static bool wrist_tilt_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(SENSOR_TYPE_WRIST_TILT_GESTURE,
        ssc_sensor::get_available_wakeup_sensors<wrist_tilt>);
    sensor_factory::request_datatype(SSC_DATATYPE_WRIST_TILT_DETECTOR);
    return true;
}

SENSOR_MODULE_INIT(wrist_tilt_module_init);

