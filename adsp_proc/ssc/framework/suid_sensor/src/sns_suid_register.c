/*=============================================================================
  @file sns_suid_register.c

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
#include "sns_suid_sensor.h"

/*=============================================================================
  External Variable Declarations
  ===========================================================================*/

extern sns_sensor_instance_api suid_sensor_instance_api;
extern sns_sensor_api suid_sensor_api;

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_rc sns_register_suid_sensor(sns_register_cb const *register_api)
{
  // Note: All state for this Sensor is held within the associated Instances
  register_api->init_sensor(sizeof(sns_suid_sensor_state), &suid_sensor_api,
      &suid_sensor_instance_api);

  return SNS_RC_SUCCESS;
}
