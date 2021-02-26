/*=============================================================================
  @file sns_suid_sensor_instance.c

  The Framework Sensor manages all SUID and attribute requests.  It is
  technically part of the Framework, but it modelled separately in order
  to avoid special cases within the event-passing code.

  This Sensor Instance code does nothing, and exists solely so that the
  Framework doesn't crash and/or require special handling code for this special
  Sensor type.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_isafe_list.h"
#include "sns_sensor_instance.h"
#include "sns_suid_sensor.h"
#include "sns_types.h"

#include "sns_memmgr.h"

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
  UNUSED_VAR(sensor_state);
  sns_suid_sensor_instance_config *state =
    (sns_suid_sensor_instance_config*)this->state->state;

  sns_isafe_list_init(&state->requests);

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
  sns_suid_sensor_instance_config *state =
    (sns_suid_sensor_instance_config*)this->state->state;
  sns_isafe_list_iter iter;

  sns_isafe_list_iter_init(&iter, &state->requests, true);
  while(NULL != sns_isafe_list_iter_curr(&iter))
  {
    sns_suid_lookup_request *request = (sns_suid_lookup_request*)
        sns_isafe_list_iter_get_curr_data(&iter);
    sns_isafe_list_iter_remove(&iter);
    sns_free(request);
  }

  return SNS_RC_SUCCESS;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_instance_api suid_sensor_instance_api =
{
  .struct_len = sizeof(sns_sensor_instance_api),
  .init = &init,
  .set_client_config = &set_client_config,
  .notify_event = &notify_event,
  .deinit = &deinit
};
