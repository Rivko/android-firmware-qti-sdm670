/*! \file pm_pwr_alg.c 
*  \n
*  \brief  
*  \n  
*  \n &copy; Copyright 2012-2016 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/drivers/pwr/src/pm_pwr_alg.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/16   aab     Added pm_pwr_ocp_latched_status_clear_alg()
11/03/15   al      Updated rails API
07/22/14   aks     Adding API to round off voltage
12/06/12   hw      Rearchitecturing module driver to peripheral driver
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pwr_alg.h"
#include "pm_comm.h"

#define SMART_BOOST_OFFSET             0xE6 //store this offset here instead of config since this bit is going to be erased in new HW
#define STEPPER_OK                     0x81
#define VREG_OK                        0x80
#define STATUS2_OFFSET                 0x009
#define OCP_CTL2_OFFSET                0x04B
#define OCP_GLOBAL_BROADCAST_EN_MASK   0x04
#define SEC_ACCESS_WRITE_DATA          0xA5

/*===========================================================================

                     FUNCTION IMPLEMENTATION 

===========================================================================*/

pm_err_flag_type pm_pwr_pull_down_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    //uint8 periph_type = 0;
    
    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0))
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }
    else if (on_off >= PM_INVALID)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type          reg = 0xFF;
        pm_register_data_type             data = 0xFF;

       // periph_type = pwr_data->pwr_specific_info[peripheral_index].periph_type;
    
        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->PD_CTL);
    
        data = (PM_OFF == on_off) ? 0 : 0x80;

        err_flag = pm_comm_write_byte_mask(comm_ptr->slave_id, reg, 0x80, data, 0);
    }

    if (PM_ERR_FLAG_SUCCESS != err_flag)
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_PWR_PULL_DOWN_ERROR, comm_ptr->pmic_index, periph_type, peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_pull_down_status_alg(pm_pwr_data_type *pwr_data,  pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

    if ((pwr_data == NULL) || (comm_ptr == NULL) || (on_off == NULL))
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    if (peripheral_index >= pwr_data->num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type           reg = 0x0;
        pm_register_data_type              data = 0;


        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->PD_CTL);

        err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &data, 0);
        if (err_flag != PM_ERR_FLAG_SUCCESS)
        {
            *on_off = PM_INVALID;
            return err_flag;
        }

        *on_off = (data & 0x80) ? PM_ON : PM_OFF;
    }

    return err_flag;
}


