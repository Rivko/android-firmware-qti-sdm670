/*=============================================================================
  @file sns_diag_sensor_instance.c

  This Sensor Instance code does nothing, and exists solely so that the
  Framework doesn't crash and/or require special handling code for this special
  Sensor type.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_diag_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_types.h"

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* See sns_sensor_instance_api::event_notify */
static sns_rc
notify_event(sns_sensor_instance *const this)
{
  UNUSED_VAR(this);
  // This Sensor Instance receives no events
  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::init */
static sns_rc
init(sns_sensor_instance *const this, sns_sensor_state const *sensor_state)
{
  UNUSED_VAR(this);
  UNUSED_VAR(sensor_state);

  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::set_client_config */
static sns_rc
set_client_config(sns_sensor_instance *const this,
    sns_request const *client_request)
{
  UNUSED_VAR(this);
  UNUSED_VAR(client_request);
  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::deinit */
static sns_rc
deinit(sns_sensor_instance *const this)
{
  UNUSED_VAR(this);


  return SNS_RC_SUCCESS;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_instance_api sns_diag_sensor_instance_api =
{
  .struct_len = sizeof(sns_sensor_instance_api),
  .init = &init,
  .set_client_config = &set_client_config,
  .notify_event = &notify_event,
  .deinit = &deinit
};
