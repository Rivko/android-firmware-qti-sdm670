/**
 * @file sns_test_sensor_instance_island.c
 *
 * The Interrupt virtual Sensor Instance implementation
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/sensors/test/src/sns_test_sensor_instance_island.c#2 $
 * $DateTime: 2018/03/27 11:46:46 $
 * $Change: 15784639 $
 *
 **/

#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_instance.h"
#include "sns_test_sensor.h"
#include "sns_test_sensor_instance.h"
#include "sns_types.h"

/** See sns_sensor_instance_api::notify_event */
sns_rc sns_test_inst_notify_event(sns_sensor_instance *const this)
{
  UNUSED_VAR(this);
  return SNS_RC_SUCCESS;
}

/** See sns_sensor_instance_api::set_client_config */
sns_rc sns_test_inst_set_client_config(sns_sensor_instance *const this,
                                             sns_request const *client_request)
{
  UNUSED_VAR(this);
  UNUSED_VAR(client_request);
  return SNS_RC_SUCCESS;
}

/** Public Data Definitions. */

sns_sensor_instance_api sns_test_sensor_instance_api =
{
  .struct_len        = sizeof(sns_sensor_instance_api),
  .init              = &sns_test_inst_init,
  .deinit            = &sns_test_inst_deinit,
  .set_client_config = &sns_test_inst_set_client_config,
  .notify_event      = &sns_test_inst_notify_event
};