pm_err_flag_type pm_pwr_sw_mode_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_sw_mode_type *sw_mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    uint8 periph_type = 0;
                                                          
    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0))
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }
    else if (sw_mode == NULL)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type           reg = 0x0;
        pm_register_data_type              data = 0xFF;

        periph_type = pwr_data->pwr_specific_info[peripheral_index].periph_type;
            
        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->MODE_CTL);

        err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &data, 0);
        
        if (err_flag == PM_ERR_FLAG_SUCCESS)
        {
            if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg == TRUE)
            {
                if (data == 7)
                {
                    *sw_mode = PM_SW_MODE_NPM;
                }
                else if ((data == 6) && (periph_type != PM_HW_MODULE_LDO))
                {
                    *sw_mode = PM_SW_MODE_AUTO;
                }
                else if (data == 5)
                {
                    *sw_mode = PM_SW_MODE_LPM;
                }
                else if (data == 4)
                {
                    *sw_mode = PM_SW_MODE_RETENTION;
                }
                else if ((data <= 3) && (data >= 1) && (periph_type == PM_HW_MODULE_LDO))
                {
                    // applicable for LDOs only
                    *sw_mode = PM_SW_MODE_BYPASS;
                }
                else
                {
                    *sw_mode = PM_SW_MODE_INVALID;
                }
            }
            else
            {
                /* BYPASS supercedes NPM & PFM */
                /* Only valid for LDO's */
                if (data & 0x20)
                {
                    *sw_mode = PM_SW_MODE_BYPASS;
                }
                /* NPM supercedes AUTO & PFM */
                else if (data & 0x80)
                {
                    *sw_mode = PM_SW_MODE_NPM;
                }
                /* AUTO supercedes PFM */
                /* only valid for SMPS */
                else if (data & 0x40)
                {
                    *sw_mode = PM_SW_MODE_AUTO;
                }
                else /* IT MIGHT BE PFM DEPENDING ON pc_mode bits */
                {
                    *sw_mode = PM_SW_MODE_LPM;
                }
            }
        }
        else
        {
            *sw_mode = PM_SW_MODE_INVALID;
        }
    }

    if (PM_ERR_FLAG_SUCCESS != err_flag)
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_PWR_MODE_ERROR, comm_ptr->pmic_index, periph_type, peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_pin_ctrl_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, uint8 select_pin)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    //uint8 periph_type = 0;

    if ((peripheral_index < pwr_data->num_of_peripherals) && (pwr_data->pwr_specific_info[peripheral_index].periph_type != 0))
    {
        pm_register_address_type reg = 0x0;

        //periph_type = pwr_data->pwr_specific_info[peripheral_index].periph_type;

        if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg ==  TRUE)
        {
            reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->FOLLOW_HWEN);
        }
        else
        {
            reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->EN_CTL);
        }

        err_flag = pm_comm_write_byte_mask(comm_ptr->slave_id, reg, 0x0F, (pm_register_data_type)select_pin, 1); // bit<0:3>
    }
    else
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }

    if (PM_ERR_FLAG_SUCCESS == err_flag)
    {
        //PM_LOG_MSG_DEBUG(PMIC_DRV_PWR_PIN_CTRL, comm_ptr->pmic_index, periph_type, peripheral_index, select_pin);
    }
    else
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_PWR_PIN_CTRL_ERROR, comm_ptr->pmic_index, periph_type, peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_pin_ctrl_mode_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, uint8 select_pin)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    //uint8 periph_type = 0;
    uint8 mask = 0;

    if ((peripheral_index < pwr_data->num_of_peripherals) && (pwr_data->pwr_specific_info[peripheral_index].periph_type != 0))
    {
        pm_register_address_type reg = 0x0;

        //periph_type = pwr_data->pwr_specific_info[peripheral_index].periph_type;

        if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg ==  TRUE)
        {
            reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->FOLLOW_HWEN);
            mask = 0xF0;
            select_pin = (select_pin<<4);
        }
        else
        {
            reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->MODE_CTL);
            mask = 0x1F;
        }

        err_flag = pm_comm_write_byte_mask(comm_ptr->slave_id, reg, mask, (pm_register_data_type)select_pin, 0); // bit<0:4>
    }
    else
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }

    if (PM_ERR_FLAG_SUCCESS == err_flag)
    {
        //PM_LOG_MSG_DEBUG(PMIC_DRV_PWR_PIN_CTRL_MODE, comm_ptr->pmic_index, periph_type, peripheral_index, select_pin);
    }
    else
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_PWR_PIN_CTRL_ERROR, comm_ptr->pmic_index, periph_type, peripheral_index, err_flag);
    }

    return err_flag;
}


pm_err_flag_type pm_pwr_pin_ctrl_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

    if ((pwr_data == NULL) || (comm_ptr == NULL) || (on_off == NULL))
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    if (peripheral_index >= pwr_data->num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type           reg = 0x0;
        pm_register_data_type              data = 0;

        if (pwr_data->pwr_reg_table == NULL)
        {
            return PM_ERR_FLAG_INVALID_PARAMETER;
        }

        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->EN_CTL);

        err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &data, 0);
        if (err_flag != PM_ERR_FLAG_SUCCESS)
        {
            *on_off = PM_INVALID;
            return err_flag;
        }

        if (data & 0x0F)
        {
            *on_off = PM_ON;
        }
        else
        {
            *on_off = PM_OFF;
        }
    }

    return err_flag;
}


