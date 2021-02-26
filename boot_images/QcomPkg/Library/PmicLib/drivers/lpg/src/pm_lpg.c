/*! \file
*  
*  \brief  pm_lpg.c ---LPG Driver implementation.
*  \details LPG Driver implementation.
*  &copy; Copyright (c) 2009-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.



when       	   who     what, where, why
--------   	   ---    ---------------------------------------------------------- 
03/29/16        al    Updating for newer target
03/16/15        al    Write PWM_OUTPUT Buffer if not LPG lite 
12/08/14        al    Added API to set LPG DTEST 
08/20/14        al    Updating comm lib  
08/12/14        al    Mask and data fixes
05/20/14        al    Architecture update 
05/12/13        al    Modified set interval counter api to configure pause time 
01/14/13        al    Adding get_status and removing 'dev' from api
10/16/12        al    added LUT support 
07/23/12        al    Created.

===========================================================================*/

/*===========================================================================

INCLUDE FILES 

===========================================================================*/

#include "pm_lpg.h"
#include "pm_lpg_driver.h"

#define PERPH_SUBTYPE_LPG_LITE  0x11


pm_err_flag_type pm_lpg_pwm_enable(uint32 pmic_chip, pm_lpg_chan_type pm_lpg_chan, boolean enable)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_data_type data = 0x00;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);


    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if ((pm_lpg_chan >= PM_LPG_CHAN_INVALID) || (pm_lpg_chan == PM_LPG_CHAN_NONE) || (pm_lpg_chan > lpg_ptr->num_of_peripherals))
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type pwm_sync = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->pwm_sync;
        pm_register_address_type enable_control = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->enable_control;
        data = (enable) ? 0x80 : 0x00;
        err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, enable_control, 0x80, (pm_register_data_type)data, 0);
        err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, pwm_sync, 0x1, 0x1, 0);
    }

    return err_flag;
};

pm_err_flag_type pm_lpg_pwm_output_enable(uint32 pmic_chip, pm_lpg_chan_type pm_lpg_chan, boolean enable)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    pm_register_data_type data = 0x00;

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if ((pm_lpg_chan >= PM_LPG_CHAN_INVALID) || (pm_lpg_chan == PM_LPG_CHAN_NONE) || (pm_lpg_chan > lpg_ptr->num_of_peripherals))
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type perph_subtype = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->perph_subtype;
        pm_register_address_type en_pwm_output = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->enable_control;
        /*if not LPG lite then only write to PWM OUTPUT buffer*/
        err_flag = pm_comm_read_byte(lpg_ptr->comm_ptr->slave_id, perph_subtype, &data,0);
        if (data != PERPH_SUBTYPE_LPG_LITE)
        {
            err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, en_pwm_output, 0x20, (pm_register_data_type)(enable << 5), 0);
        }
    }

    return err_flag;
};

pm_err_flag_type pm_lpg_pwm_set_pwm_value(uint32 pmic_chip, pm_lpg_chan_type pm_lpg_chan, uint16 pwm_value)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if ((pm_lpg_chan >= PM_LPG_CHAN_INVALID) || (pm_lpg_chan == PM_LPG_CHAN_NONE) || (pm_lpg_chan > lpg_ptr->num_of_peripherals))
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        if (pwm_value > 0x1FF)
        {
            pwm_value = 0x1FF;
        }
        pm_register_address_type pwm_value_lsb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->pwm_value_lsb;
        pm_register_address_type pwm_value_msb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->pwm_value_msb;
        pm_register_address_type pwm_sync = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->pwm_sync;
        
        err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, pwm_value_lsb, 0xFF, (pm_register_data_type)pwm_value, 0);
        err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, pwm_value_msb, 0x01, (pm_register_data_type)(pwm_value >> 8), 0);
        /*write to sync register to update PWM value */
	err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, pwm_sync, 0x1, 0x1, 0);
    }

    return err_flag;
};

