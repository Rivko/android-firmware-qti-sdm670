/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cinttypes>
#include <string>
#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_psmd.pb.h"
#include "worker.h"

static const char *SSC_DATATYPE_MOTION_DETECT = "psmd";


class motion_detect : public ssc_sensor
{
public:
    motion_detect(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_MOTION_DETECT; }
    sns_client_request_msg create_sensor_config_request();

private:
    worker _worker;
    bool _oneshot_done = false;
    virtual void handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event) override;
};

motion_detect::motion_detect(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_MOTION_DETECT);
    set_string_type(SENSOR_STRING_TYPE_MOTION_DETECT);
    if (wakeup)
        set_name("motion_detect_wakeup");
    else
        set_name("motion_detect");
    set_reporting_mode(SENSOR_FLAG_ONE_SHOT_MODE);
    // One-shot sensors must report "-1" for the minDelay
    set_min_delay(-1);
    set_fifo_max_count(0);
    _worker.setname("motion_detect");
}

sns_client_request_msg motion_detect::create_sensor_config_request()
{
    sns_client_request_msg pb_req_msg;
    sns_psmd_config pb_psmd_cfg;
    std::string pb_psmd_cfg_encoded;

    sns_logd("sending SNS_PSMD_MSGID_SNS_PSMD_CONFIG");
    pb_psmd_cfg.set_type(SNS_PSMD_TYPE_MOTION);

    pb_psmd_cfg.SerializeToString(&pb_psmd_cfg_encoded);
    pb_req_msg.set_msg_id(SNS_PSMD_MSGID_SNS_PSMD_CONFIG);
    pb_req_msg.mutable_request()->set_payload(pb_psmd_cfg_encoded);
    _oneshot_done = false;
    return pb_req_msg;
}

void motion_detect::handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event)
{
    if (!_oneshot_done && SNS_PSMD_MSGID_SNS_PSMD_EVENT == pb_event.msg_id()) {
        sensors_event_t hal_event =
            create_sensor_hal_event(pb_event.timestamp());

        hal_event.data[0] = 1.0f;
        submit_sensors_hal_event(hal_event);

        sns_logv("motion_detect_event: ts=%" PRIi64,
                hal_event.timestamp);

        /* deactivate the sensor in worker thread */
        _worker.add_task(NULL, [this] { deactivate(); });
        _oneshot_done = true;
    }
}

static bool motion_detect_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(SENSOR_TYPE_MOTION_DETECT,
        ssc_sensor::get_available_sensors<motion_detect>);
    sensor_factory::request_datatype(SSC_DATATYPE_MOTION_DETECT);
    return true;
}

SENSOR_MODULE_INIT(motion_detect_module_init);
