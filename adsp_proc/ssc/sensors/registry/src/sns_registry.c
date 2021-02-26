/*=============================================================================
  @file sns_registry.c

  The Sensors Registry is the module responsible for persistent data: data
  which persists across device reboots.  The Registry is simply responsible
  for the storage and appropriate distribution of its contained data, and
  has no understanding of what the data represents or is used for.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_register.h"
#include "sns_registry_sensor.h"
#include "sns_sensor.h"

/*=============================================================================
  External Variable Declarations
  ===========================================================================*/

extern sns_sensor_api sns_registry_sensor_api;

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_rc sns_register_registry_sensor(sns_register_cb const *register_api)
{
  // Note: All state for this Sensor is held within the associated Instances
  register_api->init_sensor(sizeof(sns_registry_sensor_state),
      &sns_registry_sensor_api, NULL);

  return SNS_RC_SUCCESS;
}
