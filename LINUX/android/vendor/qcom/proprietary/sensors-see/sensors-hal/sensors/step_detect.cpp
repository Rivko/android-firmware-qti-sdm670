/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cinttypes>
#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_pedometer.pb.h"

#ifdef SNS_WEARABLES_TARGET
static const char *SSC_DATATYPE_PEDOMETER = "pedometer_wrist";
#else
static const char *SSC_DATATYPE_PEDOMETER = "pedometer";
#endif

static const uint32_t PEDOMETER_RESERVED_FIFO_COUNT = 300;

class step_detect : public ssc_sensor
{
public:
    step_detect(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_PEDOMETER; }

private:
    virtual void handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event) override;
};

step_detect::step_detect(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_STEP_DETECTOR);
    set_string_type(SENSOR_STRING_TYPE_STEP_DETECTOR);
    set_fifo_reserved_count(PEDOMETER_RESERVED_FIFO_COUNT);
    set_reporting_mode(SENSOR_FLAG_SPECIAL_REPORTING_MODE);
    set_nowk_msgid(SNS_PEDOMETER_MSGID_SNS_STEP_EVENT_CONFIG);
}

void step_detect::handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event)
{
    if (SNS_PEDOMETER_MSGID_SNS_STEP_EVENT == pb_event.msg_id()) {
        sns_step_event pb_step_event;
        pb_step_event.ParseFromString(pb_event.payload());

        sensors_event_t hal_event =
             create_sensor_hal_event(pb_event.timestamp());

        hal_event.data[0] = 1.0f;
        submit_sensors_hal_event(hal_event);

        sns_logv("step_detect_event: ts=%" PRIu64,
                 hal_event.timestamp);
    }
}

static bool step_detect_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(SENSOR_TYPE_STEP_DETECTOR,
        ssc_sensor::get_available_sensors<step_detect>);
    sensor_factory::request_datatype(SSC_DATATYPE_PEDOMETER);
    return true;
}

SENSOR_MODULE_INIT(step_detect_module_init);
