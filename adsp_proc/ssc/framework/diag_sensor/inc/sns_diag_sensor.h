#pragma once
/*=============================================================================
  @file sns_diag_sensor.h

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_attribute_service.h"
#include "sns_data_stream.h"
#include "sns_diag_service.h"
#include "sns_sensor_uid.h"
#include "sns_suid_util.h"

typedef struct sns_diag_sensor_state
{
  sns_diag_service        *diag_service;
  sns_data_stream         *reg_stream;
  /* Registry SUID Lookup */
  SNS_SUID_LOOKUP_DATA(1) suid_lookup_data;

  // set to true the first time "sns_diag_sensor_datatype" registry 
  // group has been read
  bool sensor_datatype_reg_read; 
} sns_diag_sensor_state;
