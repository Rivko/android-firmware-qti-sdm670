/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cinttypes>
#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_device_orient.pb.h"

static const char *SSC_DATATYPE_DEVICE_ORIENTATION = "device_orient";

class device_orient : public ssc_sensor
{
public:
    device_orient(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_DEVICE_ORIENTATION; }

private:
    virtual void handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event) override;
};

device_orient::device_orient(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_DEVICE_ORIENTATION);
    set_string_type(SENSOR_STRING_TYPE_DEVICE_ORIENTATION);
}

void device_orient::handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event)
{
    if (SNS_DEVICE_ORIENT_MSGID_SNS_DEVICE_ORIENT_EVENT == pb_event.msg_id()) {
        sns_device_orient_event pb_device_orient_event;
        pb_device_orient_event.ParseFromString(pb_event.payload());

        sensors_event_t hal_event =
             create_sensor_hal_event(pb_event.timestamp());

        hal_event.data[0] = pb_device_orient_event.state();
        submit_sensors_hal_event(hal_event);

        sns_logv("device_orient_event: ts=%" PRIu64 ", type=%i",
                 hal_event.timestamp,
                 pb_device_orient_event.state());
    }
}

static bool device_orient_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(SENSOR_TYPE_DEVICE_ORIENTATION,
        ssc_sensor::get_available_sensors<device_orient>);
    sensor_factory::request_datatype(SSC_DATATYPE_DEVICE_ORIENTATION);
    return true;
}

SENSOR_MODULE_INIT(device_orient_module_init);

