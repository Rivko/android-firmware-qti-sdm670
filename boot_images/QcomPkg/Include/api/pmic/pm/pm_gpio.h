#ifndef __PM_GPIO_H__
#define __PM_GPIO_H__

/*! \file  pm_gpio.h
*  \n
 *  \brief  PMIC-GPIO MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support 
*   the PMIC GPIO module.
*
*
*  \n Copyright 2010-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/31/14   akm     Removed unused APIs
03/20/13   aab     Adding IRQ related APIs 
12/06/12   hw      Remove comdef.h and use com_dtypes.h
07/05/12   hs      Updated the interface.
03/01/12   hs    Updated the interface.
04/26/11   wra     Adding more GPIO enumeration needed from PM8921
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
                    Added struct pm_gpio_config_type
                    Added pm_gpio_config() and pm_gpio_set_interrupt_polarity()
08/11/10   wra     Removed doxygen quotations. They are causing parsing errors
07/26/10   wra     Changed documentation from @example to Examples so Deoxygen can parse the file
07/09/10   jtn     Added API to get GPIO status
07/02/10   wra     Changed pm_gpio_perph_index to int
06/23/10   vk      Added pm_gpio_set_mux_ctrl()
03/15/10   fpe     Removed RPC remoting because the application processor can do this directly
10/20/09   jtn     Move init function prototype to pm_device_init_i.h
10/19/09   vk      Removed init API
08/01/09   vk      Modified gpio_digital_input prototype
05/20/09   vk      New file
===========================================================================*/
/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/*! \enum pm_gpio_perph_index
 *  \brief Type definition for different GPIOs
*/
typedef enum
{
    PM_GPIO_1,     /**< GPIO 1. */
    PM_GPIO_2,     /**< GPIO 2. */
    PM_GPIO_3,     /**< GPIO 3. */
    PM_GPIO_4,     /**< GPIO 4. */
    PM_GPIO_5,     /**< GPIO 5. */
    PM_GPIO_6,     /**< GPIO 6. */
    PM_GPIO_7,     /**< GPIO 7. */
    PM_GPIO_8,     /**< GPIO 8. */
    PM_GPIO_9,     /**< GPIO 9. */
    PM_GPIO_10,    /**< GPIO 10. */
    PM_GPIO_11,    /**< GPIO 11. */
    PM_GPIO_12,    /**< GPIO 12. */
    PM_GPIO_13,    /**< GPIO 13. */
    PM_GPIO_14,    /**< GPIO 14. */
    PM_GPIO_15,    /**< GPIO 15. */
    PM_GPIO_16,    /**< GPIO 16. */
    PM_GPIO_17,    /**< GPIO 17. */
    PM_GPIO_18,    /**< GPIO 18. */
    PM_GPIO_19,    /**< GPIO 19. */
    PM_GPIO_20,    /**< GPIO 20. */
    PM_GPIO_21,    /**< GPIO 21. */
    PM_GPIO_22,    /**< GPIO 22. */
    PM_GPIO_23,    /**< GPIO 23. */
    PM_GPIO_24,    /**< GPIO 24. */
    PM_GPIO_25,    /**< GPIO 25. */
    PM_GPIO_26,    /**< GPIO 26. */
    PM_GPIO_27,    /**< GPIO 27. */
    PM_GPIO_28,    /**< GPIO 28. */
    PM_GPIO_29,    /**< GPIO 29. */
    PM_GPIO_30,    /**< GPIO 30. */
    PM_GPIO_31,    /**< GPIO 31. */
    PM_GPIO_32,    /**< GPIO 32. */
    PM_GPIO_33,    /**< GPIO 33. */
    PM_GPIO_34,    /**< GPIO 34. */
    PM_GPIO_35,    /**< GPIO 35. */
    PM_GPIO_36,    /**< GPIO 36. */
    PM_GPIO_37,    /**< GPIO 37. */
    PM_GPIO_38,    /**< GPIO 38. */
    PM_GPIO_39,    /**< GPIO 39. */
    PM_GPIO_40,    /**< GPIO 40. */
    PM_GPIO_41,    /**< GPIO 41. */
    PM_GPIO_42,    /**< GPIO 42. */
    PM_GPIO_43,    /**< GPIO 43. */
    PM_GPIO_44     /**< GPIO 44. */ 
}pm_gpio_perph_index;

