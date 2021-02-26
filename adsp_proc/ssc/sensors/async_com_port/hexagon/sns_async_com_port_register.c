/*=============================================================================
  @file sns_async_com_port_register.c

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_async_com_port.h"
#include "sns_async_uart.h"
#include "sns_register.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"

/*=============================================================================
  External Variable Declarations
  ===========================================================================*/

extern sns_sensor_instance_api sns_async_com_port_instance_api;
extern sns_sensor_api async_com_port_api;

extern sns_sensor_instance_api sns_async_uart_instance_api;
extern sns_sensor_api async_uart_api;

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_rc sns_async_com_port_register(sns_register_cb const *register_api)
{
  register_api->init_sensor(sizeof(sns_async_com_port_state),
                            &async_com_port_api,
                            &sns_async_com_port_instance_api);
  // Initialize the async UART sensor as a part of this library
  register_api->init_sensor(sizeof(sns_async_uart_state),
                            &async_uart_api,
                            &sns_async_uart_instance_api);
  return SNS_RC_SUCCESS;
}
