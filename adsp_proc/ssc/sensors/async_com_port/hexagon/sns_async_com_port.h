#pragma once
/*=============================================================================
  @file sns_async_com_port.h

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "sns_async_com_port.h"
#include "sns_async_com_port_int.h"
#include "sns_data_stream.h"
#include "sns_isafe_list.h"
#include "sns_sensor_util.h"
#include "sns_signal.h"
#include "sns_stream_service.h"
#include "sns_time.h"

typedef struct
{
  /* Thread handle provided by signal sensor */
  sns_signal_thread *sig_thread;
  /* Flag provided by signal sensor */
  uint32_t signal;
} sns_async_com_port_state;

