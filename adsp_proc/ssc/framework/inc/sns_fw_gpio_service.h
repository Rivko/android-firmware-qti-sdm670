#pragma once
/*=============================================================================
  @file sns_fw_gpio_service.h

  Framework header for Sensors GPIO Service.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

 * $Id: //components/rel/ssc.slpi/3.2/framework/inc/sns_fw_gpio_service.h#2 $
 * $DateTime: 2018/03/27 11:46:46 $
 * $Change: 15784639 $
  ===========================================================================*/

#include <stdint.h>
#include <stdbool.h>

/*=============================================================================
  Macros and constants
  ===========================================================================*/

/* If there are more GPIOs in future targets, add their
   registry group names and adjust the MAX GPIO value */
#define SNS_MAX_GPIO 20

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/** GPIO config with GPIO number and active/sleep configs */
typedef struct sns_gpio_config
{
  /* GPIO number of the config being stored */
  uint16_t gpio_num;
  /* Active function select config */
  uint8_t active_fs;
  /* Active direction: IN/OUT */
  uint8_t active_dir;
  /* Active pull type: NO PULL/UP/DOWN/KEEPER */
  uint8_t active_pull;
  /* Active drive strength: 2mA/4mA/6mA/8mA/10mA/12mA/14mA/16mA */
  uint8_t active_drive;
  /* Sleep function select config */
  uint8_t sleep_fs;
  /* Sleep direction: IN/OUT */
  uint8_t sleep_dir;
  /* Sleep pull type: NO PULL/UP/DOWN/KEEPER */
  uint8_t sleep_pull;
  /* Sleep drive strength: 2mA/4mA/6mA/8mA/10mA/12mA/14mA/16mA */
  uint8_t sleep_drive;
} sns_gpio_config;

/** Forward Declaration. */
typedef struct sns_fw_gpio_service sns_fw_gpio_service;

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/**
 * Set active/sleep GPIO config for the SSC GPIOs
 *
 * @param[i] sleep_config true if sleep config is to be set
 *                        false if active config is to be set
 */
void sns_gpio_service_set_sns_gpio_config(bool sleep_config);

/**
 * Store the GPIO active and sleep config for the provided GPIO
 *
 * @param[i] gpio_config GPIO config with active/sleep set values
 */
void sns_gpio_service_store_gpio_config(sns_gpio_config *gpio_config);

/**
 * Initialize the GPIO service; to be used only by the Service
 * Manager.
 */
sns_fw_gpio_service* sns_gpio_service_init(void);