pm_err_flag_type pm_pwr_sw_en_pin_ctl_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, 
                                          pm_on_off_type on_off , uint8 select_pin)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    uint8 periph_type = 0;
    uint8 perph_revision = 0;
    uint8 sw_en = 0;
    uint8 pc_en = 0;

    if (peripheral_index < pwr_data->num_of_peripherals && 
              pwr_data->pwr_specific_info[peripheral_index].periph_type != 0)
    {
        pm_register_address_type sec_reg = 0;
        pm_register_address_type smart_boost_reg = 0;   
        pm_register_address_type pin_ctl_reg = 0;   
        pm_register_address_type en_ctl_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->EN_CTL);
        periph_type = pwr_data->pwr_specific_info[peripheral_index].periph_type;


        if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg ==  TRUE)
        {
            pin_ctl_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->FOLLOW_HWEN);

            if(on_off != 0)
            {
                sw_en = 0x80;
            }
            
            err_flag = pm_comm_write_byte(comm_ptr->slave_id, en_ctl_reg, sw_en, 1);

            err_flag |= pm_comm_write_byte_mask(comm_ptr->slave_id, pin_ctl_reg, 0x0F, (pm_register_data_type)select_pin, 1); // bit<0:3>
        }
        else if (periph_type == PM_HW_MODULE_BOOST)
        {
            smart_boost_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + 0x1);

            err_flag = pm_comm_read_byte(comm_ptr->slave_id, smart_boost_reg, &perph_revision, 1);

            if(perph_revision <=2)
            {
                sec_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->SEC_ACCESS);
                smart_boost_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + SMART_BOOST_OFFSET);

                //unlock the register
                err_flag = pm_comm_write_byte(comm_ptr->slave_id, sec_reg, 0xA5, 0);

                if(on_off != 0)
                {
                    sw_en = 0x80;
                }
                else if(select_pin != 0)
                {
                    sw_en = 0x80;
                    pc_en = 0x80;
                }
                err_flag = pm_comm_write_byte(comm_ptr->slave_id, smart_boost_reg, pc_en, 1); // bit<7>
                err_flag = pm_comm_write_byte(comm_ptr->slave_id, en_ctl_reg,  sw_en, 1); 
            } 
            else
            {
                if(on_off != 0)
                {
                    sw_en = 0x80;
                }

                err_flag = pm_comm_write_byte(comm_ptr->slave_id, en_ctl_reg,  ( sw_en | (0xF & select_pin)), 1); 
            }
        }   
        else
        {   
            if(on_off != 0)
            {
                sw_en = 0x80;
            }
            
            err_flag = pm_comm_write_byte(comm_ptr->slave_id, en_ctl_reg,  ( sw_en | (0xF & select_pin)), 1); 
        }
        
    }
    else
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }
    
    if (PM_ERR_FLAG_SUCCESS == err_flag)
    {
        //PM_LOG_MSG_DEBUG(PMIC_DRV_PWR_PIN_CTRL, comm_ptr->pmic_index, periph_type, peripheral_index, select_pin);
        //PM_LOG_MSG_DEBUG(PMIC_DRV_PWR_SW_EN, comm_ptr->pmic_index, periph_type, peripheral_index, on_off);
    }
    else
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_PWR_PIN_CTRL_ERROR, comm_ptr->pmic_index, periph_type, peripheral_index, err_flag);
        //PM_LOG_MSG_ERROR(PMIC_DRV_PWR_SW_EN_ERROR, comm_ptr->pmic_index, periph_type, peripheral_index, err_flag);
    }
    
    return err_flag;

}


