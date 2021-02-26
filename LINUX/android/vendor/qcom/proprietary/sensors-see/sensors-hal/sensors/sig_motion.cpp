/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cinttypes>
#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_sig_motion.pb.h"
#include "worker.h"

static const char *SSC_DATATYPE_SIGNIFICANT_MOTION = "sig_motion";

class sig_motion : public ssc_sensor
{
public:
    sig_motion(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_SIGNIFICANT_MOTION; }

private:
    virtual void activate() override;
    virtual void handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event) override;
    worker _worker;
    bool _oneshot_done = false;
    sensors_event_t _hal_event;
};

sig_motion::sig_motion(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_SIGNIFICANT_MOTION);
    set_string_type(SENSOR_STRING_TYPE_SIGNIFICANT_MOTION);
    set_reporting_mode(SENSOR_FLAG_ONE_SHOT_MODE);
    // One-shot sensors must report "-1" for the minDelay
    set_min_delay(-1);
    set_fifo_max_count(0);
    _worker.setname("sig_motion");
}

void sig_motion::activate()
{
    _oneshot_done = false;
    ssc_sensor::activate();
}

void sig_motion::handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event)
{
    if (!_oneshot_done && SNS_SIG_MOTION_MSGID_SNS_SIG_MOTION_EVENT == pb_event.msg_id()) {
        sensors_event_t hal_event =
             create_sensor_hal_event(pb_event.timestamp());

        hal_event.data[0] = 1.0f;
        submit_sensors_hal_event(hal_event);

        sns_logv("sig_motion_event: ts=%" PRIi64, _hal_event.timestamp);

        /* deactivate the sensor in worker thread */
        _worker.add_task(NULL,[this] { deactivate(); });
        _oneshot_done = true;
    }
}

static bool sig_motion_module_init()
{
    /* register supported wakeup sensor types with factory */
    // NOTE: Significant motion is only supported as a wakeup sensor. Android
    //       does not allow a non-wakeup variant of sig-mot.
    sensor_factory::register_sensor(SENSOR_TYPE_SIGNIFICANT_MOTION,
        ssc_sensor::get_available_wakeup_sensors<sig_motion>);
    sensor_factory::request_datatype(SSC_DATATYPE_SIGNIFICANT_MOTION);
    return true;
}

SENSOR_MODULE_INIT(sig_motion_module_init);
