/*=============================================================================
  @file sns_signal_sensor_instance.c

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
/*=============================================================================
  Includes
  ===========================================================================*/
#include "sns_attribute_service.h"
#include "sns_event_service.h"
#include "sns_island.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_profiler.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_signal_sensor.h"
#include "sns_signal_sensor.pb.h"
#include "sns_signal_sensor_instance.h"
#include "sns_stream_service.h"
#include "sns_types.h"

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

SNS_SECTION(".text.sns") static sns_rc
sns_signal_inst_notify_event(sns_sensor_instance* const this)
{
  UNUSED_VAR(this);
  // N/A
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") static sns_rc
sns_signal_inst_init(sns_sensor_instance* const this,
    sns_sensor_state const *sensor_state)
{
  SNS_PROFILE(SNS_SIGNAL_SENSOR_INSTANCE_INIT_START, 0);
  UNUSED_VAR(this);
  UNUSED_VAR(sensor_state);
  // N/A
  SNS_PROFILE(SNS_SIGNAL_SENSOR_INSTANCE_INIT_END, 0);
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") static sns_rc
sns_signal_inst_deinit(sns_sensor_instance* const this)
{
  SNS_PROFILE(SNS_SIGNAL_SENSOR_INSTANCE_DEINIT_START, 0);
  UNUSED_VAR(this);
  // N/A
  return SNS_RC_SUCCESS;
  SNS_PROFILE(SNS_SIGNAL_SENSOR_INSTANCE_DEINIT_START, 0);
}

SNS_SECTION(".text.sns") static sns_rc
sns_signal_inst_set_client_config(sns_sensor_instance* const this,
                                  sns_request const* client_request)
{
  sns_signal_instance_state * state = (sns_signal_instance_state *)this->state->state;
  sns_memscpy(&state->config, sizeof(sns_signal_sensor_req), client_request->request, client_request->request_len);
  return SNS_RC_SUCCESS;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_instance_api sns_signal_sensor_instance_api SNS_SECTION(".data.sns") =
{
  .struct_len = sizeof(sns_sensor_instance_api),
  .init = &sns_signal_inst_init,
  .deinit = &sns_signal_inst_deinit,
  .set_client_config = &sns_signal_inst_set_client_config,
  .notify_event = &sns_signal_inst_notify_event
};