/*! \enum pm_gpio_volt_src_type
 *  \brief Select voltage source
*/
typedef enum
{
    PM_GPIO_VIN0,  /**< Voltage input 0. */
    PM_GPIO_VIN1,  /**< Voltage input 1. */
    PM_GPIO_VIN2,  /**< Voltage input 2. */
    PM_GPIO_VIN3,  /**< Voltage input 3. */
    PM_GPIO_VIN4,  /**< Voltage input 4. */
    PM_GPIO_VIN5,  /**< Voltage input 5. */
    PM_GPIO_VIN6,  /**< Voltage input 6. */
    PM_GPIO_VIN7,  /**< Voltage input 7. */
    PM_GPIO_VOLTAGE_SOURCE_TYPE__INVALID
}pm_gpio_volt_src_type;


/*! \enum pm_gpio_invert_ext_pin_type
 *  \brief Invert/Not Invert the output of ext pin
 */
typedef enum
{
    PM_GPIO_OUT_BUFFER_CONFIG_CMOS,             /**< CMOS output. */
    PM_GPIO_OUT_BUFFER_CONFIG_OPEN_DRAIN_NMOS,  /**< Open drain NMOS output. */
    PM_GPIO_OUT_BUFFER_CONFIG_OPEN_DRAIN_PMOS,  /**< Open drain PMOS output. */
    PM_GPIO_OUT_BUFFER_CONFIG__INVALID
}pm_gpio_out_buffer_config_type;

/*! \enum pm_gpio_out_buffer_config_type
 *  \brief select Output buffer configuration
*/
typedef enum
{
  PM_GPIO_INVERT_EXT_PIN_OUTPUT_DISABLE, /**< Ext pin output is not inverted. */
  PM_GPIO_INVERT_EXT_PIN_OUTPUT_ENABLE,  /**< Ext pin output is inverted. */
  PM_GPIO_INVERT_EXT_PIN_OUTPUT__INVALID 
}pm_gpio_invert_ext_pin_type;

/*! \enum pm_gpio_out_buffer_drv_strength_type
 *  \brief select output buffer strength
*/
typedef enum
{
    PM_GPIO_OUT_BUFFER_RESERVED,  /**< Output buffer strength reserved. */
    PM_GPIO_OUT_BUFFER_LOW,       /**< Output buffer strength low. */
    PM_GPIO_OUT_BUFFER_MEDIUM,    /**< Output buffer strength medium. */
    PM_GPIO_OUT_BUFFER_HIGH,      /**< Output buffer strength high. */
    PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH__INVALID
}pm_gpio_out_buffer_drv_strength_type;

/*! \enum pm_gpio_current_src_pulls_type
 *  \brief select current source pulls type
*/
typedef enum
{
    PM_GPIO_I_SOURCE_PULL_UP_30uA,     /**< Pull up 30 uA. */
    PM_GPIO_I_SOURCE_PULL_UP_1_5uA,    /**< Pull up 1.5 uA. */
    PM_GPIO_I_SOURCE_PULL_UP_31_5uA,   /**< Pull up 31.5 uA. */
    PM_GPIO_I_SOURCE_PULL_UP_1_5uA_PLUS_30uA_BOOST,  /**< Pull up 1.5 uA
                                                          plus 30 uA boost. */
    PM_GPIO_I_SOURCE_PULL_DOWN_10uA,   /**< Pull down 10 uA. */
    PM_GPIO_I_SOURCE_PULL_NO_PULL,     /**< No pull. */
    PM_GPIO_CURRENT_SOURCE_PULLS_TYPE__INVALID
}pm_gpio_current_src_pulls_type;


/*! \enum pm_gpio_src_config_type
 *  \brief Source select
*/
typedef enum
{
    PM_GPIO_SOURCE_GND,                  /**< Ground. */
    PM_GPIO_SOURCE_PAIRED_GPIO,          /**< Paired GPIO. */
    PM_GPIO_SOURCE_SPECIAL_FUNCTION1,    /**< Special function 1. */
    PM_GPIO_SOURCE_SPECIAL_FUNCTION2,    /**< Special function 2. */
    PM_GPIO_SOURCE_SPECIAL_FUNCTION3,    /**< Special Function 3. */
    PM_GPIO_SOURCE_SPECIAL_FUNCTION4,    /**< Special Function 4. */  
    PM_GPIO_SOURCE_DTEST1,               /**< D-test 1. */
    PM_GPIO_SOURCE_DTEST2,               /**< D-test 2. */
    PM_GPIO_SOURCE_DTEST3,               /**< D-test 3. */
    PM_GPIO_SOURCE_DTEST4,               /**< D-test 4. */
    PM_GPIO_SOURCE_CONFIG_TYPE__INVALID
}pm_gpio_src_config_type;

