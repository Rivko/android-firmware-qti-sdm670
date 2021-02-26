/*! \file
*  
*  \brief  pm_app_rgb_led.c
*  \details Implementation file for rgb led resourece type.
*
*  &copy; Copyright 2013-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/29/17   sv      Added pm_rgb_get_pmic_index API
05/01/17   aab     Updated pm_rgb_led_config() to use max/min dim level config data
05/17/15   aab     Updated LPG Channel assignment to support 8998 target
03/31/15   aab     Added a call to select PWM source: pm_lpg_pwm_src_select()
06/24/14   aab     Cleaned up and added support for PMI8994
03/13/13   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_rgb.h"
#include "pm_lpg.h"
#include "pm_err_flags.h"
#include "pm_target_information.h"
#include "CoreVerify.h"


pm_err_flag_type pm_rgb_led_config
(
   unsigned                   device_index,
   pm_rgb_which_type          rgb_peripheral,
   uint32                     rgb_mask,
   pm_rgb_voltage_source_type source_type,
   uint32                     dim_level,
   boolean                    enable_rgb) 
{
   pm_err_flag_type err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_lpg_chan_type lpg_channel = PM_LPG_CHAN_3;
   pm_rgb_specific_data_type *rgb_specific_data_ptr;
 
   rgb_specific_data_ptr = (pm_rgb_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_RGB_SPECIFIC_DATA);
   if(rgb_specific_data_ptr == NULL)
   {
      return PM_ERR_FLAG_INVALID_PARAMETER; 
   }


   if (rgb_peripheral >= PM_RGB_INVALID) 
   {
      return PM_ERR_FLAG_INVALID_PARAMETER;
   }

   if (source_type >= PM_RGB_VOLTAGE_SOURCE_INVALID) 
   {
      return PM_ERR_FLAG_INVALID_PARAMETER;
   }

   if ((dim_level < rgb_specific_data_ptr->rgb_dim_level_min) || 
       (dim_level > rgb_specific_data_ptr->rgb_dim_level_max))
   {
      return PM_ERR_FLAG_INVALID_PARAMETER;
   }

   //LPG Channel Configuration
   if (rgb_mask & PM_RGB_SEGMENT_R) 
   {
      lpg_channel = rgb_specific_data_ptr->red_led_lpg_ch;
   }
   else if (rgb_mask & PM_RGB_SEGMENT_G) 
   {
      lpg_channel = rgb_specific_data_ptr->green_led_lpg_ch;
   }
   else if (rgb_mask & PM_RGB_SEGMENT_B) 
   {
      lpg_channel = rgb_specific_data_ptr->blue_led_lpg_ch;
   }
   else
   {
      return PM_ERR_FLAG_INVALID_PARAMETER;
   }

   err_flag = pm_rgb_set_voltage_source(device_index, rgb_peripheral, source_type); //Select RGB voltage source
   if (err_flag != PM_ERR_FLAG_SUCCESS) 
   {
      return err_flag;
   }


   //LPG configuration
   if (enable_rgb == TRUE) 
   {
      err_flag |= pm_lpg_pattern_config(device_index, lpg_channel, FALSE, FALSE, FALSE, FALSE, FALSE); //Configure for no pattern
      err_flag |= pm_lpg_pwm_clock_sel(device_index, lpg_channel, PM_LPG_PWM_19_2_MHZ);  //Select 19.2 MHz clock
      err_flag |= pm_lpg_pwm_set_pre_divide(device_index, lpg_channel, PM_LPG_PWM_PRE_DIV_5, PM_LPG_PWM_FREQ_EXPO_7); //Configure 390 Hz PWM frequency
      err_flag |= pm_lpg_config_pwm_type(device_index, lpg_channel, FALSE);
      err_flag |= pm_lpg_pwm_set_pwm_value(device_index, lpg_channel, dim_level);  //Configure DIM level
      err_flag |= pm_lpg_set_pwm_bit_size(device_index, lpg_channel, PM_LPG_PWM_7BIT);    //Configure 7 bit mode
      err_flag |= pm_lpg_pwm_src_select (device_index, lpg_channel,PM_LPG_PWM_SRC_PWM_REGISTER );
      err_flag |= pm_lpg_pwm_enable(device_index, lpg_channel, TRUE); //enable LPG

      if (err_flag != PM_ERR_FLAG_SUCCESS) 
      {
         return err_flag;
      }
   }
   else
   {
      err_flag |= pm_lpg_pwm_enable(device_index, lpg_channel, FALSE); //Disable LPG
      if (err_flag != PM_ERR_FLAG_SUCCESS) 
      {
         return err_flag;
      }
   }


   //Turn ON/OFF RGB LED
   err_flag |= pm_rgb_enable(device_index, rgb_peripheral, rgb_mask, enable_rgb, FALSE); 

   return err_flag;
}

pm_err_flag_type pm_rgb_get_pmic_index(pm_rgb_uasge_type Rgb_led_type, uint32 *rgb_chg_pmic_index)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_rgb_pmic_index_type *rgb_pmic_index_data_ptr = NULL;   
    rgb_pmic_index_data_ptr = (pm_rgb_pmic_index_type*)pm_target_information_get_specific_info(PM_PROP_RGB_PMIC_INDEX);
    if(rgb_pmic_index_data_ptr == NULL)
    {
      return PM_ERR_FLAG_INVALID_PARAMETER; 
    }
    if(Rgb_led_type == PM_RGB_CHARGER_LED)
    {
        *rgb_chg_pmic_index = rgb_pmic_index_data_ptr->charger;
    }
    else
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    
    return err_flag; 
}
