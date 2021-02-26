/**
 * @file sns_ak0991x.c
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * Copyright (c) 2016-2017 Asahi Kasei Microdevices
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * Confidential and Proprietary - Asahi Kasei Microdevices
 **/

/**
*****************************************************************************************
                               Includes
*****************************************************************************************
*/

#include "sns_rc.h"
#include "sns_register.h"
#include "sns_ak0991x_sensor.h"
#include "sns_ak0991x_sensor_instance.h"
#include "sns_ak0991x_hal.h"

/**
*****************************************************************************************
                            Public Function Definitions
*****************************************************************************************
*/

sns_rc sns_register_ak0991x(sns_register_cb const *register_api)
{
  /** Register Magnetic Sensor. */
  register_api->init_sensor(sizeof(ak0991x_state), &ak0991x_mag_sensor_api,
                            &ak0991x_sensor_instance_api);

  return SNS_RC_SUCCESS;
}