/** D-test buffer enable/disable.
*/
typedef enum
{
    PM_GPIO_DTEST_DISABLE,  /**< GPIO D-test disable. */
    PM_GPIO_DTEST_ENABLE,   /**< GPIO D-test enable. */
    PM_GPIO_DTEST_BUFFER_ON_OFF_TYPE__INVALID 
}pm_gpio_dtest_buffer_on_off_type;

/** External pin configuration.
*/
typedef enum
{
    PM_GPIO_EXT_PIN_DISABLE,  /**< Disable EXT_PIN. */
    PM_GPIO_EXT_PIN_ENABLE,   /**< Puts EXT_PIN at high Z state and disables
                                   the block. */
    PM_GPIO_EXT_PIN_CONFIG_TYPE__INVALID    
}pm_gpio_ext_pin_config_type;

/*! \enum pm_gpio_uart_path_type
 *  \brief UART MUX configuration
*/
typedef enum
{
    PM_UART_MUX_NO,      /**< No MUX. */
    PM_UART_MUX_1,       /**< MUX 1. */
    PM_UART_MUX_2,       /**< MUX 2. */
    PM_UART_MUX_3,       /**< MUX 3. */
    PM_UART_MUX_INVALID
}pm_gpio_uart_path_type;

/*! \enum pm_gpio_dig_in_type
 *  \brief digital input dtest types
*/
typedef enum
{
    PM_GPIO_DIG_IN_DTEST1,              /**< DTEST1 for GPIO DIG IN CTL  */
    PM_GPIO_DIG_IN_DTEST2,              /**< DTEST2 for GPIO DIG IN CTL  */
    PM_GPIO_DIG_IN_DTEST3,              /**< DTEST3 for GPIO DIG IN CTL  */
    PM_GPIO_DIG_IN_DTEST4,              /**< DTEST4 for GPIO DIG IN CTL  */
    PM_GPIO_DIG_IN_DTEST_TYPE__INVALID  /**< Invalid DTEST for GPIO DIG IN CTL */
}pm_gpio_dig_in_type;



/**
 * Enumeration for GPIO output modes
 */
typedef enum
{

    PM_GPIO_INPUT_ON,             /**< GPIO is configured as input */
    PM_GPIO_INPUT_OUTPUT_ON,      /**< GGPIO is configured as input and output */
    PM_GPIO_OUTPUT_ON,            /**< GPIO is configured as output */
    PM_GPIO_ANA_PASS_THRU,      /**< GPIO is configured as Analog Pass Through */   
    PM_GPIO_INPUT_OUTPUT_OFF      /**< Both input and output are off */
} pm_gpio_mode_select_type;

  /**
* Structure used to return GPIO status. This structure includes all of the
* enums that are used when configuring the GPIOs.
*/
typedef struct 
{
    pm_gpio_mode_select_type               gpio_mode_select;       /**< GPIO mode selection */
    pm_gpio_volt_src_type                  gpio_volt_src;          /**< GPIO voltage source. */
    pm_gpio_out_buffer_config_type         gpio_out_buffer_config; /**< GPIO output buffer configuration. */
    pm_gpio_invert_ext_pin_type            gpio_invert_ext_pin;    /**< GPIO invert EXT_PIN configuration. */
    pm_gpio_out_buffer_drv_strength_type   gpio_out_buffer_drv_strength; /**< GPIO output buffer driver strength. */
    pm_gpio_current_src_pulls_type         gpio_current_src_pulls;       /**< GPIO current source pulls. */
    pm_gpio_src_config_type                gpio_src_config;              /**< GPIO source configuration. */
    pm_gpio_dtest_buffer_on_off_type       gpio_dtest_buffer_on_off;     /**< GPIO D-text buffer on/off. */
    pm_gpio_ext_pin_config_type            gpio_ext_pin_config;          /**< GPIO EXT_PIN configuration. */
} pm_gpio_status_type;

