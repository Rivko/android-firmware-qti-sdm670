#pragma once
/**
 * @file sns_interrupt_sensor.h
 *
 * The Interrupt virtual Sensor.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/sensors/interrupt/hexagon/sns_interrupt_sensor.h#1 $
 * $DateTime: 2018/02/01 03:15:18 $
 * $Change: 15367748 $
 *
 **/

#include "sns_sensor.h"
#include "sns_signal.h"

/** Forward Declaration of Sensor API */
sns_sensor_api interrupt_sensor_api;

/** Interrupt Sensor State. */
typedef struct interrupt_state
{
  sns_signal_thread *signal_thread;
} interrupt_state;
