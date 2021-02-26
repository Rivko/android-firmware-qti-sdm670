#pragma once
/**
 * @file sns_interrupt_sensor_configure.h
 *
 * Target-specific interrupt sensor configurations.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/sensors/interrupt/hexagon/sns_interrupt_sensor_configure.h#2 $
 * $DateTime: 2018/03/27 11:46:46 $
 * $Change: 15784639 $
 *
 **/

#include "sns_request.h"
#include "sns_sensor_instance.h"
#include "sns_time.h"
#include "stdint.h"

/**
 * Interrupt timestampt status. This is a mirror of uGPIOIntTimestampStatus in uGPIOInt.h
 */
typedef enum {
  SNS_INTERRUPT_TIMESTAMP_INVALID          = 0x0,  /**< Timestamp is not valid */
  SNS_INTERRUPT_TIMESTAMP_VALID            = 0x1,  /**< Timestamp is valid */
  SNS_INTERRUPT_TIMESTAMP_INVALID_OVERFLOW = 0x10, /**< Timestamp is invalid, and there has been an overflow */
  SNS_INTERRUPT_TIMESTAMP_VALID_OVERFLOW   = 0x11  /**< Timestamp is valid, and there has been an overflow */
} sns_interrupt_timestamp_status;

/**
 * Get flag to register sensors interrupts for island mode operation
 */
uint32_t sns_interrupt_sensor_get_island_flag(void);

/**
 * Get flag to register level triggered interrupts
 */
uint32_t sns_interrupt_sensor_get_level_trigger_flag(void);

/**
 * Get flag to use the hardware timestamping unit
 */
uint32_t sns_interrupt_sensor_get_hw_timestamp_flag(void);

sns_rc send_interrupt_done(sns_sensor_instance *const this, sns_request const *client_request);

/**
 * Attempt to read the timestamp that was captured by the interrupt's HW
 * timestamping unit.
 *
 * @param[i] this        pointer to the sensor instance, for logging purposes
 * @param[i] gpio        interrupt pin to read the timestamp from
 * @param[o] timestamp   where the timestamp is placed
 * @param[o] ts_status   timestamp status
 *
 * @return sns_rc        SNS_RC_SUCCESS if the timestamp was successfully read,
 *                       SNS_RC_FAILED otherwise
 */
sns_rc read_timestamp(sns_sensor_instance *const this, uint32_t gpio,
    sns_time* timestamp, sns_interrupt_timestamp_status* ts_status);
