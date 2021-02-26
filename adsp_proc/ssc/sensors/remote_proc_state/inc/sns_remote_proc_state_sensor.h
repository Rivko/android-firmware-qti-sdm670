#pragma once
/*=============================================================================
  @file sns_remote_proc_state_sensor.h

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_remote_proc_state.pb.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_signal.h"

/* Bit mask from the SMP2P service indicating AP is awake */
#define SMP2P_AWAKE    0x00001000

#define REMOTE_PROC_STATE_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0xc7 ,0xf9 ,0x54 ,0xe3 ,0xb1 ,0x93 ,0x43 ,0xb1 ,  \
        0x92 ,0x50 ,0x76 ,0x08 ,0x6a ,0x07 ,0xc9 ,0x3d  \
      }  \
  }

typedef struct
{
  /* REMOTE_PROC_STATE signal thread */
  sns_signal_thread *sig_thread;
  /* REMOTE_PROC_STATE signal */
  uint32_t signal;
}sns_remote_proc_state_read_only_buffer;

typedef struct
{
  /* REMOTE_PROC_STATE signal thread buffer */
  sns_remote_proc_state_read_only_buffer ro_buffer;
  /* APSS Processor State */
  sns_remote_proc_state_event_type ap_proc_state;
} sns_remote_proc_state_state;

/* See sns_sensor_api::get_sensor_uid */
sns_sensor_uid const*
sns_remote_proc_state_get_sensor_uid(sns_sensor const *this);