/**
 * Structure to be used in conjunction with pm_gpio_config().  
 * This structure includes all fields that are available when configuring the GPIOs
*/
typedef struct 
{
    pm_gpio_perph_index gpio;                                   /**< Determines which GPIO to configure. */
    boolean config_gpio;                                        /**< Determines whether the GPIO is to be configured. */
    pm_gpio_volt_src_type gpio_volt_src;                        /**< GPIO voltage source. */
    pm_gpio_out_buffer_config_type gpio_out_buffer_config;      /**< GPIO output buffer configuration. */
    boolean invert_ext_pin;                                      /**< Determines whether the EXT_PIN is to be inverted.  */
    pm_gpio_current_src_pulls_type gpio_current_src_pulls;       /**< GPIO current source pull configuration. */
    pm_gpio_out_buffer_drv_strength_type gpio_out_buffer_drv_strength; /**< GPIO output buffer drive strength. */
    pm_gpio_dtest_buffer_on_off_type gpio_dtest_buffer_on_off;    /**< GPIO D-test buffer configuration. */ 
    pm_gpio_ext_pin_config_type gpio_ext_pin_config;              /**< GPIO EXT_PIN configuration. */
    pm_gpio_src_config_type gpio_src_config;                      /**< GPIO source configuration. */
    boolean interrupt_polarity;                                   /**< Interrupt polarity. */
} pm_gpio_config_type;



/**
*  Returns the status of one of the PMIC GPIOs.
 * 
* @param[in] pmic_chip Each PMIC device in the systems is enumerated,
*                              starting with zero.
* @param[in] gpio GPIO identifier.
* @param[out] gpio_status Pointer to the GPIO status.
 * 
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*/
pm_err_flag_type pm_gpio_status_get(uint8 pmic_chip, pm_gpio_perph_index gpio , pm_gpio_status_type *gpio_status);
/*~ FUNCTION  pm_gpio_status_get */
/*~ PARAM OUT gpio_status POINTER */

/*===========================================================================
FUNCTION DEFINITIONS
===========================================================================*/

/*=========================================================================== */
/*                        pm_gpio_config_bias_voltage                         */
/*=========================================================================== */

/**
*  Configures the selected GPIO for bias voltage.
*
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                        starting with 0.
* @param[in] gpio GPIO to configure for bias voltage.
*                 See #pm_gpio_which_type.
* @param[in] voltage_source GPIO voltage source. See #pm_gpio_voltage_source_type.
 * 
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
 * 
*  <b>Example </b> \n
*   Configure GPIO5 to a logic high referencing to an analog (quiet) supply:
* @code
*  errFlag = pm_gpio_config_bias_voltage(PM_GPIO_5, 
*                                                  PM_GPIO_VIN4); @endcode
*/
pm_err_flag_type pm_gpio_config_bias_voltage
(
 uint8                          pmic_chip,
 pm_gpio_perph_index                 gpio,
 pm_gpio_volt_src_type        voltage_source
);

/*=========================================================================== */
/*                        pm_gpio_config_digital_input                        */
/*=========================================================================== */

/**
*   Configures the selected GPIO as a digital input.
*
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                              starting with zero.
* @param[in] gpio GPIO to configure as a digital input. See #pm_gpio_which_type.
* @param[in] i_source_pulls Current source pulls. See
*                           #pm_gpio_current_source_pulls_type.
* @param[in] voltage_source GPIO voltage source. See
*                           #pm_gpio_voltage_source_type.
* @param[in] out_buffer_strength GPIO output buffer strength. See
*                                #pm_gpio_out_buffer_drive_strength_type.
* @param[in] source Select the source. See #pm_gpio_source_config_type.
* 
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
 * 
*  <b>Example </b> \n
*  The keypad scan module requires GPIO to drive and sense the keypad. \n
*  The keypad drive signal is to be configured as an open_drain output with
*  low drive strength. \n
*  The keypad sense module is to be configured as an input with 1.5 uA pull up
*  + 30 uA boost, \n
*  e.g., reference voltage VIN2 for both drive and sense lines. \n
*  Configure GPIO5 as a sense line:
* @code
*  errFlag = pm_gpio_config_digital_input(PM_GPIO_5, 
*                               PM_GPIO_I_SOURCE_PULL_UP_1_5uA_PLUS_30uA_BOOST,
*                               PM_GPIO_VIN2,
*                               PM_GPIO_OUT_BUFFER_OFF,
*                               PM_GPIO_SOURCE_GND); @endcode
*/
pm_err_flag_type pm_gpio_config_digital_input
(
 uint8                           pmic_chip,
 pm_gpio_perph_index                 gpio,
 pm_gpio_current_src_pulls_type  i_source_pulls,
 pm_gpio_volt_src_type        voltage_source,
 pm_gpio_out_buffer_drv_strength_type  out_buffer_strength,
 pm_gpio_src_config_type         source
);

