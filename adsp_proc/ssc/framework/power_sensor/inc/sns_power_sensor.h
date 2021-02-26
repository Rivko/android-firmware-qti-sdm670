#pragma once
/*=============================================================================
  @file sns_power_sensor.h

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_data_stream.h"
#include "sns_fw_gpio_service.h"
#include "sns_sensor_uid.h"
#include "sns_suid_util.h"

/** Sensor power state structure */
typedef struct sns_power_sensor_state
{
  /* Registry SUID Lookup */
  SNS_SUID_LOOKUP_DATA(1) suid_lookup_data;
  /* Registry data stream */
  sns_data_stream         *reg_stream;
} sns_power_sensor_state;

