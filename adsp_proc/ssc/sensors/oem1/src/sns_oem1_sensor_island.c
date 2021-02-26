/*=============================================================================
  @file sns_oem1_sensor.c

  The oem1 virtual Sensor implementation

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_attribute_service.h"
#include "sns_attribute_util.h"
#include "sns_event_service.h"
#include "sns_mem_util.h"
#include "sns_oem1.pb.h"
#include "sns_oem1_sensor.h"
#include "sns_oem1_sensor_instance.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_suid.pb.h"
#include "sns_types.h"

// API used by registry
#include "sns_registry.pb.h"

/*=============================================================================
  Function Definitions
  ===========================================================================*/

/* See sns_sensor::get_sensor_uid */
static sns_sensor_uid const*
sns_oem1_get_sensor_uid(sns_sensor const *this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid oem1_suid = OEM1_SUID;
  return &oem1_suid;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_api sns_oem1_api =
{
    .struct_len = sizeof(sns_sensor_api),
    .init = &sns_oem1_init,
    .deinit = &sns_oem1_deinit,
    .get_sensor_uid = &sns_oem1_get_sensor_uid,
    .set_client_request = &sns_oem1_set_client_request,
    .notify_event = &sns_oem1_notify_event,
};
