/**
 * @file sns_interrupt.c
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/sensors/interrupt/hexagon/sns_interrupt.c#1 $
 * $DateTime: 2018/02/01 03:15:18 $
 * $Change: 15367748 $
 *
 **/
#include "sns_interrupt_sensor.h"
#include "sns_interrupt_sensor_instance.h"
#include "sns_rc.h"
#include "sns_register.h"

/** Public Function Definitions. */

sns_rc sns_register_interrupt(sns_register_cb const *register_api)
{
  register_api->init_sensor(sizeof(interrupt_state), &interrupt_sensor_api,
      &interrupt_sensor_instance_api);

  return SNS_RC_SUCCESS;
}
