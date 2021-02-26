/**
 * @file sns_lsm6ds3.c
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/
#include "sns_lsm6ds3_sensor.h"
#include "sns_lsm6ds3_sensor_instance.h"
#include "sns_rc.h"
#include "sns_register.h"

/** Public Function Definitions. */

sns_rc sns_register_lsm6ds3(sns_register_cb const *register_api)
{
  /** Register Accel Sensor. */
  register_api->init_sensor(sizeof(lsm6ds3_state), &lsm6ds3_accel_sensor_api,
      &lsm6ds3_sensor_instance_api);

  /** Register Gyro Sensor. */
  register_api->init_sensor(sizeof(lsm6ds3_state), &lsm6ds3_gyro_sensor_api,
      &lsm6ds3_sensor_instance_api);

  /** Register Motion Accel Sensor. */
  register_api->init_sensor(sizeof(lsm6ds3_state), &lsm6ds3_motion_detect_sensor_api,
      &lsm6ds3_sensor_instance_api);

  /** Register Sensor Temperature Sensor. */
  register_api->init_sensor(sizeof(lsm6ds3_state), &lsm6ds3_sensor_temp_sensor_api,
      &lsm6ds3_sensor_instance_api);

  return SNS_RC_SUCCESS;
}