pm_err_flag_type pm_lpg_pwm_set_pre_divide(uint32 pmic_chip, pm_lpg_chan_type pm_lpg_chan, pm_lpg_pwm_pre_divide_type pre_div, pm_lpg_pwm_freq_expo_type exp)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if ((pm_lpg_chan >= PM_LPG_CHAN_INVALID) || (pm_lpg_chan == PM_LPG_CHAN_NONE) || (pm_lpg_chan > lpg_ptr->num_of_peripherals))
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type lpg_pwm_freq_prediv_clk = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->lpg_pwm_freq_prediv_clk;
        err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, lpg_pwm_freq_prediv_clk, 0x67, (pm_register_data_type)((pre_div << 5) | exp), 0);
    }

    return err_flag;
}

pm_err_flag_type pm_lpg_pwm_clock_sel(uint32 pmic_chip, pm_lpg_chan_type pm_lpg_chan, pm_lpg_pwm_clock_type clock)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if ((pm_lpg_chan >= PM_LPG_CHAN_INVALID) || (pm_lpg_chan == PM_LPG_CHAN_NONE) || (pm_lpg_chan > lpg_ptr->num_of_peripherals))
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else if (clock >= PM_LPG_PWM_CLOCK_MAX)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type pm_freq_clk_select = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->lpg_pwm_size_clk;
        err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, pm_freq_clk_select, 0x03, (pm_register_data_type)clock, 0);
    }

    return err_flag;
};

pm_err_flag_type pm_lpg_set_pwm_bit_size(uint32 pmic_chip, pm_lpg_chan_type pm_lpg_chan, pm_lpg_pwm_size_type bit_size)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    pm_register_data_type data=0;

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if ((pm_lpg_chan >= PM_LPG_CHAN_INVALID) || (pm_lpg_chan == PM_LPG_CHAN_NONE) || (pm_lpg_chan > lpg_ptr->num_of_peripherals))
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else if (bit_size >= PM_LPG_PWM_SIZE_MAX)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type pwm_size = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->lpg_pwm_size_clk;
        pm_register_address_type pwm_sync = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->pwm_sync;

        /*LPG lite supports 6 and 9 bit PWM and reg bit position is 4. LPG legacy supports 6,7,8 and 9 bit PWM and reg bit position is 4,5*/
        data = (bit_size==PM_LPG_PWM_6BIT) ? 0x00 : 0xFF;
        err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, pwm_size, 0x10, data, 0);
        err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, pwm_sync, 0x1, 0x1, 0);
    }

    return err_flag;
};

pm_err_flag_type pm_lpg_pwm_src_select(uint32 pmic_chip, pm_lpg_chan_type pm_lpg_chan, pm_lpg_src_type lpg_src_type)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if ((pm_lpg_chan >= PM_LPG_CHAN_INVALID) || (pm_lpg_chan == PM_LPG_CHAN_NONE) || (pm_lpg_chan > lpg_ptr->num_of_peripherals))
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else if (lpg_src_type >= PM_LPG_PWM_SRC_INVALID)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type pwm_src_select = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->enable_control;
        err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, pwm_src_select, 0x04, ((pm_register_data_type)lpg_src_type) << 2, 0);
    }

    return err_flag;
};


pm_err_flag_type pm_lpg_config_pwm_type(uint32 pmic_chip, pm_lpg_chan_type pm_lpg_chan, boolean glitch_removal)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    pm_register_data_type data;

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if ((pm_lpg_chan >= PM_LPG_CHAN_INVALID) || (pm_lpg_chan == PM_LPG_CHAN_NONE) || (pm_lpg_chan > lpg_ptr->num_of_peripherals))
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type lpg_pwm_type_config = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->lpg_pwm_type_config;
        data = (glitch_removal) ? 0xFF : 0x00;
        err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, lpg_pwm_type_config, 0x20, data, 0);
    }

    return err_flag;

}; 


pm_err_flag_type pm_lpg_pattern_config(uint32 pmic_chip, pm_lpg_chan_type pm_lpg_chan, boolean ramp_direction, boolean pattern_repeat, boolean ramp_toggle, boolean en_pause_hi, boolean en_pause_lo)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    pm_register_data_type data;

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if ((pm_lpg_chan >= PM_LPG_CHAN_INVALID) || (pm_lpg_chan == PM_LPG_CHAN_NONE) || (pm_lpg_chan > lpg_ptr->num_of_peripherals))
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type lpg_pattern_config = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->lpg_pattern_config;
        data = (ramp_direction << 4) | (pattern_repeat << 3) | (ramp_toggle << 2) | (en_pause_hi << 1) | en_pause_lo;
        err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, lpg_pattern_config, 0x1F, data, 0);
    }
    return err_flag;
}


