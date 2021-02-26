/*=============================================================================
  @file sns_timer.c

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_register.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_timer_sensor.h"

/*=============================================================================
  External Variable Declarations
  ===========================================================================*/

extern sns_sensor_instance_api sns_timer_sensor_instance_api;
extern sns_sensor_api timer_sensor_api;

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_rc sns_timer_register(sns_register_cb const *register_api)
{
  return register_api->init_sensor(sizeof(sns_timer_state),
                                   &timer_sensor_api,
                                   &sns_timer_sensor_instance_api);
}