pm_err_flag_type pm_pwr_sw_enable_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    //uint8 periph_type = 0;

    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0))
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }
    else if (on_off >= PM_INVALID)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type reg = 0x0;
        pm_register_data_type data = 0;

        //periph_type = pwr_data->pwr_specific_info[peripheral_index].periph_type;
            
        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->EN_CTL);

        if (PM_OFF == on_off)
        {
            data = 0;
        }
        else
        {
            data = 0x80;
        }

        if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg ==  TRUE)
        {
            err_flag = pm_comm_write_byte(comm_ptr->slave_id, reg, data, 0);
        }
        else
        {
            err_flag = pm_comm_write_byte_mask(comm_ptr->slave_id, reg, 0x80, data, 0);
        }
    }

    if (PM_ERR_FLAG_SUCCESS == err_flag)
    {
        //PM_LOG_MSG_DEBUG(PMIC_DRV_PWR_SW_EN, comm_ptr->pmic_index, periph_type, peripheral_index, on_off);
    }
    else
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_PWR_SW_EN_ERROR, comm_ptr->pmic_index, periph_type, peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_sw_enable_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    //uint8 periph_type = 0;

    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0))
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }
    else if (on_off == NULL)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type           reg = 0x0;
        pm_register_data_type              data = 0;

        //periph_type = pwr_data->pwr_specific_info[peripheral_index].periph_type;

        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->EN_CTL);

        err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &data, 0);
        if (err_flag != PM_ERR_FLAG_SUCCESS)
        {
            *on_off = PM_INVALID;
            return err_flag;
        }

        if (data & 0x80)
        {
            *on_off = PM_ON;
        }
        else
        {
            *on_off = PM_OFF;
        }
    }

    if (PM_ERR_FLAG_SUCCESS != err_flag)
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_PWR_SW_EN_ERROR, comm_ptr->pmic_index, periph_type, peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_is_vreg_ready_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, 
                                                 uint8 peripheral_index, boolean *vreg_ready)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    uint8 vreg_mask = 0;
    //uint8 periph_type = 0;

    if (peripheral_index >= pwr_data->num_of_peripherals || 
             pwr_data->pwr_specific_info[peripheral_index].periph_type == 0)
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }
    else if (vreg_ready == NULL)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type reg = 0x0;
        pm_register_data_type data = 0;
        //periph_type = pwr_data->pwr_specific_info[peripheral_index].periph_type;

        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->STATUS);

        err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &data, 0);
        if (err_flag != PM_ERR_FLAG_SUCCESS)
        {
            err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &data, 0);
            if (err_flag != PM_ERR_FLAG_SUCCESS)
            {
                *vreg_ready = FALSE;
                return err_flag;
            }
        }

        if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg == TRUE)
        {
            // In the new peripherals, VREG_READY bit is used which indicates both VREG_OK and STEPPER_OK
            vreg_mask = VREG_OK; 
        }
        else if (pwr_data->pwr_specific_info[peripheral_index].is_periph_stepper == TRUE)
        {
            vreg_mask = STEPPER_OK; 
        }
        else
        {
            vreg_mask = VREG_OK;
        }

        if ((data & vreg_mask) == vreg_mask)
        {
            *vreg_ready = TRUE;
        }
        else
        {
            *vreg_ready = FALSE;
        }
    }

    if (PM_ERR_FLAG_SUCCESS != err_flag)
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_PWR_SW_EN_ERROR, comm_ptr->pmic_index, periph_type, peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_status_reg_dump_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, 
                                            uint8 *status_reg_data, uint32 num_of_regs)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    //uint8 periph_type = 0;

    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0))
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }
    else if (status_reg_data == NULL)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type reg = 0x0;

        //periph_type = pwr_data->pwr_specific_info[peripheral_index].periph_type;

        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->STATUS);

        err_flag = pm_comm_read_byte_array(comm_ptr->slave_id, reg, num_of_regs, status_reg_data, 0);
    }

    if (PM_ERR_FLAG_SUCCESS != err_flag)
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_PWR_SW_EN_ERROR, comm_ptr->pmic_index, periph_type, peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_volt_level_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_volt_level_type  volt_level)
{
    pm_err_flag_type                  err_flag = PM_ERR_FLAG_SUCCESS;
    uint32                            vmin = 0;  // microvolt
    uint32                            vStep = 0; // microvolt
    uint32                            range = 0; // 0 to 4
    uint32                            curr_range = 0;
    uint32                            calculated_vset = 0;
    pm_register_data_type             vset_ht[2] = {0};
    pm_register_data_type             vset = 0;
    pm_register_data_type             data = 0;
    pm_register_address_type          reg = 0;
    pm_register_address_type          status_reg = 0;
    uint8                             periph_type = 0;

    if (peripheral_index >= (pwr_data->num_of_peripherals) || 
            pwr_data->pwr_specific_info[peripheral_index].periph_type == 0)
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL ;
    }
    else
    {
        periph_type = pwr_data->pwr_specific_info[peripheral_index].periph_type;

        curr_range = pwr_data->pwr_specific_info[peripheral_index].pwr_range;
        if ((volt_level <= pwr_data->pwr_specific_info[peripheral_index].pwr_vset[curr_range].RangeMax) && 
            (volt_level >= pwr_data->pwr_specific_info[peripheral_index].pwr_vset[curr_range].RangeMin) )
        {
            vmin = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[curr_range].RangeMin;
            vStep = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[curr_range].VStep;
            range = curr_range;
        }
        else
        {
            for(range = pwr_data->pwr_specific_info[peripheral_index].pwr_range_info->start; range <= pwr_data->pwr_specific_info[peripheral_index].pwr_range_info->end; range++)
            {
                if( (volt_level <= pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMax) && 
                    (volt_level >= pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMin) )
                {
                    vmin = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMin;
                    vStep = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].VStep;
                    break;
                }
            }
        }
       
        if (vStep > 0)
        {
            calculated_vset = (volt_level - vmin)/vStep;

            if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg ==  TRUE)
            {
                vset_ht[0] = ((calculated_vset) & 0xFF);
                vset_ht[1] = ((calculated_vset>>8) & 0xFF);

                reg =  (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->VOLTAGE_CTRL1);

                // Set vset
                err_flag = pm_comm_write_byte_array(comm_ptr->slave_id, reg, 2, vset_ht, 0);
            }
            else
            {

                // Set the range first, only if the current range is not equal to requested range
                if(curr_range != range)
                {
                    status_reg = pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->STATUS;

                    data = 0;
 
                    //Read the Vreg OK status
                    err_flag = pm_comm_read_byte(comm_ptr->slave_id, status_reg, &data, 0);

                    if(PM_ERR_FLAG_SUCCESS != err_flag)
                    {
                        return err_flag;
                    }

                    //New range cannot be set if Power rail is ON
                    if(data & 0x80)
                    {
                        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
                    }

                    // The range register VOLT_CTL1 update is not applicable for ULT rails
                    if ((PM_HW_MODULE_ULT_BUCK != periph_type) && (PM_HW_MODULE_ULT_LDO != periph_type))
                    {
                        // Set Range reg
                        reg = pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->VOLTAGE_CTRL1;

                        //Set the new range
                        err_flag = pm_comm_write_byte_mask(comm_ptr->slave_id, reg, 0x07, (pm_register_data_type)range, 0) ; // 0:2

                        if(PM_ERR_FLAG_SUCCESS != err_flag)
                        {
                            return err_flag;
                        }
                    }
                }
                //calculate VSET
                vset = (pm_register_data_type)(calculated_vset);

                // Set VSET
                reg =  (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->VOLTAGE_CTRL2);

                // Update vset value for ULT buck range 1 cases
                if ((PM_HW_MODULE_ULT_BUCK == periph_type) && (1 == range))
                {
                    // since bit<5> and bit<6> are used for range
                    vset |= 0x60; 
                }

                pwr_data->pwr_specific_info[peripheral_index].pwr_range = range;

                // Set vset
                err_flag = pm_comm_write_byte(comm_ptr->slave_id, reg, vset , 0);
            }
        }
        else
        {
            err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
        }
    }

    if (PM_ERR_FLAG_SUCCESS == err_flag)
    {
        //PM_LOG_MSG_DEBUG(PMIC_DRV_PWR_VOLT_LVL, comm_ptr->pmic_index, periph_type, peripheral_index, volt_level);
    }
    else
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_PWR_VOLT_LVL_ERROR, comm_ptr->pmic_index, periph_type, peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_volt_level_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8  peripheral_index, pm_volt_level_type *volt_level)
{
    pm_err_flag_type                 err_flag = PM_ERR_FLAG_SUCCESS;
    uint32                           vmin = 0; // microvolt
    uint32                           vStep = 0; //microvolt
    uint32                           range = 0; // 0 to 4
    uint16                           vset_ht = 0;
    pm_register_data_type            vset = 0;
    pm_register_address_type         reg = 0;
    pm_register_data_type            reg_data[2] = {0};
    uint8                            periph_type = 0;
    boolean                          on_off = FALSE;

    if (peripheral_index >= (pwr_data->num_of_peripherals) || 
            pwr_data->pwr_specific_info[peripheral_index].periph_type == 0)
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }
    else if (volt_level == NULL)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        periph_type = pwr_data->pwr_specific_info[peripheral_index].periph_type;

        if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg ==  TRUE)
        {
            err_flag |= pm_pwr_is_vreg_ready_alg(pwr_data, comm_ptr, peripheral_index, &on_off);
        }

        if ((pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg ==  TRUE)&& (on_off == TRUE))
        {
            reg =  (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->VSET_VALID_LB);
        }
        else
        {
            // get the voltage level LUT
            reg =  (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->VOLTAGE_CTRL1);
        }

        err_flag = pm_comm_read_byte_array(comm_ptr->slave_id, reg, 2, reg_data, 0);

        if ( err_flag == PM_ERR_FLAG_SUCCESS )
        {
            range = pwr_data->pwr_specific_info[peripheral_index].pwr_range;
            vmin = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMin; // microvolt
            vStep = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].VStep; //microvolt

            if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg == TRUE)
            {
                vset_ht = (reg_data[1]<<8) | (reg_data[0]);

                *volt_level = vmin + vset_ht * vStep;
            }
            else
            {
                vset = reg_data[1];
                if ((1 == range) && (PM_HW_MODULE_ULT_BUCK == periph_type))
                {
                    vset &= 0x1F;
                }

                *volt_level = vmin + vset * vStep;
            }
        }
    }

    if (PM_ERR_FLAG_SUCCESS != err_flag)
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_PWR_VOLT_LVL_ERROR, comm_ptr->pmic_index, periph_type, peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_volt_calculate_vset_celing_uv(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, 
                                                      uint8 peripheral_index, pm_volt_level_type req_volt_level, 
                                                      uint32 *calculated_vset, pm_volt_level_type *ceiling_uv )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    uint32           vmin = 0; // microvolt
    uint32           vStep = 0; //microvolt
    uint32           range = 0; //microvolt
    uint8            periph_type = 0;
    uint32           vset =  0;

    if (peripheral_index >= (pwr_data->num_of_peripherals) || 
            pwr_data->pwr_specific_info[peripheral_index].periph_type == 0)
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }
    else if (calculated_vset == NULL || ceiling_uv == NULL)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        periph_type = pwr_data->pwr_specific_info[peripheral_index].periph_type;

        if ((req_volt_level <= pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMax) && 
            (req_volt_level >= pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMin))
        {
            vmin = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMin;
            vStep = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].VStep;
        }

        if (vStep > 0)
        {
            // calculate VSET
            vset = ((req_volt_level - vmin)/vStep);

            if ((1 == range) && (PM_HW_MODULE_ULT_BUCK == periph_type))
            {
                vset |= 0x60;
            }

            /* check if rounding off vset is required - if the voltage request
               cannot be met with the vstep , then it ends up being set to one
               vstep lower. So move it to next higher vstep boundary */
             if( ((vset*vStep) + vmin)  <  req_volt_level )
             {
                // increment the voltage by a step count if the voltage is less than the expected 
                vset++; 
                *ceiling_uv =  (vset*vStep) + vmin; 
             }
             *calculated_vset = vset;
        }
        else
        {
            err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
        }
    }

    if (PM_ERR_FLAG_SUCCESS != err_flag)
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_PWR_VOLT_LVL_ERROR, comm_ptr->pmic_index, periph_type, peripheral_index, err_flag);
    }

    return err_flag;
}





