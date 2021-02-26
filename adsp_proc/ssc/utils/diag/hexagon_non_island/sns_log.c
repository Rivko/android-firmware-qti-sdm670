/*=============================================================================
  @file sns_log.h

  Diag Log functions

  Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "log_codes.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_log.h"

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/
SNS_SECTION(".text.sns") bool sns_log_status(sns_log_code_type code)
{
  bool status = false;
  // Call log_status, If device is not in island enabled
  // PEND: Disabled Sensor State Logs that are causing heap corruption
  if(SNS_ISLAND_STATE_IN_ISLAND != sns_island_get_island_state() &&
     LOG_SNS_SENSOR_STATE_EXT != code &&
     LOG_SNS_SENSOR_STATE_INT != code &&
     LOG_SNS_HW_INTERRUPT != code)
  {
    status = log_status(code);
  }
  return status;
}

SNS_SECTION(".text.sns") void sns_log_set_length (void *ptr, unsigned int length)
{    
  // Call log_set_length, If device is not in island enabled
  if(SNS_ISLAND_STATE_IN_ISLAND != sns_island_get_island_state())
  {
    log_set_length(ptr,length);
  }
}

SNS_SECTION(".text.sns") void sns_log_set_code (void *ptr, sns_log_code_type code)
{
  // Call log_set_code, If device is not in island enabled  
  if(SNS_ISLAND_STATE_IN_ISLAND != sns_island_get_island_state())
  {
    log_set_code(ptr,code);
  }
}

SNS_SECTION(".text.sns") void sns_log_set_timestamp (void *ptr)
{
  // Call log_set_timestamp, If device is not in island enabled    
  if(SNS_ISLAND_STATE_IN_ISLAND != sns_island_get_island_state())
  {
    log_set_timestamp(ptr);
  }
}

SNS_SECTION(".text.sns") bool sns_log_submit (void *ptr)
{
  bool status = false;
  // Call log_submit, If device is not in island enabled
  if(SNS_ISLAND_STATE_IN_ISLAND != sns_island_get_island_state())
  {
    status = log_submit(ptr);
  }
  return status;
}
