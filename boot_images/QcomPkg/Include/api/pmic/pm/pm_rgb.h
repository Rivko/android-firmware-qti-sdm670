#ifndef __PM_RGB_H__
#define __PM_RGB_H__

/*! \file pm_rgb.h
 *  \n
 *  \brief  PMIC RGB driver APIs
 *  \details  This file contains functions and variable declarations to support
 *   the PMIC RGB LED module.
 *
 *
 *  \n &copy; Copyright (c) 2012-2017  by QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/26/17   sv      Added get Rgb picIndex API .
05/01/17   aab     Added pm_rgb_specific_data_type: used for RGB related config data
11/05/15   al      Syncing to latest
08/22/14   aab     Updated LED brightness level
03/31/14   akm     Merged pm_rgb_enable() and pm_rgb_enable_atc() API
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "pm_lpg.h"

/*===========================================================================
                     GLOBAL TYPE DEFINITIONS
===========================================================================*/

/*! \enum pm_rgb_which_type
 *  \brief Type for selecting the available RGB peripherals.
 */
typedef enum
{
  PM_RGB_1,
  PM_RGB_INVALID
} pm_rgb_which_type;

/*
 * LED bitmasks.
 */
#define PM_RGB_SEGMENT_ALL    0x07
/** Red color. */
#define PM_RGB_SEGMENT_R      0x04
/** Green color. */
#define PM_RGB_SEGMENT_G      0x02
/** Blue color. */
#define PM_RGB_SEGMENT_B      0x01


/* 
 * LED brightness level
 */
#define PM_RGB_DIM_LEVEL_LOW  0x080
#define PM_RGB_DIM_LEVEL_MID  0x100
#define PM_RGB_DIM_LEVEL_HIGH 0x180
#define PM_RGB_DIM_LEVEL_MAX  0x1FF

typedef struct 
{
   uint32            rgb_dim_level_min; 
   uint32            rgb_dim_level_max; 
   pm_lpg_chan_type  red_led_lpg_ch;
   pm_lpg_chan_type  green_led_lpg_ch;
   pm_lpg_chan_type  blue_led_lpg_ch;
}pm_rgb_specific_data_type;




/*! \enum pm_rgb_voltage_source_type
 *  \brief Type for selecting the possible RGB LED voltage sources.
 */
typedef enum
{
  PM_RGB_VOLTAGE_SOURCE_GND,
  /**ground, no power */ 
  PM_RGB_VOLTAGE_SOURCE_VPH,
  /**VPH_PWR / VSYS */ 
  PM_RGB_VOLTAGE_SOURCE_VINRGB_VBOOST,
  /**VIN_RGB / Vboost_bypass */ 
  PM_RGB_VOLTAGE_SOURCE_5V,
  /**5V supply */ 
  PM_RGB_VOLTAGE_SOURCE_4P2V,
  /**4.2v internally clamped by charger module */ 
  PM_RGB_VOLTAGE_SOURCE_MIN_VCHG_5V,
  /**min VCHG 5V internally clamped by charger module */ 
  PM_RGB_VOLTAGE_SOURCE_INVALID
  /**Invalid source */ 
} pm_rgb_voltage_source_type;


/*! \enum pm_rgb_status_type
 *  \brief Structure for RGB status.
 */
typedef struct pm_rgb_status_type
{
  uint32                      enabled_mask;
  uint32                      enabled_atc_mask;
  pm_rgb_voltage_source_type  voltage_source;
} pm_rgb_status_type;

/*! \enum pm_rgb_uasge_type
 *  \brief Structure for RGB Uasge type.
 */
typedef enum
{
  PM_RGB_CHARGER_LED, /* SCHG RGB*/
  //PM_RGB_NOTIFICATION_LED, /* Note used for now*/
  PM_RGB_USE_INVALID

} pm_rgb_uasge_type;

typedef struct 
{
   uint32 charger;            // RGB Charger LED PMIC device index
   //uint32 notification      //RGB Notofication LED Device index
}pm_rgb_pmic_index_type;

/*===========================================================================
                          FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION  pm_rgb_set_voltage_source

DESCRIPTION
  This function sets the voltage source for the given RGB peripheral.

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: rgb
      - Selects which RGB peripheral.
   3) Name: source
      - Voltage source to use for the RGB.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG_INVALID_PARAMETER     -> Invalid voltage source.
     - PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this
                                             version of the PMIC.
     - PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.

DEPENDENCIES
   The following function must have been called:
   1) pm_init()
===========================================================================*/
pm_err_flag_type pm_rgb_set_voltage_source
(
  uint32                      pmic_device_index,
  pm_rgb_which_type           rgb,
  pm_rgb_voltage_source_type  source
);


