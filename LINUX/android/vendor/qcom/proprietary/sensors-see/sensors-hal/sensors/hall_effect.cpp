/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sensors_qti.h"
#include "sns_hall.pb.h"

static const char *SSC_DATATYPE_HALL_EFFECT = "hall";

class hall_effect : public ssc_sensor
{
public:
    hall_effect(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_HALL_EFFECT; }
private:
    virtual void handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event) override;
};

hall_effect::hall_effect(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(QTI_SENSOR_TYPE_HALL_EFFECT);
    set_string_type(QTI_SENSOR_STRING_TYPE_HALL_EFFECT);
    set_sensor_typename("Hall Effect Sensor");
    set_nowk_msgid(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT);
}

void hall_effect::handle_sns_client_event(
    const sns_client_event_msg_sns_client_event& pb_event)
{
    if (pb_event.msg_id() == SNS_HALL_MSGID_SNS_HALL_EVENT) {
        sns_hall_event pb_hall_event;
        pb_hall_event.ParseFromString(pb_event.payload());

        sensors_event_t hal_event =
             create_sensor_hal_event(pb_event.timestamp());

        if (pb_hall_event.hall_event_type() == SNS_HALL_EVENT_TYPE_NEAR) {
            hal_event.distance = 0.0f;
        } else {
            hal_event.distance = 1.0f;
        }
        submit_sensors_hal_event(hal_event);

        sns_logv("hall_event: ts=%llu, near_far=%d, distance=%f",
                 (unsigned long long)hal_event.timestamp,
                 pb_hall_event.hall_event_type(),
                 hal_event.distance);
    }
}


static bool hall_effect_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(
        QTI_SENSOR_TYPE_HALL_EFFECT,
        ssc_sensor::get_available_sensors<hall_effect>);
    sensor_factory::request_datatype(SSC_DATATYPE_HALL_EFFECT);
    return true;
}

SENSOR_MODULE_INIT(hall_effect_module_init);
