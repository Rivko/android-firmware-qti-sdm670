/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cinttypes>
#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_tilt.pb.h"

static const char *SSC_DATATYPE_TILT_DETECTOR = "tilt";

class tilt : public ssc_sensor
{
public:
    tilt(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_TILT_DETECTOR; }

private:
    virtual void handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event) override;
};

tilt::tilt(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_TILT_DETECTOR);
    set_string_type(SENSOR_STRING_TYPE_TILT_DETECTOR);
    set_reporting_mode(SENSOR_FLAG_SPECIAL_REPORTING_MODE);
}

void tilt::handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event)
{
    if (SNS_TILT_MSGID_SNS_TILT_EVENT == pb_event.msg_id()) {
        sensors_event_t hal_event =
             create_sensor_hal_event(pb_event.timestamp());

        hal_event.data[0] = 1.0f;
        submit_sensors_hal_event(hal_event);

        sns_logv("tilt_event: ts=%" PRIi64, hal_event.timestamp);
    }
}

static bool tilt_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(SENSOR_TYPE_TILT_DETECTOR,
        ssc_sensor::get_available_wakeup_sensors<tilt>);
    sensor_factory::request_datatype(SSC_DATATYPE_TILT_DETECTOR);
    return true;
}

SENSOR_MODULE_INIT(tilt_module_init);

