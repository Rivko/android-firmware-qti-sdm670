#pragma once
/*=============================================================================
  @file sns_timer_sensor.h

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "sns_data_stream.h"
#include "sns_event_service.h"
#include "sns_isafe_list.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_signal.h"
#include "sns_stream_service.h"
#include "sns_time.h"
#include "sns_timer.h"

#include "sns_timer.pb.h"

#define SNS_TIMER_PINGPONG_BUFF_SIZE 2

#define TIMER_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0x13 ,0xa8 ,0x3b ,0x7e ,0xee ,0x63 ,0x65 ,0x31 ,  \
        0x8a ,0xb6 ,0x3b ,0x7c ,0x78 ,0xb9 ,0x08 ,0xd7  \
      }  \
  }

typedef struct
{
  uint32_t timeout_id_counter;
  sns_signal_thread *sig_thread;
  uint32_t signal;
  struct
  {
    uint32_t last_timeout_id;
    sns_time timestamp;
  }timeout_event;
}sns_timer_read_only_buffer;

typedef struct
{
  sns_isafe_list registered_timers;
  sns_isafe_list periodic_timers;
  sns_timer_read_only_buffer ro_buffer;
  uint32_t last_timeout_id_processed;
  sns_timer *system_timer;
} sns_timer_state;