/*======================================================================= */
/*                        pm_gpio_config_digital_output                   */
/*======================================================================= */

/**
*   Configures the selected GPIO as a digital output.
*
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                              starting with zero.
* @param[in] gpio GPIO to configure as digital output. See
*                 #pm_gpio_which_type.
* @param[in] out_buffer_config GPIO output buffer configuration (CMOS or open
*                              drain). See #pm_gpio_out_buffer_config_type.
* @param[in] voltage_source GPIO voltage source. See
*                           #pm_gpio_voltage_source_type.
* @param[in] source Select the source. See #pm_gpio_source_config_type.
* @param[in] out_buffer_strength GPIO output buffer strength. See
*                                #pm_gpio_out_buffer_drive_strength_type.
* @param[in] out_inversion Invert the output of EXT_PIN.
* 
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
 * 
*  <b>Example </b> \n
*  The keypad scan module requires GPIO to drive and sense the keypad. \n
*  The keypad drive signal is to be configured as an open-drain output with
*  low drive strength \n
*  The keypad sense module is to be configured as an input with 1.5uA pull up
*  + 30uA boost, \n
*  e.g., reference voltage VIN2 for both drive and sense lines. \n
*  Configure GPIO5 as a drive signal:
* @code
*  errFlag = pm_gpio_config_digital_output(PM_GPIO_5, 
*                                         PM_GPIO_OUT_BUFFER_CONFIG_OPEN_DRAIN,
*                                         PM_GPIO_VIN2,
*                                         PM_GPIO_SOURCE_SPECIAL_FUNCTION1,
*                                         PM_GPIO_OUT_BUFFER_LOW,
*                                         FALSE); @endcode
*/

pm_err_flag_type pm_gpio_config_digital_output
(
 uint8                                 pmic_chip,
 pm_gpio_perph_index                   gpio,
 pm_gpio_out_buffer_config_type        out_buffer_config,
 pm_gpio_volt_src_type                 voltage_source,
 pm_gpio_src_config_type               source,
 pm_gpio_out_buffer_drv_strength_type  out_buffer_strength,
 boolean                               out_inversion
);

pm_err_flag_type pm_gpio_config_digital_input_output
(
 uint8                           pmic_chip, 
 pm_gpio_perph_index                 gpio,
 pm_gpio_src_config_type         source,
 pm_gpio_current_src_pulls_type  i_source_pulls,
 pm_gpio_volt_src_type        voltage_source,
 pm_gpio_out_buffer_config_type       out_buffer_config,
 pm_gpio_out_buffer_drv_strength_type out_buffer_strength
);

/*=========================================================================== */
/*                        pm_gpio_set_voltage_source                          */
/*=========================================================================== */

/** 
*  Sets the voltage source.
 * 
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                              starting with zero.
* @param[in] gpio GPIO for which to configure voltage source. See
*                 #pm_gpio_which_type.
* @param[in] voltage_source GPIO voltage source. See
*                           #pm_gpio_voltage_source_type.
*
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
 * 
*  <b>Example </b> \n
*  Set voltage source to VIN2 for GPIO5:
* @code
*  errFlag = pm_gpio_set_voltage_source(PM_GPIO_5, 
*                                                 PM_GPIO_VIN2); @endcode
 */
pm_err_flag_type pm_gpio_set_voltage_source
(
 uint8                           pmic_chip,
 pm_gpio_perph_index                 gpio,
 pm_gpio_volt_src_type        voltage_source
);

/*=========================================================================== */
/*                        pm_gpio_set_output_buffer_configuration            */
/*=========================================================================== */