pm_err_flag_type pm_pwr_ocp_latched_status_clear_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, boolean *ocp_occured)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    uint8 periph_type = 0;
    pm_register_address_type ocp_status_reg = 0x0;
    pm_register_address_type sec_reg  = 0x00;
    pm_register_address_type ocp_reg  = 0x0;
    pm_register_data_type status_data = 0;
    pm_register_data_type ocp_clear_data = 0;
    pm_register_data_type status_mask    = 0;
    pm_register_data_type ocp_clear_mask = 0;
  
      
    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0) )
    {
        err_flag |= PM_ERR_FLAG_INVALID_PERIPHERAL;
    }
    else if (NULL == ocp_occured)
    {
       err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        periph_type = pwr_data->pwr_specific_info[peripheral_index].periph_type;

        if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg == TRUE)
        {
            ocp_status_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->STATUS);
            status_mask = 0x1 << 5;
            ocp_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->OCP);
            ocp_clear_mask = 0x1 << 1;
            ocp_clear_data = 0x1 << 1;
        }
        else if (periph_type == PM_HW_MODULE_ULT_LDO)
        {
            /*Only PMOS LDO have OCP but for other LDO either these registers are missing or the bit field is missing 
              and so status will never reported to be high  */
            ocp_status_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + STATUS2_OFFSET);
            status_mask = 0x1 << 6;
            ocp_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + OCP_CTL2_OFFSET);
            ocp_clear_mask = 0x1 << 6;
            ocp_clear_data = 0x1 << 6;
        }
        else
        {
            return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
        }

        err_flag |= pm_comm_read_byte(comm_ptr->slave_id, ocp_status_reg, &status_data, 0);
        if (err_flag != PM_ERR_FLAG_SUCCESS)
        {
            err_flag |= pm_comm_read_byte(comm_ptr->slave_id, ocp_status_reg, &status_data, 0);
        }
        CORE_VERIFY(err_flag == PM_ERR_FLAG_SUCCESS);

        *ocp_occured = (status_data & status_mask)? TRUE : FALSE;

        if (*ocp_occured)
        {
           sec_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->SEC_ACCESS);

           //OCP_STATUS_CLEAR : Write 1d then 0 (HW needs to see a 1 to 0 transition)
           err_flag |= pm_comm_write_byte(comm_ptr->slave_id, sec_reg, 0xA5, 0); 
           err_flag |= pm_comm_write_byte_mask(comm_ptr->slave_id, ocp_reg, ocp_clear_mask, ocp_clear_data, 0);
           err_flag |= pm_comm_write_byte(comm_ptr->slave_id, sec_reg, 0xA5, 0); 
           err_flag |= pm_comm_write_byte_mask(comm_ptr->slave_id, ocp_reg, ocp_clear_mask, 0, 0);
        }

    }
    return err_flag;
}


pm_err_flag_type pm_pwr_ocp_broadcast_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type ocp_reg = 0x0;
    pm_register_address_type sec_reg = 0x0;

    pm_register_data_type ocp_broadcast_data = (on_off) ? (0xFF & OCP_GLOBAL_BROADCAST_EN_MASK) : 0x0;

    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0))
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }
    else
    {
        ocp_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->OCP);
        sec_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->SEC_ACCESS);
        err_flag |= pm_comm_write_byte(comm_ptr->slave_id, sec_reg, SEC_ACCESS_WRITE_DATA, 0);
        err_flag |= pm_comm_write_byte_mask(comm_ptr->slave_id, ocp_reg, OCP_GLOBAL_BROADCAST_EN_MASK, ocp_broadcast_data, 0);
    }

    return err_flag;
}

