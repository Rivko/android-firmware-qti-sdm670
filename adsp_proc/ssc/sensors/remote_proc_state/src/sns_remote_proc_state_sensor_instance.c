/*=============================================================================
  @file sns_remote_proc_state_sensor_instance.c

  Copyright (c) 2017-2018  Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
/*=============================================================================
  Includes
  ===========================================================================*/
#include "sns_rc.h"
#include "sns_remote_proc_state_sensor.h"
#include "sns_remote_proc_state_sensor_instance.h"
#include "sns_std.pb.h"
#include "sns_types.h"

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* See sns_sensor_instance_api::notify_event */
static sns_rc
sns_remote_proc_state_inst_notify_event(sns_sensor_instance* const this)
{
  UNUSED_VAR(this);
  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::init */
static sns_rc
sns_remote_proc_state_inst_init(sns_sensor_instance* const this,
    sns_sensor_state const *sensor_state)
{
  UNUSED_VAR(this);
  UNUSED_VAR(sensor_state);
  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::deinit */
static sns_rc
sns_remote_proc_state_inst_deinit(sns_sensor_instance* const this)
{
  UNUSED_VAR(this);
  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::set_client_config */
static sns_rc
sns_remote_proc_state_inst_set_client_config(sns_sensor_instance* const this,
                                 sns_request const* client_request)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_remote_proc_state_inst_state *state =
     (sns_remote_proc_state_inst_state *)this->state->state;
  sns_remote_proc_state_config *config = client_request->request;

  if (SNS_REMOTE_PROC_STATE_MSGID_SNS_REMOTE_PROC_STATE_CONFIG == client_request->message_id)
  {
    if(config->proc_type == SNS_STD_CLIENT_PROCESSOR_APSS)
    {
      state->inst_proc_type = config->proc_type;
    }
    else
    {
      rc = SNS_RC_INVALID_VALUE;
    }
  }
  else if(SNS_STD_MSGID_SNS_STD_FLUSH_REQ == client_request->message_id)
  {
    sns_sensor_util_send_flush_event(sns_remote_proc_state_get_sensor_uid(NULL), this);
  }
  else
  {
    rc = SNS_RC_NOT_SUPPORTED;
  }
  
  return rc;
}


/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_instance_api sns_remote_proc_state_sensor_instance_api =
{
  .struct_len = sizeof(sns_sensor_instance_api),
  .init = &sns_remote_proc_state_inst_init,
  .deinit = &sns_remote_proc_state_inst_deinit,
  .set_client_config = &sns_remote_proc_state_inst_set_client_config,
  .notify_event = &sns_remote_proc_state_inst_notify_event
};
