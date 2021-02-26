/*=============================================================================
  @file sns_signal.c

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
#include "sns_signal_sensor.h"

/*=============================================================================
  External Variable Declarations
  ===========================================================================*/

extern sns_sensor_instance_api sns_signal_sensor_instance_api;
extern sns_sensor_api signal_sensor_api;

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_rc sns_signal_register(sns_register_cb const *register_api)
{
  register_api->init_sensor(sizeof(sns_signal_state),
                            &signal_sensor_api,
                            &sns_signal_sensor_instance_api);

  return SNS_RC_SUCCESS;
}
