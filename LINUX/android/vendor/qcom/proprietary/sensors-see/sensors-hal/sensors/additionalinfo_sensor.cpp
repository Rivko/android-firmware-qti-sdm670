/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "additionalinfo_sensor.h"


additionalinfo_sensor::additionalinfo_sensor(sensor_uid suid, sensor_wakeup_type wakeup, additionalinfo_cb cbk):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_ADDITIONAL_INFO);
    set_string_type(SENSOR_STRING_TYPE_ADDITIONAL_INFO);
    set_resampling(true);
    _cb = cbk;
}

void additionalinfo_sensor::handle_sns_std_sensor_event(
    const sns_client_event_msg_sns_client_event& pb_event)
{
    sns_std_sensor_event pb_sensor_event;
    pb_sensor_event.ParseFromString(pb_event.payload());

    if(pb_sensor_event.data_size()) {
        sensors_event_t hal_event = create_sensor_hal_event(pb_event.timestamp());

        hal_event.type = SENSOR_TYPE_ADDITIONAL_INFO;
        sns_logv("datatype.c_str() %s", datatype().c_str());
        if (!strncmp("sensor_temperature", datatype().c_str(), sizeof("sensor_temperature"))) {
            hal_event.additional_info.type = AINFO_INTERNAL_TEMPERATURE;
            hal_event.additional_info.data_float[0] = pb_sensor_event.data(0);
            sns_logd("additionalinfo_sample: ts=%lld ns; infotype=%d; value = [%f]",
                     (long long) hal_event.timestamp,(int) hal_event.additional_info.type,
                     hal_event.additional_info.data_float[0]);
            _cb(hal_event);
        }
    } else {
        sns_loge("empty data returned for sensor temperature");
    }
}

