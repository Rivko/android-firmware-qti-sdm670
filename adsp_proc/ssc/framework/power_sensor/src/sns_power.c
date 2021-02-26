/*=============================================================================
  @file sns_power.c

  The power sensor is responsible for reading the power related config items
  from the registry and setting the values in the respective power modules

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_power_sensor.h"
#include "sns_register.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"

/*=============================================================================
  External Variable Declarations
  ===========================================================================*/

extern sns_sensor_api sns_power_sensor_api;

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_rc sns_register_power_sensor(sns_register_cb const *register_api)
{
  register_api->init_sensor(sizeof(sns_power_sensor_state),
      &sns_power_sensor_api, NULL);

  return SNS_RC_SUCCESS;
}