pm_err_flag_type pm_lpg_lut_config_set(uint32 pmic_chip, uint32 lut_index, uint32 pwm_value)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type periph_index = (pm_register_address_type)lut_index;

    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (lut_index >= lpg_ptr->lpg_specific->num_of_lut)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type lut_lsb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)((periph_index * 2) + lpg_ptr->lpg_reg_table->lut_lsb);
        pm_register_address_type lut_msb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)((periph_index * 2) + lpg_ptr->lpg_reg_table->lut_msb);
        err_flag  = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, lut_lsb, 0xFF, (pm_register_data_type)pwm_value, 0);
        err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, lut_msb, 0x01, (pm_register_data_type)(pwm_value >> 8), 0);
    }
    return err_flag;
}

pm_err_flag_type pm_lpg_lut_config_get(uint32 pmic_chip, uint32 lut_index, uint32 *pwm_value)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type periph_index = lut_index;
    pm_register_data_type pwm_value_lsb, pwm_value_msb;

    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (lut_index >= (lpg_ptr)->lpg_specific->num_of_lut)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type lut_lsb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)((periph_index * 2) + lpg_ptr->lpg_reg_table->lut_lsb);
        pm_register_address_type lut_msb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)((periph_index * 2) + lpg_ptr->lpg_reg_table->lut_msb);

        err_flag = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, lut_lsb, 0xFF, &pwm_value_lsb, 0);
        err_flag |= pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, lut_msb, 0x01, &pwm_value_msb, 0);

        *pwm_value = (pwm_value_msb << 8) | pwm_value_lsb;
    }
    return err_flag;
}

pm_err_flag_type pm_lpg_lut_config_set_array(uint32 pmic_chip, uint32 start_index, uint32 *value, uint32 count)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    uint32 end_index = start_index + count - 1;
    uint32 i_count = 0;
    pm_register_address_type lut_lsb, lut_msb;

    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (start_index >= (lpg_ptr)->lpg_specific->num_of_lut)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else if(end_index >= (lpg_ptr)->lpg_specific->num_of_lut)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        while (end_index >= start_index)
        {
            /*if value is greater than 9bits then round it to 9bits max value*/
            *(value + i_count) = (*(value + i_count) > 0x1FF) ? 0x1FF : (*(value + i_count));
            lut_lsb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)((start_index * 2) + lpg_ptr->lpg_reg_table->lut_lsb);
            lut_msb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)((start_index * 2) + lpg_ptr->lpg_reg_table->lut_msb);
            err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, lut_lsb, 0xFF, (pm_register_data_type)(*(value + i_count)), 0);
            err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, lut_msb, 0x01, (pm_register_data_type)((*(value + i_count)) >> 8), 0);
            start_index++;
            i_count++;
        }

    }
    return err_flag;
}

pm_err_flag_type pm_lpg_lut_config_get_array(uint32 pmic_chip, uint32 start_index, uint32 *value, uint32 count)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    uint32 end_index = start_index + count - 1;
    uint32 i_count = 0;
    pm_register_address_type lut_lsb, lut_msb;
    pm_register_data_type pwm_value_lsb, pwm_value_msb;

    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (start_index >= (lpg_ptr)->lpg_specific->num_of_lut)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else if (end_index >= (lpg_ptr)->lpg_specific->num_of_lut)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        while (end_index >= start_index)
        {
            lut_lsb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)((start_index * 2) + lpg_ptr->lpg_reg_table->lut_lsb);
            lut_msb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)((start_index * 2) + lpg_ptr->lpg_reg_table->lut_msb);
            err_flag |= pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, lut_lsb, 0xFF, &pwm_value_lsb, 0);
            err_flag |= pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, lut_msb, 0x01, &pwm_value_msb, 0);
            *(value + i_count) = (pwm_value_msb << 8) | pwm_value_lsb;
            start_index++;
            i_count++;
        }
    }
    return err_flag;
}