/**
*  Sets the output buffer configuration.
 * 
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                              starting with zero.
* @param[in] gpio GPIO for which to configure the current source pulls. See
*                 #pm_gpio_which_type.
* @param[in] out_buffer_config GPIO output buffer configuration: 0 -- CMOS;
*                              1 -- open drain. See 
*                              #pm_gpio_out_buffer_config_type.
 * 
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
*  <b>Example </b> \n
*  Set the output buffer configuration to CMOS in bias mode for GPIO5:
* @code
*  errFlag = pm_gpio_set_output_buffer_configuration(PM_GPIO_5, 
*                                              PM_GPIO_OUT_BUFFER_CONFIG_CMOS); @endcode
*/
pm_err_flag_type pm_gpio_set_output_buffer_configuration
(
 uint8                                   pmic_chip, 
 pm_gpio_perph_index                      gpio,
 pm_gpio_out_buffer_config_type          out_buffer_config
);

/*=========================================================================== */
/*                        pm_gpio_set_inversion_configuration()               */
/*=========================================================================== */

/**
*  Sets the inversion configuration.
*
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                              starting with zero.
* @param[in] gpio GPIO for which to set the inversion configuration. See
*                 #pm_gpio_which_type.
* @param[in] inversion TRUE -- Enable inversion. \n
*                      FALSE -- Disable inversion.
 * 
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
 * 
*  <b>Example </b> \n
*  For  GPIO5, set inversion to TRUE:
* @code
*  errFlag = pm_gpio_set_inversion_configuration(PM_GPIO_5, TRUE); @endcode
*/
pm_err_flag_type pm_gpio_set_inversion_configuration
(
 uint8                                   pmic_chip,
 pm_gpio_perph_index                      gpio,
 boolean                   inversion
);
                                    
/*=========================================================================== */
/*                        pm_gpio_set_current_source_pulls                    */
/*=========================================================================== */

/**
*  Sets the current source pulls.
*
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                              starting with zero.
* @param[in] gpio GPIO for which to configure current source pulls. See
*                 #pm_gpio_which_type.
* @param[in] i_source_pulls Current source pulls. See
*                           #pm_gpio_current_source_pulls_type.
 * 
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
*  <b>Example </b> \n
*  Set the current source pulls to 1.5 uA + 30 uA boost for GPIO5:
* @code
*  errFlag = pm_gpio_set_current_source_pulls(PM_GPIO_5, 
*                              PM_GPIO_I_SOURCE_PULL_UP_1_5uA_PLUS_30uA_BOOST); @endcode
*/
pm_err_flag_type pm_gpio_set_current_source_pulls
(
 uint8                           pmic_chip, 
 pm_gpio_perph_index                 gpio,
 pm_gpio_current_src_pulls_type  i_source_pulls
);

/*=========================================================================== */
/*                        pm_gpio_set_ext_pin_config()               */
/*=========================================================================== */

/**
*  Sets the external pin configuration.
 * 
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                              starting with zero.
* @param[in] gpio GPIO for which to configure the EXT_PIN.
*                 See #pm_gpio_which_type.
* @param[in] ext_pin_config Enable or disable the EXT_PIN (disable puts
*                           EXT_PIN at a high Z state and disables the block.
*                           See #pm_gpio_ext_pin_config_type.
 * 
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
*  <b>Example </b> \n
*  To enable a GPIO EXT_PIN:
* @code
*  errFlag = pm_gpio_set_ext_pin_config(PM_GPIO_5, PM_GPIO_EXT_PIN_ENABLE); @endcode
*/

pm_err_flag_type pm_gpio_set_ext_pin_config
(
 uint8                          pmic_chip, 
 pm_gpio_perph_index            gpio,
 pm_gpio_ext_pin_config_type    ext_pin_config
);


/*=========================================================================== */
/*                        pm_gpio_set_output_buffer_drive_strength            */
/*=========================================================================== */

/**
*  Sets the output buffer drive strength.
*
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                              starting with zero.
* @param[in] gpio GPIO for which to configure current source pulls. See
*                 #pm_gpio_which_type.
* @param[in] out_buffer_strength GPIO output buffer drive strength.
*                                #See #pm_gpio_out_buffer_drive_strength_type.
 * 
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
 * 
*  <b>Example </b> \n
*  Set the output buffer drive strength for GPIO5 to HIGH:
* @code
*  errFlag = pm_gpio_set_output_buffer_drv_strength(PM_GPIO_5, 
*                                                   PM_GPIO_OUT_BUFFER_HIGH); @endcode
*/
pm_err_flag_type pm_gpio_set_output_buffer_drive_strength
(
 uint8                                 pmic_chip, 
 pm_gpio_perph_index                      gpio,
 pm_gpio_out_buffer_drv_strength_type  out_buffer_strength
);

