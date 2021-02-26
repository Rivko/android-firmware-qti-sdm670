/*=============================================================================
  @file sns_async_uart_instance.c

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
/*=============================================================================
  Includes
  ===========================================================================*/
#include "sns_async_uart.h"
#include "sns_async_uart_instance.h"
#include "sns_printf.h"
#include "sns_rc.h"
#include "sns_sensor.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_types.h"


/*=============================================================================
  Macros and Constants
  ===========================================================================*/


/*=============================================================================
  Extern data
  ===========================================================================*/


/*=============================================================================
  Type Definitions
  ===========================================================================*/


/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* See sns_sensor_instance_api::event_notify */
static sns_rc
sns_async_uart_inst_notify_event(sns_sensor_instance* const this)
{
  UNUSED_VAR(this);
  // N/A
  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::init */
static sns_rc
sns_async_uart_inst_init(sns_sensor_instance* const this,
    sns_sensor_state const *sensor_state)
{
  UNUSED_VAR(sensor_state);
  SNS_INST_PRINTF(ERROR, this, "UART unavailable");
  return SNS_RC_NOT_AVAILABLE;
}

static sns_rc
sns_async_uart_inst_deinit(sns_sensor_instance* const this)
{
  SNS_INST_PRINTF(ERROR, this, "UART unavailable");
  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::set_client_config */
static sns_rc
sns_async_uart_inst_set_client_config(sns_sensor_instance* const this,
                  sns_request const* client_request)
{
  UNUSED_VAR(client_request);
  SNS_INST_PRINTF(ERROR, this, "UART unavailable");
  return SNS_RC_INVALID_VALUE;
}
/*===========================================================================
  Public Function Definition
  ===========================================================================*/

sns_rc
sns_async_uart_inst_generate_event(sns_sensor_instance *const this)
{
  SNS_INST_PRINTF(ERROR, this, "UART unavailable");
  return SNS_RC_SUCCESS;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_instance_api sns_async_uart_instance_api =
{
  .struct_len = sizeof(sns_sensor_instance_api),
  .init = &sns_async_uart_inst_init,
  .deinit = &sns_async_uart_inst_deinit,
  .set_client_config = &sns_async_uart_inst_set_client_config,
  .notify_event = &sns_async_uart_inst_notify_event
};