pm_err_flag_type pm_lpg_pwm_ramp_generator_start(uint32 pmic_chip, pm_lpg_chan_type pm_lpg_chan)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if ((pm_lpg_chan >= PM_LPG_CHAN_INVALID) || (pm_lpg_chan == PM_LPG_CHAN_NONE) || (pm_lpg_chan > lpg_ptr->num_of_peripherals))
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type ramp_control = lpg_ptr->lpg_reg_table->base_address + lpg_ptr->lpg_reg_table->ramp_control; //LUT base address + ramp_control
        /*
         * The ramp control fields are zero-indexed.
         */
        pm_lpg_chan--;

        err_flag = pm_comm_write_byte(lpg_ptr->comm_ptr->slave_id, ramp_control, (pm_register_data_type)(1 << pm_lpg_chan), 0);
    }
    return err_flag;
}



pm_err_flag_type pm_lpg_pwm_lut_index_set(uint32 pmic_chip, pm_lpg_chan_type pm_lpg_chan, uint32 low_index, uint32 high_index)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;

    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if ((pm_lpg_chan >= PM_LPG_CHAN_INVALID) || (pm_lpg_chan == PM_LPG_CHAN_NONE) || (pm_lpg_chan > lpg_ptr->num_of_peripherals))
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else if (low_index > 0x03F)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else if (high_index > 0x03F)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type hi_index = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->hi_index;
        pm_register_address_type lo_index = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->lo_index;

        err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, hi_index, 0x3F, (pm_register_data_type)high_index, 0);
        err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, lo_index, 0x3F, (pm_register_data_type)low_index, 0);
    }
    return err_flag;
}


pm_err_flag_type pm_lpg_config_pause_time(uint32 pmic_chip, pm_lpg_chan_type pm_lpg_chan, uint32 ramp_step_duration, uint32 hi_multiplier, uint32 low_multiplier)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if ((pm_lpg_chan >= PM_LPG_CHAN_INVALID) || (pm_lpg_chan == PM_LPG_CHAN_NONE) || (pm_lpg_chan > lpg_ptr->num_of_peripherals))
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type ramp_step_duration_msb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->ramp_step_duration_msb;
        pm_register_address_type ramp_step_duration_lsb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->ramp_step_duration_lsb;

        pm_register_address_type pause_hi_multiplier_lsb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->pause_hi_multiplier_lsb;
        //pm_register_address_type pause_hi_multiplier_msb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->pause_hi_multiplier_msb;

        pm_register_address_type pause_lo_multiplier_lsb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->pause_lo_multiplier_lsb;
        //pm_register_address_type pause_lo_multiplier_msb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->pause_lo_multiplier_msb;

        ramp_step_duration = (ramp_step_duration > 0x1FF) ? 0x1FF : ramp_step_duration;

        hi_multiplier = (hi_multiplier > 0xFF) ? 0xFF : hi_multiplier;

        low_multiplier = (low_multiplier > 0xFF) ? 0xFF : low_multiplier;

        err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, ramp_step_duration_lsb, 0xFF, (pm_register_data_type)(ramp_step_duration & 0xFF), 0);
        err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, ramp_step_duration_msb, 0x01, (pm_register_data_type)(ramp_step_duration >> 8), 0);

        err_flag |= pm_comm_write_byte(lpg_ptr->comm_ptr->slave_id, pause_hi_multiplier_lsb, (pm_register_data_type)(hi_multiplier & 0xFF), 0);
        //err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, pause_hi_multiplier_msb, 0x0F, (pm_register_data_type)(hi_multiplier >> 8), 0); //not supported

        err_flag |= pm_comm_write_byte(lpg_ptr->comm_ptr->slave_id, pause_lo_multiplier_lsb, (pm_register_data_type)(low_multiplier & 0xFF), 0);
        //err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, pause_lo_multiplier_msb, 0x0F, (pm_register_data_type)(low_multiplier >> 8), 0); //not supported
    }
    return err_flag;
}


