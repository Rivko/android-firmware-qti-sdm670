/*! \file pm_clk.c
 *  \n 
 *  \brief Implementation file for CLOCK public APIs. 
 *  \n  
 *  \n &copy; Copyright 2013-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/2014 akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture 
09/25/13   kt      Updated clk module driver.
02/01/13   hw      Rearchitecting clk module driver to peripheral driver
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_clk.h"
#include "pm_clk_driver.h"
#include "pm_comm.h"

/*===========================================================================

                     API IMPLEMENTATION 

===========================================================================*/

pm_err_flag_type pm_clk_drv_strength(uint32 pmic_chip, pm_clk_type periph, pm_clk_drv_strength_type drive_strength)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    uint32 periph_index = 0;

    if ((clk_ptr == NULL) || (periph >= PM_ALL_CLKS) || 
        (drive_strength >= PM_CLK_DRV_STRENGTH_INVALID))
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    periph_index = clk_ptr->clk_perph_index[periph];

    if((periph_index == PM_CLK_INVALID_DATA) || (periph_index >= PM_MAX_NUM_CLKS) ||
       (clk_ptr->periph_subtype[periph_index] == 0))
    {
        return PM_ERR_FLAG_INVALID_PERIPHERAL;
    }

    if (clk_ptr->periph_subtype[periph_index] == PM_HW_MODULE_CLOCK_XO_CORE)
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset * periph_index) +
       clk_ptr->clk_reg_table->drv_ctl1;

    /* Set strength */
    err_flag = pm_comm_write_byte_mask(clk_ptr->comm_ptr->slave_id, reg, 0x03, (pm_register_data_type)(drive_strength), 0);  
    
    /* Let the user know if we were successful or not */
    return err_flag;
}

pm_err_flag_type pm_clk_sw_enable(uint32 pmic_chip, pm_clk_type periph, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_data_type data = 0;
    uint32 periph_index = 0;

    if ((clk_ptr == NULL) || (periph >= PM_ALL_CLKS))
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    periph_index = clk_ptr->clk_perph_index[periph];

    if((periph_index == PM_CLK_INVALID_DATA) || (periph_index >= PM_MAX_NUM_CLKS) ||
       (clk_ptr->periph_subtype[periph_index] == 0))
    {
        return PM_ERR_FLAG_INVALID_PERIPHERAL;
    }

    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset * periph_index) +
       clk_ptr->clk_reg_table->en_ctl;

    if (PM_ON == on_off)
    {
        data = 0xFF;
    }
    else
    {
        data = 0x00;
    }

    err_flag =  pm_comm_write_byte_mask(clk_ptr->comm_ptr->slave_id, reg , 0x80, data, 0); // 7

    return err_flag;
}

pm_err_flag_type pm_clk_pin_ctrled(uint32 pmic_chip, pm_clk_type periph, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_data_type data = 0;
    uint32 periph_index = 0;

    if ((clk_ptr == NULL) || (periph >= PM_ALL_CLKS))
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    periph_index = clk_ptr->clk_perph_index[periph];

    if((periph_index == PM_CLK_INVALID_DATA) || (periph_index >= PM_MAX_NUM_CLKS) ||
       (clk_ptr->periph_subtype[periph_index] == 0))
    {
        return PM_ERR_FLAG_INVALID_PERIPHERAL;
    }

    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset * periph_index) +
       clk_ptr->clk_reg_table->en_ctl;

    if (PM_ON == on_off)
    {
        data = 0xFF;
    }
    else
    {
        data = 0x00;
    }

    err_flag = pm_comm_write_byte_mask(clk_ptr->comm_ptr->slave_id, reg , 0x01, data, 0); // 0

    return err_flag;
}



