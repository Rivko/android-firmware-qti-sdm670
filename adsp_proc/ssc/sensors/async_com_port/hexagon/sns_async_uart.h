#pragma once
/*=============================================================================
  @file sns_async_uart.h

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "sns_signal.h"

typedef struct
{
  /* Thread handle provided by signal sensor */
  sns_signal_thread *sig_thread;
  /* Flag provided by signal sensor */
  uint32_t signal;
} sns_async_uart_state;