pm_err_flag_type pm_lpg_pwm_ramp_generator_enable(uint32 pmic_chip, pm_lpg_chan_type pm_lpg_chan, boolean enable)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    pm_register_data_type data;

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if ((pm_lpg_chan >= PM_LPG_CHAN_INVALID) || (pm_lpg_chan == PM_LPG_CHAN_NONE) || (pm_lpg_chan > lpg_ptr->num_of_peripherals))
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type en_ramp_gen = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->enable_control;
        data = (enable) ? 0xFF : 0x00;
        err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, en_ramp_gen, 0x02, data, 0);
    }
    return err_flag;
}


pm_err_flag_type pm_lpg_set_lpg_dtest(uint32 pmic_chip, pm_lpg_chan_type pm_lpg_chan, pm_lpg_chan_dtest_type dtest_type, pm_lpg_chan_lpg_out_type lpg_out)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if ((pm_lpg_chan >= PM_LPG_CHAN_INVALID) || (pm_lpg_chan == PM_LPG_CHAN_NONE) || (pm_lpg_chan > lpg_ptr->num_of_peripherals) || (lpg_out >= PM_LPG_CHAN_LPG_OUT_INVALID))
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else if ((lpg_out >= PM_LPG_CHAN_LPG_OUT_INVALID) || ((dtest_type > PM_LPG_CHAN_DTEST_1) && (lpg_out > PM_LPG_CHAN_LPG_OUT_2)))
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type test_reg = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->test1 + (pm_register_address_type)dtest_type;
	pm_register_address_type sec_access = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->sec_access;
        err_flag = pm_comm_write_byte(lpg_ptr->comm_ptr->slave_id, sec_access, 0xA5, 0);
	err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, test_reg, 0x7, (pm_register_data_type)lpg_out, 0);
    }
    return err_flag;
}