pm_err_flag_type pm_clk_pull_down(uint32 pmic_chip, pm_clk_type periph, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_data_type    data = 0;
    uint8 periph_index = 0;

    if ((clk_ptr == NULL) || (periph >= PM_ALL_CLKS))
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
        }

    periph_index = clk_ptr->clk_perph_index[periph];

    if((periph_index == PM_CLK_INVALID_DATA) || (periph_index >= PM_MAX_NUM_CLKS) ||
       (clk_ptr->periph_subtype[periph_index] == 0))
    {
        return PM_ERR_FLAG_INVALID_PERIPHERAL;
    }

    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset * periph_index) +
       clk_ptr->clk_reg_table->pd_ctl;

    if (PM_ON == on_off)
    {
        data = 0xFF;
    }
    else
    {
        data = 0x00;
    }

    err_flag = pm_comm_write_byte_mask(clk_ptr->comm_ptr->slave_id, reg , 0x80, data, 0); // 7

    return err_flag;
}


pm_err_flag_type pm_clk_set_out_clk_div(uint32 pmic_chip, pm_clk_type clk, uint32 divider)
{
    pm_err_flag_type         err_flag   = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type reg;
    pm_register_data_type    data = 0;
    uint8 mask = 0x07;
    
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);

    if ((clk_ptr == NULL) || (clk >= PM_ALL_CLKS))
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    uint8 periph_index = clk_ptr->clk_perph_index[clk];

    if (clk_ptr->periph_subtype[periph_index] != PM_HW_MODULE_CLOCK_DIV_CLK)
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    /*data = log2divider*/
    while (divider && (data < 7))
    {
        divider >>= 1;
        data++;
    }

    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset * periph_index) +
       clk_ptr->clk_reg_table->div_ctl1;

    err_flag = pm_comm_write_byte_mask(clk_ptr->comm_ptr->slave_id, reg, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_clk_get_status(uint32 pmic_chip, pm_clk_type clk, pm_clk_status_type *clk_status)
{
    pm_err_flag_type         err_flag   = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type reg;
    pm_register_data_type    data = 0;
    
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);

    if ((clk_ptr == NULL) || (clk >= PM_ALL_CLKS))
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    uint8 periph_index = clk_ptr->clk_perph_index[clk];

    if (clk_ptr->periph_subtype[periph_index] != PM_HW_MODULE_CLOCK_XO_CORE)
    {
        reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset * periph_index) +
           clk_ptr->clk_reg_table->drv_ctl1;
        /* Set strength */
        err_flag = pm_comm_read_byte_mask(clk_ptr->comm_ptr->slave_id, reg, 0x03, &data, 0);
        clk_status->clk_drv_strength = (pm_clk_drv_strength_type)data;
    }

    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset * periph_index) +
       clk_ptr->clk_reg_table->status1;

    err_flag |= pm_comm_read_byte(clk_ptr->comm_ptr->slave_id, reg, &data, 0);
    clk_status->clk_ok = (data & 0x80) ? PM_ON : PM_OFF;

    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset * periph_index) +
       clk_ptr->clk_reg_table->en_ctl;

    err_flag |= pm_comm_read_byte(clk_ptr->comm_ptr->slave_id, reg, &data, 0);

    clk_status->clk_sw_enable = (data & 0x80) ? PM_ON : PM_OFF;

    clk_status->clk_pin_ctrled = (data & 0x01) ? PM_ON : PM_OFF;

    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset * periph_index) +
       clk_ptr->clk_reg_table->pd_ctl;

    err_flag = pm_comm_read_byte(clk_ptr->comm_ptr->slave_id, reg, &data, 0); // 
    clk_status->clk_pull_down = (data & 0x80) ? PM_ON : PM_OFF;

    if (clk_ptr->periph_subtype[periph_index] == PM_HW_MODULE_CLOCK_DIV_CLK)
    {
        reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset * periph_index) +
           clk_ptr->clk_reg_table->div_ctl1;
        err_flag = pm_comm_read_byte(clk_ptr->comm_ptr->slave_id, reg, &data, 0);
        clk_status->clk_out_div = data & 0x7;
    }

    return err_flag;
}
