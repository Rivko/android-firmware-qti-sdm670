#ifndef QSEE_TLMM_H
#define QSEE_TLMM_H



/**
@file qsee_tlmm.h
@brief Provide TLMM API wrappers
*/

/*===========================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
06/18/15   dcf      Initial version.

===========================================================================*/

#include <stdint.h>

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/**
 * Enumeration describing whether a GPIO should be configured for
 * input or output.
 */
typedef enum
{
   QSEE_GPIO_INPUT,
   QSEE_GPIO_OUTPUT,

}qsee_gpio_direction_t;


/** Describes the available weak pulls allowed for GPIOs.
 */
typedef enum
{
  QSEE_GPIO_NO_PULL,
  QSEE_GPIO_PULL_DOWN,
  QSEE_GPIO_KEEPER,
  QSEE_GPIO_PULL_UP,

} qsee_gpio_pull_t;


/** Describes the available drive strengths for GPIOs configured as function
 *  select 0 with output enabled.
 */
typedef enum
{
  QSEE_GPIO_2MA,
  QSEE_GPIO_4MA,
  QSEE_GPIO_6MA,
  QSEE_GPIO_8MA,
  QSEE_GPIO_10MA,
  QSEE_GPIO_12MA,
  QSEE_GPIO_14MA,
  QSEE_GPIO_16MA,

} qsee_gpio_drive_t;


/**
 * Specifies if an output GPIO (with function 0) should be driven low
 * or high.
 */
typedef enum
{
  QSEE_GPIO_LOW,
  QSEE_GPIO_HIGH,

}qsee_gpio_value_t;


/**
 * Specifies whether the GPIO pin should be set to its primary function select
 * or if it should be set to general purpose (function select 0) in order to
 * drive output through SW, etc.
 */
typedef enum
{
  QSEE_GPIO_MODE_PRIMARY,
  QSEE_GPIO_MODE_GENERAL,

}qsee_gpio_mode_t;


/**
 * Structure to specify a particular configuration for a GPIO pin.
 */
typedef struct
{
  qsee_gpio_direction_t direction;
  qsee_gpio_pull_t      pull;
  qsee_gpio_drive_t     drive;

} qsee_tlmm_config_t;


/**
 * Retrieve a GPIO ID (key) based on a signal name string.  On
 * success, this will lock the GPIO to be modifiable only by
 * the holder of the key.
 *
 * @param[in] psz_str - The string signal name to use.
 * @param[out] p_gpio_id - Holder for the GPIO ID
 *
 * @return 0 on success, negative on failure
 */
int qsee_tlmm_get_gpio_id
(
  const char* psz_str,
  uint32_t*   p_gpio_id
);


/**
 * Releases a GPIO ID (key).  On success, this will unlock
 * the GPIO and allow it to be claimed by another entity.  This
 * API is optional and generally for use when a GPIO must be
 * shared between drivers that cannot share the GPIO ID.
 *
 * @param[in] gpio_id - Holder for the GPIO ID
 *
 * @return 0 on success, negative on failure
 */
int qsee_tlmm_release_gpio_id
(
  uint32_t     gpio_id
);


/**
 * Allows the holder of a GPIO ID key to change the function select
 * of the GPIO corresponding to the GPIO ID to either software driven
 * (function select 0) or its primary function select.
 *
 * @param[in] gpio_id - Holder for the GPIO ID
 * @param[in] gpio_mode - The mode to program
 *
 * @return 0 on success, negative on failure
 */
int qsee_tlmm_select_gpio_id_mode
(
  uint32_t             gpio_id,
  qsee_gpio_mode_t     gpio_mode
);


/**
 * Allows the holder of a GPIO ID key to program the GPIO
 * settings (direction, drive strength, and pull) on a the
 * pin corresponding to the GPIO ID key.
 *
 * @param[in] gpio_id - Holder for the GPIO ID
 * @param[in] p_settings - Pointer to a TLMM GPIO configuration
 *                         to set.
 *
 * @return 0 on success, negative on failure
 */
int qsee_tlmm_config_gpio_id
(
  uint32_t            gpio_id,
  qsee_tlmm_config_t* p_settings
);


/**
 * Allows the holder of a GPIO ID key to drive the output value
 * of the GPIO corresponding to the ID high or low when the
 * function select is set to 0 and the direction is output.
 *
 * @param[in] gpio_id - Holder for the GPIO ID
 * @param[in] output_val - The value to drive the GPIO
 *
 * @return 0 on success, negative on failure
 */
int qsee_tlmm_gpio_id_out
(
  uint32_t          gpio_id,
  qsee_gpio_value_t output_val
);


/**
 * Allows the holder of a GPIO ID key to retrieve the input value
 * of the GPIO corresponding to the ID.  The value will read high
 * or low.  Note:  This is not the same value written by the
 * qsee_tlmm_gpio_id_out API.
 *
 * @param[in] gpio_id - Holder for the GPIO ID
 * @param[in] input_val - The value to drive the GPIO
 *
 * @return 0 on success, negative on failure
 */
int qsee_tlmm_gpio_id_in
(
  uint32_t            gpio_id,
  qsee_gpio_value_t*  input_val
);


#endif
