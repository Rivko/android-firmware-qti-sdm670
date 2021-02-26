/*! \file pm_pwr_alg.c 
*  \n
*  \brief  
*  \n  
*  \n &copy; Copyright 2012-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/drivers/pwr/src/pm_pwr_alg.c#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
07/22/14   aks     Adding API to round off voltage
12/06/12   hw      Rearchitecturing module driver to peripheral driver
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pwr_alg.h"
#include "pm_comm.h"

#define VREG_READY_MASK   0x80
#define PWR_MODE_MASK     0x7
/*===========================================================================

                     FUNCTION IMPLEMENTATION 

===========================================================================*/

pm_err_flag_type pm_pwr_sw_mode_raw_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, uint8 mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0))
    {
        err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
    }
    else if (mode > PWR_MODE_MASK)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg == FALSE)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type reg = 0x0;
        pm_register_data_type data = mode;
            
        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->MODE_CTL);

        err_flag = pm_comm_write_byte(comm_ptr->slave_id, reg, data, 0);
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_sw_mode_status_raw_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, uint8 *mode_ctl)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
                                                          
    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0))
    {
        err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
    }
    else if (mode_ctl == NULL)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type           reg = 0x0;
        pm_register_data_type              data = 0xFF;
            
        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->MODE_CTL);

        err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &data, 0);
        
        if (err_flag == PM_ERR_FLAG_SUCCESS)
        {
            *mode_ctl = data;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_sw_enable_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0))
    {
        err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
    }
    else if (on_off >= PM_INVALID)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type reg = 0x0;
        pm_register_data_type data = 0;
            
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

    return err_flag;
}

pm_err_flag_type pm_pwr_sw_enable_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0))
    {
        err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
    }
    else if (on_off == NULL)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type           reg = 0x0;
        pm_register_data_type              data = 0;

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

    return err_flag;
}

pm_err_flag_type pm_pwr_is_vreg_ready_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, boolean *vreg_ready)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    uint8 vreg_mask = 0;
    uint8 periph_type = 0;

    if (peripheral_index >= pwr_data->num_of_peripherals || 
             pwr_data->pwr_specific_info[peripheral_index].periph_type == 0)
    {
        err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
    }
    else if (vreg_ready == NULL)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type reg = 0x0;
        pm_register_data_type data = 0;
        periph_type = pwr_data->pwr_specific_info[peripheral_index].periph_type;

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

        vreg_mask = VREG_READY_MASK;

        if ((data & vreg_mask) == vreg_mask)
        {
            *vreg_ready = TRUE;
        }
        else
        {
            *vreg_ready = FALSE;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_volt_level_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, pm_volt_level_type  volt_level)
{
    pm_err_flag_type                  err_flag = PM_ERR_FLAG_SUCCESS;
    uint32                            vmin = 0;  // microvolt
    uint32                            vStep = 0; // microvolt
    uint32                            calculated_vset = 0;
    pm_register_data_type             vset_ht[2] = {0};
    pm_register_address_type          reg = 0;

    if (peripheral_index >= (pwr_data->num_of_peripherals) || 
            pwr_data->pwr_specific_info[peripheral_index].periph_type == 0)
    {
        err_flag = PM_ERR_FLAG_PERIPHERAL_ERR ;
    }
    else if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg == FALSE)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        if ((volt_level <= pwr_data->pwr_specific_info[peripheral_index].pwr_vset.RangeMax) && 
            (volt_level >= pwr_data->pwr_specific_info[peripheral_index].pwr_vset.RangeMin) )
        {
            vmin = pwr_data->pwr_specific_info[peripheral_index].pwr_vset.RangeMin;
            vStep = pwr_data->pwr_specific_info[peripheral_index].pwr_vset.VStep;
        }
       
        if (vStep > 0)
        {
            calculated_vset = (volt_level - vmin)/vStep;

            vset_ht[0] = ((calculated_vset) & 0xFF);
            vset_ht[1] = ((calculated_vset>>8) & 0xFF);

            reg =  (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->VOLTAGE_CTRL1);

            // Set vset
            err_flag = pm_comm_write_byte_array(comm_ptr->slave_id, reg, 2, vset_ht, 0);
        }
        else
        {
            err_flag = PM_ERR_FLAG_DATA_VERIFY_ERR;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_pin_ctrl_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, pm_on_off_type *on_off, uint8 *select_pin)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type reg = 0x0;
    pm_register_data_type data = 0;
    uint8 mask = 0x0F;

    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0))
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        if(pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg == TRUE)
        {
            reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->FOLLOW_HWEN);
        }
        else 
        {   
            reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->EN_CTL);
        }
        
        err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &data, 0);
        if(err_flag != PM_ERR_FLAG_SUCCESS)
        {
            *on_off = PM_INVALID;
            return err_flag;
        }

        *select_pin = (uint8)(data & mask);

        if(*select_pin != 0)
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

