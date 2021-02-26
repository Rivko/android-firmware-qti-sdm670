#pragma once
/**
 *  sns_gpio_service.h
 *
 *  Provides a synchronous API to read/write from/to output GPIO
 *  pins.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/** Include Files */

#include <stdbool.h>
#include <stdint.h>
#include "sns_rc.h"
#include "sns_service.h"

/** Type Definitions */

/** Types of GPIO pin drive strength.*/
typedef enum
{
  SNS_GPIO_DRIVE_STRENGTH_2_MILLI_AMP = 0,  // Specify a 2 mA drive.
  SNS_GPIO_DRIVE_STRENGTH_4_MILLI_AMP = 1,  // Specify a 4 mA drive.
  SNS_GPIO_DRIVE_STRENGTH_6_MILLI_AMP = 2,  // Specify a 6 mA drive.
  SNS_GPIO_DRIVE_STRENGTH_8_MILLI_AMP = 3,  // Specify an 8 mA drive.
  SNS_GPIO_DRIVE_STRENGTH_10_MILLI_AMP = 4, // Specify a 10 mA drive.
  SNS_GPIO_DRIVE_STRENGTH_12_MILLI_AMP = 5, // Specify a 12 mA drive.
  SNS_GPIO_DRIVE_STRENGTH_14_MILLI_AMP = 6, // Specify a 14 mA drive.
  SNS_GPIO_DRIVE_STRENGTH_16_MILLI_AMP = 7  // Specify a 16 mA drive.
} sns_gpio_drive_strength;

/** Types of GPIO pin pull.*/
typedef enum
{
  SNS_GPIO_PULL_TYPE_NO_PULL = 0,     // Do not specify a pull.
  SNS_GPIO_PULL_TYPE_PULL_DOWN = 1,   // Pull the GPIO down.
  SNS_GPIO_PULL_TYPE_KEEPER = 2,      // Designate as a Keeper.
  SNS_GPIO_PULL_TYPE_PULL_UP = 3      // Pull the pin up.
} sns_gpio_pull_type;

typedef enum
{
  SNS_GPIO_STATE_LOW,
  SNS_GPIO_STATE_HIGH
} sns_gpio_state;

/**
 * The GPIO service. Can be obtained from
 * sns_service_managere::getService.
 */
typedef struct sns_gpio_service
{
  /** Service information */
  sns_service service;

  /** Public API provided by the Service to be used by the Sensor. */
  struct sns_gpio_service_api *api;
} sns_gpio_service;

/**
 * API made available to Sensors.
 */
typedef struct sns_gpio_service_api
{
  uint32_t struct_len;

  /**
   * Read current state of a general purpose input pin.
   *
   * @param[i] gpio  gpio pin to read
   * @param[i] is_chip_pin  true if gpio is chip level TLMM pin
   *       else false
   * @param[o] state  output gpio state
   *
   * @return sns_rc
   * SNS_RC_SUCCESS if gpio read is successful
   * SNS_RC_NOT_SUPPORTED in case of unsupported GPIO input
   * SNS_RC_FAILED for other errors
   */
  sns_rc (*read_gpio)(uint32_t gpio,
                      bool is_chip_pin,
                      sns_gpio_state *state);

  /**
   * Write a value to a general purpose output pin.
   *
   * @param[i] gpio  gpio pin to write to
   * @param[i] is_chip_pin  true if gpio is chip level TLMM pin
   *       else false
   * @param[i]  drive_strength  drive strength config
   * @param[i] pull  pull type config
   * @param[i] state  output state to set for the gpio
   *
   * @return sns_rc
   * SNS_RC_SUCCESS if gpio write is successful
   * SNS_RC_NOT_SUPPORTED in case of unsupported GPIO input
   * SNS_RC_FAILED for other errors
   */
  sns_rc (*write_gpio)(uint32_t gpio,
                       bool is_chip_pin,
                       sns_gpio_drive_strength drive_strength,
                       sns_gpio_pull_type pull,
                       sns_gpio_state state);

} sns_gpio_service_api;

