/**
 * @file sns_ak0991x_sensor_island.c
 *
 * Common implementation for AK0991X Sensors.
 *
 * Copyright (c) 2016-2017 Asahi Kasei Microdevices
 * All Rights Reserved.
 * Confidential and Proprietary - Asahi Kasei Microdevices
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 **/

#include <string.h>
#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_event_service.h"
#include "sns_stream_service.h"
#include "sns_service.h"
#include "sns_sensor_util.h"
#include "sns_math_util.h"
#include "sns_types.h"

#include "sns_ak0991x_sensor.h"
#include "sns_ak0991x_hal.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_suid.pb.h"
#include "sns_timer.pb.h"
#include "sns_diag_service.h"
#include "sns_sync_com_port_service.h"
#include "sns_attribute_util.h"

static sns_sensor_uid const* ak0991x_mag_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
#ifdef AK0991X_ENABLE_DUAL_SENSOR
  static const sns_sensor_uid sensor_uid_1 = MAG_SUID1;
  static const sns_sensor_uid sensor_uid_2 = MAG_SUID2;
  if(this->cb->get_registration_index(this) == 0)
  {
    return &sensor_uid_1;
  }
  else
  {
    return &sensor_uid_2;
  }
#else
  static const sns_sensor_uid sensor_uid = MAG_SUID1;
  return &sensor_uid;
#endif
}

sns_sensor_api ak0991x_mag_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &ak0991x_mag_init,
#ifdef AK0991X_ENABLE_DEINIT
  .deinit             = &ak0991x_mag_deinit,
#endif
  .get_sensor_uid     = &ak0991x_mag_get_sensor_uid,
  .set_client_request = &ak0991x_set_client_request,
  .notify_event       = &ak0991x_sensor_notify_event,
};
