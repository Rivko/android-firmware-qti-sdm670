/*=============================================================================
  @file sns_diag.c

  This sensor can be used to select a set of data_types for diag logging and
  F3 msgs print on QXDM. These data_types are enlisted in registry in predefined
  format

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_diag_sensor.h"
#include "sns_register.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"

/*=============================================================================
  External Variable Declarations
  ===========================================================================*/

extern sns_sensor_instance_api sns_diag_sensor_instance_api;
extern sns_sensor_api sns_diag_sensor_api;

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_rc sns_register_diag_sensor(sns_register_cb const *register_api)
{
  register_api->init_sensor(sizeof(sns_diag_sensor_state), &sns_diag_sensor_api,
      &sns_diag_sensor_instance_api);

  return SNS_RC_SUCCESS;
}
