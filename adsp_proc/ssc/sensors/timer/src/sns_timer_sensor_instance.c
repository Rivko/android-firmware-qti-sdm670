/*=============================================================================
  @file sns_timer_sensor_instance.c

  Copyright (c) 2016-2017  Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
/*=============================================================================
  Includes
  ===========================================================================*/
#include "sns_island.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_rc.h"
#include "sns_sensor_event.h"
#include "sns_timer_sensor.h"
#include "sns_timer_sensor_instance.h"
#include "sns_types.h"

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* See sns_sensor_instance_api::notify_event */
SNS_SECTION(".text.sns") static sns_rc
sns_timer_inst_notify_event(sns_sensor_instance* const this)
{
  UNUSED_VAR(this);
  // N/A
  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::init */
SNS_SECTION(".text.sns") static sns_rc
sns_timer_inst_init(sns_sensor_instance* const this,
    sns_sensor_state const *sensor_state)
{
  UNUSED_VAR(this);
  UNUSED_VAR(sensor_state);
  // N/A
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") static sns_rc
sns_timer_inst_deinit(sns_sensor_instance* const this)
{
  UNUSED_VAR(this);
  // N/A
  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::set_client_config */
SNS_SECTION(".text.sns") static sns_rc
sns_timer_inst_set_client_config(sns_sensor_instance* const this,
                                 sns_request const* client_request)
{
  UNUSED_VAR(this);
  UNUSED_VAR(client_request);
  return 0;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_instance_api sns_timer_sensor_instance_api SNS_SECTION(".data.sns")  =
{
  .struct_len = sizeof(sns_sensor_instance_api),
  .init = &sns_timer_inst_init,
  .deinit = &sns_timer_inst_deinit,
  .set_client_config = &sns_timer_inst_set_client_config,
  .notify_event = &sns_timer_inst_notify_event
};
