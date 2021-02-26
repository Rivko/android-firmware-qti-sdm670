/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cinttypes>
#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_offbody_detect.pb.h"

static const char *SSC_DATATYPE_OFFBODY_DETECT = "offbody_detect";

class offbody_detect : public ssc_sensor
{
public:
    offbody_detect(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_OFFBODY_DETECT; }

private:
    virtual void handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event) override;
};

offbody_detect::offbody_detect(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_LOW_LATENCY_OFFBODY_DETECT);
    set_string_type(SENSOR_STRING_TYPE_LOW_LATENCY_OFFBODY_DETECT);
}

void offbody_detect::handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event)
{
    if (SNS_OFFBODY_DETECT_MSGID_SNS_OFFBODY_DETECT_EVENT == pb_event.msg_id()) {
        sns_offbody_detect_event pb_offbody_detect_event;
        pb_offbody_detect_event.ParseFromString(pb_event.payload());

        sensors_event_t hal_event =
             create_sensor_hal_event(pb_event.timestamp());

        if (SNS_OFFBODY_DETECT_EVENT_TYPE_ON ==
              pb_offbody_detect_event.state()) {
            hal_event.data[0] = 1.0f;
        } else {
            hal_event.data[0] = 0.0f;
        }
        submit_sensors_hal_event(hal_event);

        sns_logv("offbody_detect_event: ts=%" PRIu64 ", type=%i",
                 hal_event.timestamp,
                 pb_offbody_detect_event.state());
    }
}

static bool offbody_detect_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(SENSOR_TYPE_LOW_LATENCY_OFFBODY_DETECT,
        ssc_sensor::get_available_sensors<offbody_detect>);
    sensor_factory::request_datatype(SSC_DATATYPE_OFFBODY_DETECT);
    return true;
}

SENSOR_MODULE_INIT(offbody_detect_module_init);
