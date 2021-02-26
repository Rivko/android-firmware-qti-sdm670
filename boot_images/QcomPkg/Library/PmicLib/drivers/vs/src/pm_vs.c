/*! \file
*  
*  \brief  pm_vs.c
*  
*  &copy; Copyright 2012-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/15   al      Updated rails API
07/16/14   al      Comm change Updates
08/11/14   al      Adding API to read VREG_OK
05/20/14   al      updated PMIC arch   
12/06/12   hw      Rearchitecturing module driver to peripheral driver
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_vs.h"
#include "pm_vs_driver.h"
#include "pm_comm.h"

/*===========================================================================

                     API IMPLEMENTATION 

===========================================================================*/


pm_err_flag_type 
pm_vs_sw_mode(uint8 pmic_chip, uint8 vs_peripheral_index, pm_sw_mode_type mode) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_vs_data_type *vs_ptr = pm_vs_get_data(pmic_chip);

    if (vs_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if(vs_peripheral_index >= vs_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }
    else if (mode >= PM_SW_MODE_INVALID)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type          reg = 0;
        pm_register_mask_type             mask = 0;
        pm_register_data_type             data = 0;
        pm_pwr_data_type                  *pwr_data = &(vs_ptr->pm_pwr_data);
        
        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[vs_peripheral_index].periph_base_address + pwr_data->pwr_reg_table->MODE_CTL);

        switch(mode)
        {
        case PM_SW_MODE_LPM: // Low power mode
          {
              mask = 0xFF;
              data = 0x00;
          }
          break;
        case PM_SW_MODE_NPM: // normal power mode
          {
              mask = 0xC0; //bit 7 NPM and bit 6 AUTO_MODE
              data = 0x80;
          }
          break;
        case PM_SW_MODE_AUTO: // auto mode
          {
              mask = 0xC0; //bit 7 NPM and bit 6 AUTO_MODE
              data = 0x40;
          }
          break;
        default:
          err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
          break;
        }

        if (PM_ERR_FLAG_SUCCESS == err_flag)
        {
            err_flag = pm_comm_write_byte_mask(vs_ptr->comm_ptr->slave_id, reg, mask, data, 0);
        }
    }

    return err_flag;
}


pm_err_flag_type 
pm_vs_sw_mode_status(uint8 pmic_chip, uint8 vs_peripheral_index, pm_sw_mode_type *mode) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_vs_data_type *vs_ptr = pm_vs_get_data(pmic_chip);

    if (vs_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_mode_status_alg(&(vs_ptr->pm_pwr_data), vs_ptr->comm_ptr, vs_peripheral_index, mode);
    }
    return err_flag;
}

pm_err_flag_type 
pm_vs_pin_ctrled(uint8 pmic_chip, uint8 vs_peripheral_index, uint8 select_pin)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_vs_data_type *vs_ptr = pm_vs_get_data(pmic_chip);

    if (vs_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_pin_ctrl_alg(&(vs_ptr->pm_pwr_data), vs_ptr->comm_ptr, vs_peripheral_index, select_pin);
    }


    return err_flag;
}

pm_err_flag_type 
pm_vs_pin_ctrled_status(uint8 pmic_chip, uint8 vs_peripheral_index, pm_on_off_type* on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_vs_data_type *vs_ptr = pm_vs_get_data(pmic_chip);

    if (vs_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_pin_ctrl_status_alg(&(vs_ptr->pm_pwr_data), vs_ptr->comm_ptr, vs_peripheral_index, on_off);
    }


    return err_flag;
}

pm_err_flag_type 
pm_vs_sw_enable(uint8 pmic_chip, uint8 vs_peripheral_index, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_vs_data_type *vs_ptr = pm_vs_get_data(pmic_chip);

    if (vs_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_enable_alg(&(vs_ptr->pm_pwr_data), vs_ptr->comm_ptr, vs_peripheral_index, on_off);
    }

    return err_flag;
}

pm_err_flag_type
pm_vs_sw_enable_status(uint8 pmic_chip, uint8 vs_peripheral_index, pm_on_off_type* on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_vs_data_type *vs_ptr = pm_vs_get_data(pmic_chip);

    if (vs_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_enable_status_alg(&(vs_ptr->pm_pwr_data), vs_ptr->comm_ptr, vs_peripheral_index, on_off);
    }

    return err_flag;
}


pm_err_flag_type 
pm_vs_pull_down(uint8 pmic_chip, uint8 vs_peripheral_index, pm_on_off_type on_off) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_vs_data_type *vs_ptr = pm_vs_get_data(pmic_chip);

    if (vs_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_pull_down_alg(&(vs_ptr->pm_pwr_data), vs_ptr->comm_ptr, vs_peripheral_index, on_off);
    }
    return err_flag;
}

pm_err_flag_type 
pm_vs_pull_down_status(uint8 pmic_chip, uint8 vs_peripheral_index, pm_on_off_type* on_off) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_vs_data_type *vs_ptr = pm_vs_get_data(pmic_chip);

    if (vs_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_pull_down_status_alg(&(vs_ptr->pm_pwr_data), vs_ptr->comm_ptr, vs_peripheral_index, on_off);
    }
    return err_flag;
}





pm_err_flag_type 
pm_vs_vreg_ready_status(uint8 pmic_chip, uint8 vs_peripheral_index, boolean *vreg_ready)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_vs_data_type *vs_ptr = pm_vs_get_data(pmic_chip);

    if (vs_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_is_vreg_ready_alg(&(vs_ptr->pm_pwr_data), vs_ptr->comm_ptr, vs_peripheral_index, vreg_ready);
    }

    return err_flag;
}

