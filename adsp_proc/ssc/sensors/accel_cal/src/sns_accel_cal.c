/*=============================================================================
  @file sns_accel_cal.c

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_accel_cal_sensor.h"
#include "sns_register.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"

/*=============================================================================
  External Variable Declarations
  ===========================================================================*/

extern sns_sensor_instance_api sns_accel_cal_sensor_instance_api;
extern sns_sensor_api sns_accel_cal_api;

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_rc sns_accel_cal_register(sns_register_cb const *register_api)
{
  register_api->init_sensor(sizeof(accel_cal_state), &sns_accel_cal_api,
      &sns_accel_cal_sensor_instance_api);

  return SNS_RC_SUCCESS;
}

