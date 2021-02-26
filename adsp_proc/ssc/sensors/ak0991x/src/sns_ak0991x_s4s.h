#pragma once
/**
 * @file sns_ak0991x_s4s.h
 *
 * S4S functions
 *
 * Copyright (c) 2016-2017 Asahi Kasei Microdevices
 * All Rights Reserved.
 * Confidential and Proprietary - Asahi Kasei Microdevices
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#include <stdint.h>

#include "sns_sensor_instance.h"
#include "sns_rc.h"

#include "sns_std_sensor.pb.h"

sns_rc ak0991x_s4s_set_mag_config(sns_sensor_instance *const this);

void ak0991x_s4s_send_config_event(sns_sensor_instance *const this,
                                   sns_std_sensor_physical_config_event *phy_sensor_config);

void ak0991x_s4s_inst_init(sns_sensor_instance *const this, sns_sensor_state const *sstate);

void ak0991x_s4s_inst_deinit(sns_sensor_instance *const this);

void ak0991x_s4s_register_timer(sns_sensor_instance *const this);

sns_rc ak0991x_s4s_handle_timer_event(sns_sensor_instance *const this);

void ak0991x_s4s_handle_timer_data_stream(sns_sensor_instance *const this);