/*===========================================================================
FUNCTION  pm_rgb_enable

DESCRIPTION
  This function enables the invidual segments of a RGB LED.

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: rgb
      - Selects which RGB peripheral.
   3) Name: rgb_mask
      - Mask of the RGB segments to enable/disable (PM_RGB_SEGMENT_*).
   4) Name: enable
      - Boolean to enable/disable the indicated RGB segments.
   5) Name: atc
      - Boolean to enable/disable the ATC mode of the indicated RGB segments.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this
                                             version of the PMIC.
     - PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.

DEPENDENCIES
   The following function must have been called:
   1) pm_init()
===========================================================================*/
pm_err_flag_type pm_rgb_enable
(
  uint32              pmic_device_index,
  pm_rgb_which_type   rgb,
  uint32              rgb_mask,
  boolean             enable,
  boolean             atc
);

/*===========================================================================
FUNCTION  pm_rgb_get_status

DESCRIPTION
  This function returns the status of the RGB LED.

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: rgb
      - Selects which RGB peripheral.
   3) Name: status
      - Structure to store the status.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this
                                             version of the PMIC.
     - PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.

DEPENDENCIES
   The following function must have been called:
   1) pm_init()
===========================================================================*/
pm_err_flag_type pm_rgb_get_status
(
  uint32              pmic_device_index,
  pm_rgb_which_type   rgb,
  pm_rgb_status_type  *status
);



/*===========================================================================
FUNCTION  pm_rgb_led_config

DESCRIPTION
  This function enables toggling RGB LEDs.

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC :
         Valid Value:  0
   2) Name: rgb_peripheral
      - Selects which RGB peripheral:
         Valid value:  PM_RGB_1
   3) Name: rgb_mask
      - Selects which RGB LED to toggle
        Valid Values:
          PM_RGB_SEGMENT_R
          PM_RGB_SEGMENT_G
          PM_RGB_SEGMENT_B
          PM_RGB_SEGMENT_ALL
   4) Name: voltage_source_type
      - Selects which voltage source to use for RGB pripheral
        Valid values:
          PM_RGB_VOLTAGE_SOURCE_GND,
          PM_RGB_VOLTAGE_SOURCE_VPH,
          PM_RGB_VOLTAGE_SOURCE_5V,
          PM_RGB_VOLTAGE_SOURCE_MIN_VCHG_5V,
   5) Name: dim_level
      - Selects the RGB LED brightness level
        Valid values should be between:
          PM_RGB_DIM_LEVEL_MIN  0x00   and  
          PM_RGB_DIM_LEVEL_MAX  0x1FF (Min/Max levels are Configurable Per target)
   6) Name: enable_rgb
          TRUE:  Enable LED
          FALSE: Disable LED
 
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG_INVALID_PARAMETER -> Invalid input parameter
     - PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this
                                             version of the PMIC.
     - PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.
 
 USAGE EXAMPLE:
   pm_rgb_led_config(0, PM_RGB_1, PM_RGB_SEGMENT_R,  PM_RGB_VOLTAGE_SOURCE_VPH, 0x018, TRUE);
 
DEPENDENCIES
   The following function must have been called:
   1) pm_driver_init()
===========================================================================*/
pm_err_flag_type pm_rgb_led_config
(
   unsigned                   pmic_device_index,
   pm_rgb_which_type          rgb_peripheral,
   uint32                     rgb_mask,
   pm_rgb_voltage_source_type source_type,
   uint32                     dim_level,
   boolean                    enable_rgb
);

/*===========================================================================
FUNCTION  pm_rgb_get_pmic_index

DESCRIPTION
  This function returns the pmic index of the given RGB module.

PARAMETERS
   1) Name: pm_rgb_uasge_type
      - Selects which RGB usage type.
   2) Name: rgb_chg_pmic_index
      - Selects which pmic hase given RGB.


RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this
                                             version of the PMIC.
     - PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.
DEPENDENCIES
   The following function must have been called:
   1) pm_init()
===========================================================================*/

pm_err_flag_type pm_rgb_get_pmic_index(pm_rgb_uasge_type Rgb_led_type, uint32 *rgb_chg_pmic_index);

#endif