/*======================================================================= */
/*                        pm_gpio_set_source_configuration                */
/*======================================================================= */

/**
*  Sets the source configuration.
*
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                              starting with zero.
* @param[in] gpio GPIO for which to set the source configuration.
*                 See #pm_gpio_which_type</td>
* @param[in] source Select the source. See #pm_gpio_source_config_type.
*
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
 * 
*  <b>Example </b> \n
*  To serve GPIO5 as an output in Level Translator mode, select Pair In as
*  the source:
* @code
*  errFlag = pm_gpio_set_source_configuration(PM_GPIO_5, 
*                                                  PM_GPIO_SOURCE_PAIRED_GPIO); @endcode 
*/
pm_err_flag_type pm_gpio_set_source_configuration
(
 uint8                           pmic_chip, 
 pm_gpio_perph_index                 gpio,
 pm_gpio_src_config_type         source
);
                                   
/*======================================================================= */
/*                        pm_gpio_set_mux_ctrl                            */
/*======================================================================= */

/**
*  Sets GPIO MUX control.
*
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                              starting with zero.
* @param[in] uart_path Select the UART path. See #pm_gpio_uart_path_type.
*
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*/
pm_err_flag_type pm_gpio_set_mux_ctrl(pm_gpio_uart_path_type uart_path);

/**
 * @brief This function enables or disables gpio irq
 * 
 * @param[in] pmic_chip. Primary: 0. Secondary: 1
 * @param[in] gpio:  
 *                GPIO number. Refer enum pm_gpio_perph_index.
 *  
 * @param[in] enable:  
 *                TRUE: Enable interrupt for the gpio 
 *                FALSE:Disable interrupt for the gpio
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_gpio_irq_enable
(
  uint8 pmic_chip, 
  pm_gpio_perph_index gpio,
  boolean enable
);


/**
 *  Clears the GPIO IRQ. 
 * 
 * @param[in] pmic_chip Primary -- 0; Secondary -- 1.
 * @param[in] gpio GPIO number. See #pm_gpio_which_type.  
 *
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_gpio_irq_clear
(
  uint8 pmic_chip, 
  pm_gpio_perph_index gpio
);


/**
 *  Configures the GPIO for an IRQ. 
 * 
 * @param[in] pmic_chip Primary -- 0; Secondary -- 1.
 * @param[in] gpio GPIO number. See #pm_gpio_which_type.
 * @param[in] trigger One of the various IRQ triggers. See
 *                    #pm_irq_trigger_type.
 *
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_gpio_irq_set_trigger
(
  uint8 pmic_chip, 
  pm_gpio_perph_index gpio, 
  pm_irq_trigger_type trigger
);


/**
 *  Gets the GPIO IRQ status. 
 * 
 * @param[in] pmic_chip Primary -- 0; Secondary -- 1.
 * @param[in] gpio GPIO number. See #pm_gpio_which_type.
 * @param[in] type Type of IRQ status to read. See #pm_irq_status_type.
 * @param[out] status IRQ status.
 *
 * @return
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_gpio_irq_status
(
  uint8 pmic_chip, 
  pm_gpio_perph_index gpio, 
  pm_irq_status_type type, 
  boolean *status
);


/**
 *  Enables GPIO. 
 * 
 * @param[in] pmic_chip Primary -- 0; Secondary -- 1.
 * @param[in] gpio GPIO number. See #pm_gpio_which_type.
 * @param[in] enable  TRUE to Enable or FALSE to Disable the GPIO.
 *
 * @return
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */

pm_err_flag_type pm_gpio_enable
(
   uint8 pmic_chip, 
   pm_gpio_perph_index  gpio, 
   boolean enable
);

/**
 * @brief This function enables or disables dtest lines for 
 *        digital input ctl
 * 
 * @param[in] pmic_chip. Primary: 0. Secondary: 1
 * @param[in] gpio:  
 *                GPIO number. Refer enum pm_gpio_perph_index.
 * @param[in] dtest: 
 *                GPIO digital input dtest line 
 *  
 * @param[in] enable:  
 *                TRUE:  Enables dtest line
 *                FALSE: Disables dtest line
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_gpio_set_dig_in_ctl(uint32 pmic_chip, pm_gpio_perph_index perph_index, pm_gpio_dig_in_type dig_in, boolean enable);


/** @} */ /* end_addtogroup pm_gpio */

#endif /* __PM_GPIO_H__ */
