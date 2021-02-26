/**
 * sns_gpio_service_mdsp.c
 *
 * The GPIO Service implementation for SEE on MDSP
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *
 **/

#include <stdint.h>
#include "sns_fw_gpio_service.h"
#include "sns_gpio_service.h"
#include "sns_types.h"

/** Version: */
static const uint16_t sns_gpio_service_version = 1;

struct sns_fw_gpio_service
{
  sns_gpio_service service;
};

/**
 * Private state of the GPIO Service.
 */
static sns_fw_gpio_service gpio_service;

/** See sns_fw_gpio_service.h */
void sns_gpio_service_set_sns_gpio_config(bool sleep_config)
{
  UNUSED_VAR(sleep_config);
}

/** See sns_fw_gpio_service.h */
void sns_gpio_service_store_gpio_config(sns_gpio_config *gpio_config)
{
  UNUSED_VAR(gpio_config);
}

/** See sns_gpio_service.h */
sns_rc sns_read_gpio(uint32_t gpio,
                     bool is_chip_pin,
                     sns_gpio_state *state)
{
  UNUSED_VAR(gpio);
  UNUSED_VAR(is_chip_pin);
  UNUSED_VAR(state);
  return SNS_RC_SUCCESS;
}

/** See sns_gpio_service.h */
sns_rc sns_write_gpio(uint32_t gpio,
                      bool is_chip_pin,
                      sns_gpio_drive_strength drive_strength,
                      sns_gpio_pull_type pull,
                      sns_gpio_state state)
{
  UNUSED_VAR(gpio);
  UNUSED_VAR(is_chip_pin);
  UNUSED_VAR(drive_strength);
  UNUSED_VAR(pull);
  UNUSED_VAR(state);
  return SNS_RC_SUCCESS;
}

/**
 * GPIO Service API.
 */
static sns_gpio_service_api  gpio_service_api =
{
  .struct_len = sizeof(sns_gpio_service_api),
  .read_gpio = &sns_read_gpio,
  .write_gpio = &sns_write_gpio
};

/**
 * See sns_fw_gpio_service.h
 */
sns_fw_gpio_service* sns_gpio_service_init(void)
{
  gpio_service.service.api = &gpio_service_api;
  gpio_service.service.service.type = SNS_GPIO_SERVICE;
  gpio_service.service.service.version = sns_gpio_service_version;

  return (sns_fw_gpio_service*)&gpio_service;
}