pm_err_flag_type pm_lpg_get_status(uint32 pmic_chip, pm_lpg_chan_type pm_lpg_chan, pm_lpg_status_type *lpg_status)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_register_data_type data, data1;
    pm_register_address_type pwm_value_lsb, pwm_value_msb, lpg_pwm_freq_prediv_clk, enable_control, lpg_pwm_size_clk;
    pm_register_address_type lpg_pwm_type_config, lpg_pattern_config, hi_index, lo_index, ramp_step_duration_msb, ramp_step_duration_lsb;

    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if ((pm_lpg_chan >= PM_LPG_CHAN_INVALID) || (pm_lpg_chan == PM_LPG_CHAN_NONE) || (pm_lpg_chan > lpg_ptr->num_of_peripherals))
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    //boolean pwm_output;
    //pm_lpg_src_type lpg_src_type;
    enable_control = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->enable_control;
    err_flag = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, enable_control, 0xFF, &data, 0);
    if (err_flag)
    {
        return err_flag;  //PM_ERR_FLAG_BUS_ERR
    }
    lpg_status->pwm_output = (data & 0x80) ? TRUE : FALSE;
    lpg_status->lpg_src_type = (data & 0x04) ? PM_LPG_PWM_SRC_PWM_REGISTER : PM_LPG_PWM_SRC_LUT;

    //uint16 pwm_value;
    pwm_value_lsb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->pwm_value_lsb;
    pwm_value_msb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->pwm_value_msb;

    err_flag = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, pwm_value_msb, 0x01, &data1, 0);
    if (err_flag)
    {
        return err_flag;
    }
    err_flag = pm_comm_read_byte(lpg_ptr->comm_ptr->slave_id, pwm_value_lsb, &data, 0);
    if (err_flag)
    {
        return err_flag;
    }
    lpg_status->pwm_value = (data1 << 8) | data;
    //pm_lpg_pwm_pre_divide_type pre_div;
    //pm_lpg_pwm_freq_expo_type exp;
    lpg_pwm_freq_prediv_clk = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->lpg_pwm_freq_prediv_clk;
    err_flag = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, lpg_pwm_freq_prediv_clk, 0xFF, &data, 0);
    if (err_flag)
    {
        return err_flag;
    }
    lpg_status->exp = (pm_lpg_pwm_freq_expo_type)(data & 0x07);
    lpg_status->pre_div = (pm_lpg_pwm_pre_divide_type)((data & 0x60) >> 5);
    //pm_lpg_pwm_clock_type clock;
    //pm_lpg_pwm_size_type bit_size;
    lpg_pwm_size_clk = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->lpg_pwm_size_clk;
    err_flag = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, lpg_pwm_size_clk, 0xFF, &data, 0);
    if (err_flag)
    {
        return err_flag;
    }
    lpg_status->pwm_freq_clk = (pm_lpg_pwm_clock_type)(data & 0x03);
    lpg_status->pwm_bit_size = (data & 0x10)? PM_LPG_PWM_9BIT : PM_LPG_PWM_6BIT; 

    //boolean glitch_removal;
    //boolean full_scale;
    //boolean phase_stagger;
    //pm_lpg_phase_stag_shift_type phase_shift;
    lpg_pwm_type_config = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->lpg_pwm_type_config;
    err_flag = pm_comm_read_byte(lpg_ptr->comm_ptr->slave_id, lpg_pwm_type_config, &data, 0);
    if (err_flag)
    {
        return err_flag;
    }
    lpg_status->glitch_removal = (data & 0x20) ? TRUE : FALSE;
    lpg_status->full_scale = (data & 0x08) ? TRUE : FALSE;
    lpg_status->en_phase_stagger = (data & 0x04) ? TRUE : FALSE;
    lpg_status->phase_stagger = (pm_lpg_phase_stag_shift_type)(data & 0x03);

    //boolean ramp_direction;
    //boolean pattern_repeat;
    //boolean ramp_toggle;
    //boolean en_pause_hi;
    //boolean en_pause_lo;
    lpg_pattern_config = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->lpg_pattern_config;
    err_flag = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, lpg_pattern_config, 0xFF, &data, 0);
    if (err_flag)
    {
        return err_flag;
    }
    lpg_status->ramp_direction = (data & 0x10) ? TRUE : FALSE;
    lpg_status->pattern_repeat = (data & 0x08) ? TRUE : FALSE;
    lpg_status->ramp_toggle = (data & 0x04) ? TRUE : FALSE;
    lpg_status->en_pause_hi = (data & 0x02) ? TRUE : FALSE;
    lpg_status->en_pause_lo = (data & 0x01) ? TRUE : FALSE;
    //uint32 lut_low_index;
    //uint32 lut_high_index;
    hi_index = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->hi_index;
    lo_index = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->lo_index;
    err_flag = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, hi_index, 0x3F, &data, 0);
    if (err_flag)
    {
        return err_flag;
    }
    lpg_status->lut_high_index = data & 0x3F;

    err_flag = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, lo_index, 0x3F, &data, 0);
    if (err_flag)
    {
        return err_flag;
    }
    lpg_status->lut_low_index = data & 0x3F;

    //uint32 interval_count;
    ramp_step_duration_msb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->ramp_step_duration_msb;
    ramp_step_duration_lsb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->ramp_step_duration_lsb;
    err_flag = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, ramp_step_duration_msb, 0x01, &data1, 0);
    if (err_flag)
    {
        return err_flag;
    }
    err_flag = pm_comm_read_byte(lpg_ptr->comm_ptr->slave_id, ramp_step_duration_lsb, &data, 0);
    if (err_flag)
    {
        return err_flag;
    }
    lpg_status->ramp_step_duration = (data1 << 8) | data;

    pm_register_address_type pause_hi_multiplier_lsb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->pause_hi_multiplier_lsb;
    pm_register_address_type pause_lo_multiplier_lsb = lpg_ptr->lpg_reg_table->base_address + (pm_register_address_type)(periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->pause_lo_multiplier_lsb;

    err_flag |= pm_comm_read_byte(lpg_ptr->comm_ptr->slave_id, pause_hi_multiplier_lsb, &data, 0);
    lpg_status->hi_multiplier = data;
    err_flag |= pm_comm_read_byte(lpg_ptr->comm_ptr->slave_id, pause_lo_multiplier_lsb, &data, 0);
    lpg_status->low_multiplier = data;
    
    return err_flag;

}
