/*=============================================================================
  @file sns_oem1.c

  OEM1 (template algorithm) implementation

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_oem1_sensor.h"
#include "sns_register.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"

/*=============================================================================
  External Variable Declarations
  ===========================================================================*/

extern sns_sensor_instance_api sns_oem1_sensor_instance_api;
extern sns_sensor_api sns_oem1_api;

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_rc sns_oem1_register(sns_register_cb const *register_api)
{
  register_api->init_sensor(sizeof(sns_oem1_sensor_state),
                            &sns_oem1_api,
                            &sns_oem1_sensor_instance_api);

  return SNS_RC_SUCCESS;
}

