/*! \file pm_clk_sleep.c
*  \n
*  \brief Implementation file for CLK SLEEP resource type.
*  \n  
*  &copy; Copyright 2013-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
02/01/13   hw      Rearchitecting clk module driver to peripheral driver
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_clk_sleep.h"
#include "pm_clk_driver.h"
#include "pm_comm.h"

/*===========================================================================

                     ACCESS LAYER

===========================================================================*/


pm_err_flag_type pm_clk_sleep_smpl(uint8 pmic_chip, pm_clk_type periph, pm_on_off_type on_off, pm_clk_sleep_smpl_delay_type delay)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_data_type data = 0;
    uint8 periph_index = 0;


    if ((clk_ptr == NULL) || (clk_ptr->comm_ptr == NULL) || (periph != PM_CLK_SLEEP))
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    periph_index = clk_ptr->clk_perph_index[periph];

    if((periph_index == PM_CLK_INVALID_DATA) || (periph_index >= PM_MAX_NUM_CLKS) ||
       (clk_ptr->periph_subtype[periph_index] == 0))
        {
        return PM_ERR_FLAG_INVALID_PERIPHERAL;
        }

    if (clk_ptr->periph_subtype[periph_index] != PM_HW_MODULE_CLOCK_SLP_CLK)
        {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
        }


    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset*periph_index);

    reg = reg + clk_ptr->clk_sleep_reg_table->smpl_ctrl1;

    if (PM_ON == on_off)
    {
        data = 0x80;
    }
    else
    {
        data = 0x00;
    }

    data = data | ((uint32)delay);

    err_flag = pm_comm_write_byte_mask(clk_ptr->comm_ptr->slave_id, reg, 0x83, data, 0);

    return err_flag;  
}

pm_err_flag_type pm_clk_sleep_cal_rc_enable(uint8 pmic_chip, pm_clk_type periph, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_data_type data = 0;
    uint8 periph_index = 0;


    if ((clk_ptr == NULL)|| (clk_ptr->comm_ptr == NULL) || (periph != PM_CLK_SLEEP))
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    periph_index = clk_ptr->clk_perph_index[periph];

    if((periph_index == PM_CLK_INVALID_DATA) || (periph_index >= PM_MAX_NUM_CLKS) ||
       (clk_ptr->periph_subtype[periph_index] == 0))
        {
        return PM_ERR_FLAG_INVALID_PERIPHERAL;
        }

    if (clk_ptr->periph_subtype[periph_index] != PM_HW_MODULE_CLOCK_SLP_CLK)
        {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
        }


    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset*periph_index);

    reg = reg + clk_ptr->clk_sleep_reg_table->cal_rc4;

    if (PM_ON == on_off)
    {
        data = 0xFF;
    }
    else
    {
        data = 0x00;
    }

    err_flag = pm_comm_write_byte_mask(clk_ptr->comm_ptr->slave_id, reg, 0x80, data, 0);


    return err_flag;  
}

pm_err_flag_type pm_clk_sleep_coincell_valid(uint8 pmic_chip, pm_clk_type periph, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_data_type data = 0;
    uint8 periph_index = 0;


    if ((clk_ptr == NULL) || (clk_ptr->comm_ptr == NULL) || (periph != PM_CLK_SLEEP))
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    periph_index = clk_ptr->clk_perph_index[periph];

    if((periph_index == PM_CLK_INVALID_DATA) || (periph_index >= PM_MAX_NUM_CLKS) ||
       (clk_ptr->periph_subtype[periph_index] == 0))
        {
        return PM_ERR_FLAG_INVALID_PERIPHERAL;
        }

    if (clk_ptr->periph_subtype[periph_index] != PM_HW_MODULE_CLOCK_SLP_CLK)
        {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
        }

    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset*periph_index);

    reg = reg + clk_ptr->clk_sleep_reg_table->cal_rc4;

    if (PM_ON == on_off)
    {
        data = 0xFF;
    }
    else
    {
        data = 0x00;
    }

    err_flag = pm_comm_write_byte_mask(clk_ptr->comm_ptr->slave_id, reg, 0x40, data, 0);


    return err_flag;  
}

