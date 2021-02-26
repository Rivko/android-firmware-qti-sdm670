/*! \file rgb_api.c
*  \n
*  \details This file contains the top-level API wrappers for the RGB
*           peripheral.
*
*  \n &copy; Copyright 2015-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/02/15   al      Updaing RGB with latest 
04/06/15   aab     Updating pm_rgb_enable 
08/20/14   al      Updating comm lib 
05/20/14   al      Architecture update
===========================================================================*/

/*===========================================================================
INCLUDE FILES
===========================================================================*/
#include "pm_rgb.h"
#include "pm_rgb_driver.h"

pm_err_flag_type pm_rgb_set_voltage_source
(
  uint32                      pmic_chip,
  pm_rgb_which_type           rgb,
  pm_rgb_voltage_source_type  source
  )
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type reg = 0;
  pm_register_address_type periph_index = (pm_register_address_type)rgb;
  pm_register_data_type data = 0x0;
  pm_rgb_data_type *rgb_ptr = pm_rgb_get_data(pmic_chip);

  if (rgb_ptr == NULL)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  if (periph_index >= rgb_ptr->num_of_peripherals)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  reg = rgb_ptr->rgb_reg_table->base_address + (rgb_ptr->rgb_reg_table->peripheral_offset * periph_index) + rgb_ptr->rgb_reg_table->src_sel;

  switch (source)
  {
    //ToDo: This require update (remove casting enums that doesn't not guarantee sequencing)
    case PM_RGB_VOLTAGE_SOURCE_GND            : data = 0x0;   break;
    case PM_RGB_VOLTAGE_SOURCE_VINRGB_VBOOST  : data = 0x1; break;
    /*below both are 0x3*/
    case PM_RGB_VOLTAGE_SOURCE_VPH            : data = 0x3;   break;
    case PM_RGB_VOLTAGE_SOURCE_4P2V           : data = 0x3;   break;   
    default:
      data = 0x0;
  }

  err_flag = pm_comm_write_byte_mask(rgb_ptr->comm_ptr->slave_id, reg, 0x03, data, 0);  // 1:0

  return err_flag;
}

pm_err_flag_type pm_rgb_enable
(
  uint32              pmic_chip,
  pm_rgb_which_type   rgb,
  uint32              rgb_mask,
  boolean             enable,
  boolean             atc
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_rgb_data_type *rgb_ptr = pm_rgb_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_address_type periph_index = (pm_register_address_type)rgb;
    pm_register_data_type rgb_en_ctl_val;

    if (rgb_ptr == NULL)
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    if (periph_index >= rgb_ptr->num_of_peripherals)
    {
      return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    if (atc)
    {
      reg = rgb_ptr->rgb_reg_table->base_address + (rgb_ptr->rgb_reg_table->peripheral_offset*periph_index) + rgb_ptr->rgb_reg_table->en_ctl_atc;
    }
    else
    {
     reg = rgb_ptr->rgb_reg_table->base_address + (rgb_ptr->rgb_reg_table->peripheral_offset*periph_index) + rgb_ptr->rgb_reg_table->en_ctl;   
    }

    rgb_en_ctl_val = (enable)? (rgb_mask <<5): 0x00; /*<R: Bit 7, G: Bit6, B: Bit 5>*/
    rgb_mask = rgb_mask <<5;

    err_flag = pm_comm_write_byte_mask(rgb_ptr->comm_ptr->slave_id, reg, rgb_mask, rgb_en_ctl_val, 0);
   
    return err_flag;
}


pm_err_flag_type pm_rgb_get_status
(
  uint32              pmic_chip,
  pm_rgb_which_type   rgb,
  pm_rgb_status_type  *status
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_rgb_data_type *rgb_ptr = pm_rgb_get_data(pmic_chip);
    pm_register_address_type periph_index = (pm_register_address_type)rgb;

    pm_register_address_type reg_rgb_en_ctl = 0;
    pm_register_address_type reg_rgb_en_ctl_atc = 0;
    pm_register_address_type reg_rgb_src_sel = 0;

    pm_register_data_type en_ctl_val = 0;
    pm_register_data_type en_ctl_atc_val = 0;
    pm_register_data_type src_sel_val = 0;

    if (rgb_ptr == NULL)
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    if (periph_index >= rgb_ptr->num_of_peripherals)
    {
      return PM_ERR_FLAG_INVALID_PARAMETER;
    }


    reg_rgb_en_ctl     = rgb_ptr->rgb_reg_table->base_address + (rgb_ptr->rgb_reg_table->peripheral_offset*periph_index) + rgb_ptr->rgb_reg_table->en_ctl;
    reg_rgb_en_ctl_atc = rgb_ptr->rgb_reg_table->base_address + (rgb_ptr->rgb_reg_table->peripheral_offset*periph_index) + rgb_ptr->rgb_reg_table->en_ctl_atc;
    reg_rgb_src_sel    = rgb_ptr->rgb_reg_table->base_address + (rgb_ptr->rgb_reg_table->peripheral_offset*periph_index) + rgb_ptr->rgb_reg_table->src_sel;

    err_flag   = pm_comm_read_byte(rgb_ptr->comm_ptr->slave_id, reg_rgb_en_ctl,     &en_ctl_val, 0);
    err_flag  |= pm_comm_read_byte(rgb_ptr->comm_ptr->slave_id, reg_rgb_en_ctl_atc, &en_ctl_atc_val, 0);
    err_flag  |= pm_comm_read_byte(rgb_ptr->comm_ptr->slave_id, reg_rgb_src_sel,    &src_sel_val, 0);

    status->enabled_mask = (en_ctl_val & (1 << 7)) ? PM_RGB_SEGMENT_R : 0;
    status->enabled_mask |= (en_ctl_val & (1 << 6)) ? PM_RGB_SEGMENT_G : 0;
    status->enabled_mask |= (en_ctl_val & (1 << 5)) ? PM_RGB_SEGMENT_B : 0;
    status->enabled_atc_mask = (en_ctl_atc_val & (1 << 7)) ? PM_RGB_SEGMENT_R : 0;
    status->enabled_atc_mask |= (en_ctl_atc_val & (1 << 6)) ? PM_RGB_SEGMENT_G : 0;
    status->enabled_atc_mask |= (en_ctl_atc_val & (1 << 5)) ? PM_RGB_SEGMENT_B : 0;

    switch (src_sel_val & 0x3)
    {
      case 0: status->voltage_source = PM_RGB_VOLTAGE_SOURCE_GND; break;
      case 1: status->voltage_source = PM_RGB_VOLTAGE_SOURCE_VINRGB_VBOOST; break;
      case 3: status->voltage_source = PM_RGB_VOLTAGE_SOURCE_VPH; break;
      default:
        status->voltage_source = PM_RGB_VOLTAGE_SOURCE_INVALID;
    }
    
    return err_flag;
}