pm_err_flag_type pm_pwr_volt_level_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, pm_volt_level_type *volt_level)
{
    pm_err_flag_type                 err_flag = PM_ERR_FLAG_SUCCESS;
    uint32                           vmin = 0; // microvolt
    uint32                           vStep = 0; //microvolt
    uint16                           vset_ht = 0;
    pm_register_address_type         reg = 0;
    pm_register_data_type            reg_data[2] = {0};

    if (peripheral_index >= (pwr_data->num_of_peripherals) || 
            pwr_data->pwr_specific_info[peripheral_index].periph_type == 0)
    {
        err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
    }
    else if (volt_level == NULL)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg == FALSE)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {

        // get the voltage level LUT
        reg =  (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->VOLTAGE_CTRL1);

        err_flag |= pm_comm_read_byte_array(comm_ptr->slave_id, reg, 2, reg_data, 0);

        if ( err_flag == PM_ERR_FLAG_SUCCESS )
        {
            vmin = pwr_data->pwr_specific_info[peripheral_index].pwr_vset.RangeMin; // microvolt
            vStep = pwr_data->pwr_specific_info[peripheral_index].pwr_vset.VStep; //microvolt

            vset_ht = (reg_data[1]<<8) | (reg_data[0]);
            *volt_level = vmin + vset_ht * vStep;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_volt_get_vstep(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, uint8 *vstep_ptr)
{
    pm_err_flag_type                 err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type         reg = 0;

    if (peripheral_index >= (pwr_data->num_of_peripherals) || 
            pwr_data->pwr_specific_info[peripheral_index].periph_type == 0)
    {
        err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
    }
    else if (vstep_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else if ((pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg == FALSE) || 
             (pwr_data->pwr_specific_info[peripheral_index].is_periph_stepper == FALSE))
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        reg =  (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->VOLTAGE_STEP_CTL);
        
        err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, vstep_ptr, 0);
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_enable_get_softstart(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, uint8* ss_ptr)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type reg = 0;

    if (peripheral_index >= (pwr_data->num_of_peripherals) || 
            pwr_data->pwr_specific_info[peripheral_index].periph_type == 0)
    {
        err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
    }
    else if (ss_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else if ((pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg == FALSE) || 
             (pwr_data->pwr_specific_info[peripheral_index].is_periph_stepper == FALSE) ||
             (pwr_data->pwr_specific_info[peripheral_index].periph_type == PM_HW_MODULE_LDO) ||
             (pwr_data->pwr_specific_info[peripheral_index].periph_type == PM_HW_MODULE_ULT_LDO))
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        reg =  (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->SOFT_START_CTL);
        
        err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, ss_ptr, 0);
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_irq_enable_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, pm_pwr_irq_type irq, boolean enable)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0))
    {
        err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
    }
    else if (irq >= PM_PWR_IRQ_INVALID)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg == FALSE)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type reg = 0;
        uint8 irq_mask = 0;

        switch (irq)
        {
        case PM_PWR_IRQ_VREG_FAULT:
          irq_mask = 0x01;
          break;
        case PM_PWR_IRQ_VREG_READY:
          irq_mask = 0x02;
          break;
        default:
          // This check is already done so exec cannot reach here
          err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
          break;
        }
         
        if (enable == TRUE)
        {
            reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->INT_EN_SET);
        }
        else
        {
            reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->INT_EN_CLR);
        }

        if (PM_ERR_FLAG_SUCCESS == err_flag)
        {
            err_flag = pm_comm_write_byte(comm_ptr->slave_id, reg, irq_mask, 0);
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_irq_clear_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, pm_pwr_irq_type irq)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0))
    {
        err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
    }
    else if (irq >= PM_PWR_IRQ_INVALID)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg == FALSE)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type reg = 0;
        uint8 irq_mask = 0;

        switch (irq)
        {
        case PM_PWR_IRQ_VREG_FAULT:
          irq_mask = 0x01;
          break;
        case PM_PWR_IRQ_VREG_READY:
          irq_mask = 0x02;
          break;
        default:
          // This check is already done so exec cannot reach here
          err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
          break;
        }

        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->INT_LATCHED_CLR);

        if (PM_ERR_FLAG_SUCCESS == err_flag)
        {
            err_flag = pm_comm_write_byte(comm_ptr->slave_id, reg, irq_mask, 0);
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_irq_set_trigger_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, pm_pwr_irq_type irq, pm_irq_trigger_type trigger)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0))
    {
        err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
    }
    else if ((irq >= PM_PWR_IRQ_INVALID) || (trigger >= PM_IRQ_TRIGGER_INVALID))
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg == FALSE)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type INT_SET_TYPE = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->INT_SET_TYPE);
        pm_register_address_type INT_POLARITY_HIGH = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->INT_POLARITY_HIGH);
        pm_register_address_type INT_POLARITY_LOW = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->INT_POLARITY_LOW);
        uint8 irq_mask = 0;
        uint8 set_type = 0;
        uint8 polarity_high = 0;
        uint8 polarity_low = 0;

        switch (irq)
        {
        case PM_PWR_IRQ_VREG_FAULT:
          irq_mask = 0x01;
          break;
        case PM_PWR_IRQ_VREG_READY:
          irq_mask = 0x02;
          break;
        default:
          // This check is already done so exec cannot reach here
          err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
          break;
        }

        switch (trigger)
        {
        case PM_IRQ_TRIGGER_ACTIVE_LOW:
          set_type = 0x00;
          polarity_high = 0x00;
          polarity_low = 0xFF;
          break;
        case PM_IRQ_TRIGGER_ACTIVE_HIGH:
          set_type = 0x00;
          polarity_high = 0xFF;
          polarity_low = 0x00;
          break;
        case PM_IRQ_TRIGGER_RISING_EDGE:
          set_type = 0xFF;
          polarity_high = 0xFF;
          polarity_low = 0x00;
          break;
        case PM_IRQ_TRIGGER_FALLING_EDGE:
          set_type = 0xFF;
          polarity_high = 0x00;
          polarity_low = 0xFF;
          break;
        case PM_IRQ_TRIGGER_DUAL_EDGE:
          set_type = 0xFF;
          polarity_high = 0xFF;
          polarity_low = 0xFF;
          break;
        default:
          // This check is already done so exec cannot reach here
          err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
          break;
        }

        if (PM_ERR_FLAG_SUCCESS == err_flag)
        {
            err_flag = pm_comm_write_byte_mask(comm_ptr->slave_id, INT_SET_TYPE, irq_mask, set_type, 0);
            err_flag |= pm_comm_write_byte_mask(comm_ptr->slave_id, INT_POLARITY_HIGH, irq_mask, polarity_high, 0);
            err_flag |= pm_comm_write_byte_mask(comm_ptr->slave_id, INT_POLARITY_LOW, irq_mask, polarity_low, 0);
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_ocp_latched_status_clear_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, boolean *ocp_occured)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type ocp_status_reg = 0x0;
    pm_register_address_type ocp_reg = 0x0;
    pm_register_address_type sec_reg = 0x0;
    pm_register_data_type status_data = 0;
    pm_register_data_type ocp_clear_data = 0;
    pm_register_data_type status_mask = 0;
    pm_register_data_type ocp_clear_mask = 0;

    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0))
    {
        err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
    }
    else if(NULL == ocp_occured)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        if (pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg == TRUE)
        {
            ocp_status_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->STATUS);
            status_mask = 0x1 << 5;
            ocp_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->OCP);
            ocp_clear_mask = 0x1 << 1;
            ocp_clear_data = 0x1 << 1;
        }
        else
        {
            return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
        }

        err_flag = pm_comm_read_byte(comm_ptr->slave_id, ocp_status_reg, &status_data, 0);

        if (err_flag != PM_ERR_FLAG_SUCCESS)
        {
            err_flag = pm_comm_read_byte(comm_ptr->slave_id, ocp_status_reg, &status_data, 0);
        }

        *ocp_occured = (status_data & status_mask) ? TRUE : FALSE ;

        if (*ocp_occured == TRUE)
        {
            sec_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->SEC_ACCESS);
            /*write 1 and then 0 to clear OCP*/
            err_flag |= pm_comm_write_byte(comm_ptr->slave_id, sec_reg, 0xA5, 0);
            err_flag |= pm_comm_write_byte_mask(comm_ptr->slave_id, ocp_reg, ocp_clear_mask, ocp_clear_data, 0);
            err_flag |= pm_comm_write_byte(comm_ptr->slave_id, sec_reg, 0xA5, 0);
            err_flag |= pm_comm_write_byte_mask(comm_ptr->slave_id, ocp_reg, ocp_clear_mask, 0, 0);
        }
    }

    return err_flag;
}
