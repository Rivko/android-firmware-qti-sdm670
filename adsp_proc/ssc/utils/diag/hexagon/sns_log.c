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
#include "sns_log.h"


/*=============================================================================
  Public Function Definitions
  ===========================================================================*/
SNS_SECTION(".text.sns") bool sns_log_status(sns_log_code_type code)
{
  if(LOG_SNS_SENSOR_STATE_EXT == code ||
     LOG_SNS_SENSOR_STATE_INT == code ||
     LOG_SNS_HW_INTERRUPT == code)
  {
    // PEND: Disabled Sensor State Logs that are causing heap corruption
    return false;
  }
  else
  {
    return (log_status(code));
  }
}

SNS_SECTION(".text.sns") void sns_log_set_length (void *ptr, unsigned int length)
{
  log_set_length(ptr,length);
}

SNS_SECTION(".text.sns") void sns_log_set_code (void *ptr, sns_log_code_type code)
{
  log_set_code(ptr,code);
}

SNS_SECTION(".text.sns") void sns_log_set_timestamp (void *ptr)
{
  log_set_timestamp(ptr);
}

SNS_SECTION(".text.sns") bool sns_log_submit (void *ptr)
{
  return(log_submit(ptr));
}
