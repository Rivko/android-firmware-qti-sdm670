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

class step_count : public ssc_sensor
{
public:
    step_count(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_PEDOMETER; }

private:
    virtual void handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event) override;
    virtual void deactivate() override;
    int first_step_count; // Step count received in first config_event
    int persist_step_count_current; //accumulative steps taken by the user since last activated
    int persist_step_count_update; //accumulative steps taking till now, update per each new step
};

step_count::step_count(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup), first_step_count(-1)
{
    set_type(SENSOR_TYPE_STEP_COUNTER);
    set_string_type(SENSOR_STRING_TYPE_STEP_COUNTER);
    set_fifo_reserved_count(PEDOMETER_RESERVED_FIFO_COUNT);
    set_nowk_msgid(SNS_PEDOMETER_MSGID_SNS_STEP_EVENT_CONFIG);
}

void step_count::deactivate()
{
    /* base class deactivate */
    ssc_sensor::deactivate();
    first_step_count = -1;
}

void step_count::handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event)
{
    sensors_event_t hal_event =
         create_sensor_hal_event(pb_event.timestamp());
    if (SNS_PEDOMETER_MSGID_SNS_STEP_EVENT == pb_event.msg_id()) {
        sns_step_event pb_step_event;
        pb_step_event.ParseFromString(pb_event.payload());

        if(-1 == this->first_step_count)
        {
            sns_loge("Received pedometer step event prior to config event");
        }
        else
        {
            hal_event.u64.step_counter = pb_step_event.step_count()
                                         - this->first_step_count
                                         + this->persist_step_count_current;
            this->persist_step_count_update = hal_event.u64.step_counter;
            submit_sensors_hal_event(hal_event);
        }
        sns_logv("step_detect_event: ts=%" PRIu64 " count %i",
                 hal_event.timestamp, pb_step_event.step_count());
    } else if (SNS_PEDOMETER_MSGID_SNS_STEP_EVENT_CONFIG == pb_event.msg_id()) {
        sns_step_event_config pb_step_event_config;
        pb_step_event_config.ParseFromString(pb_event.payload());

        // We may receive multiple config events - ignore all but the first
        if(-1 == this->first_step_count){
          this->first_step_count = pb_step_event_config.step_count();
          this->persist_step_count_current = this->persist_step_count_update;
          hal_event.u64.step_counter = this->persist_step_count_update;
          submit_sensors_hal_event(hal_event);
        }
        sns_logv("step_detect_event_config: ts=%llu count %i persist_count_current %i",
                 pb_event.timestamp(), pb_step_event_config.step_count(), this->persist_step_count_current);
    }
}

static bool step_count_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(SENSOR_TYPE_STEP_COUNTER,
        ssc_sensor::get_available_sensors<step_count>);
    sensor_factory::request_datatype(SSC_DATATYPE_PEDOMETER);
    return true;
}

SENSOR_MODULE_INIT(step_count_module_init);

