#pragma once
/*=============================================================================
  @file sns_remote_proc_state_sensor_instance.h

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_remote_proc_state.pb.h"

typedef struct sns_remote_proc_state_inst_state
{
  /* Instance processor type */
  sns_std_client_processor inst_proc_type;
  
} sns_remote_proc_state_inst_state;

