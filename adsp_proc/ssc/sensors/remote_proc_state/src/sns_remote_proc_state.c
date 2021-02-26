/*=============================================================================
  @file sns_remote_proc_state.c

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_register.h" 
#include "sns_remote_proc_state_sensor.h" 

/*=============================================================================
  External Variable Declarations
  ===========================================================================*/

extern sns_sensor_instance_api sns_remote_proc_state_sensor_instance_api;
extern sns_sensor_api remote_proc_state_sensor_api;

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_rc sns_remote_proc_state_register(sns_register_cb const *register_api)
{
  return register_api->init_sensor(sizeof(sns_remote_proc_state_state),
                                   &remote_proc_state_sensor_api,
                                   &sns_remote_proc_state_sensor_instance_api);
}
