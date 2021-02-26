/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include "ssc_sensor.h"

using additionalinfo_cb = std::function<void(sensors_event_t hal_event)>;


class additionalinfo_sensor : public ssc_sensor {
public:
    additionalinfo_sensor(sensor_uid suid, sensor_wakeup_type wakeup, additionalinfo_cb cbk);
private:
    virtual void handle_sns_std_sensor_event(
        const sns_client_event_msg_sns_client_event& pb_event) override;
    additionalinfo_cb _cb;
};
