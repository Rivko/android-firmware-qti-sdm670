/*=============================================================================
  @file sns_resampler_sensor_instance.c

  The Resampler virtual Sensor Instance implementation

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <math.h>
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_event_service.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_resampler_sensor.h"
#include "sns_resampler_sensor_instance.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_stream_service.h"
#include "sns_types.h"

#define NS_PER_SEC (1000000000)

/* See sns_sensor_instance_api::init */
sns_rc
sns_resampler_inst_init(sns_sensor_instance *this,
    sns_sensor_state const *state)
{
  UNUSED_VAR(state);
  resampler_instance_state *inst_state =
     (resampler_instance_state*)this->state->state;

  // PEND: Reset resampler state to default values
  // from resampler_sensor_state

  inst_state->ts_last_saved = 0;

  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::deinit */
sns_rc
sns_resampler_inst_deinit(sns_sensor_instance *const this)
{
  UNUSED_VAR(this);
  return SNS_RC_SUCCESS;
}

