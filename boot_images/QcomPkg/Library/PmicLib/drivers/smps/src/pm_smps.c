/*! \file pm_smps.c
*  
*  \brief Implementation file for SMPS public APIs.
*  \n  
*  &copy; Copyright 2010-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/16   aab     Added pm_smps_ocp_latched_status_clear()
03/26/14   vtw     SW workaround for GFX rail multiphase support.
04/25/13   aks     Code Refactoring: Removing PMIC_SubRsc from pwr algs
04/02/13   hs      Added API pm_ipwr_sw_mode_status_lite()
02/27/13   hs      Code refactoring.
12/06/12   hw      Rearchitecturing module driver to peripheral driver
04/09/12   hs      Changed the type for internalResourceIndex from int to uint8.
04/04/12   hs      Removed the buck switch size alg.
03/30/12   hs      Removed the dirver size alg.
04/10/11   wra     Removed the ifdef RPM declaration. RPM has it's own routing layer now
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_smps.h"
#include "pm_smps_driver.h"
#include "pm_comm.h"

/*===========================================================================

                     API IMPLEMENTATION

===========================================================================*/

pm_err_flag_type pm_smps_sw_mode(uint8 pmic_chip, uint8 smps_peripheral_index, pm_sw_mode_type mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
    //uint8 periph_type = 0;

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if(smps_peripheral_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }
    else if (mode >= PM_SW_MODE_INVALID)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type reg = 0;
        pm_register_mask_type    mask = 0;
        pm_register_data_type    data = 0;
        pm_pwr_data_type         *pwr_data = &(smps_ptr->pm_pwr_data);
        boolean is_periph_unified_reg = pwr_data->pwr_specific_info[smps_peripheral_index].is_periph_unified_reg;

        //periph_type = pwr_data->pwr_specific_info[smps_peripheral_index].periph_type;

        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[smps_peripheral_index].periph_base_address + pwr_data->pwr_reg_table->MODE_CTL);

        switch(mode)
        {
        case PM_SW_MODE_LPM: // Low power mode
          {
              if (is_periph_unified_reg == TRUE)
              {
                  data = 0x5;
              }
              else
              {
                  mask = 0xFF;
                  data = 0x00;
              }
          }
          break;
        case PM_SW_MODE_NPM: // normal power mode
          {
              if (is_periph_unified_reg == TRUE)
              {
                  data = 0x7;
              }
              else
              {
                  mask = 0xC0; //bit 7 NPM and bit 6 AUTO_MODE
                  data = 0x80;
              }
          }
          break;
        case PM_SW_MODE_AUTO: // auto mode
          {
              if (is_periph_unified_reg == TRUE)
              {
                  data = 0x6;
              }
              else
              {
                  mask = 0xC0; //bit 7 NPM and bit 6 AUTO_MODE
                  data = 0x40;
              }
          }
          break;
        case PM_SW_MODE_RETENTION: // retention mode
          {
              if (is_periph_unified_reg == TRUE)
              {
                  data = 0x4;
              }
              else
              {
                  err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
              }
          }
          break;
        default:
          err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
          break;
        }

        if (PM_ERR_FLAG_SUCCESS == err_flag)
        {
            if (is_periph_unified_reg == TRUE)
            {
                err_flag = pm_comm_write_byte(smps_ptr->comm_ptr->slave_id, reg, data, 0);
            }
            else
            {
                err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg, mask, data, 0);
            }
        }
    }

    if (PM_ERR_FLAG_SUCCESS == err_flag)
    {
        //PM_LOG_MSG_DEBUG(PMIC_DRV_PWR_MODE, pmic_chip, periph_type, smps_peripheral_index, mode);
    }
    else
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_PWR_MODE_ERROR, pmic_chip, periph_type, smps_peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_smps_sw_mode_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_sw_mode_type *sw_mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_sw_mode_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, sw_mode);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_sw_enable(uint8 pmic_chip, uint8 smps_peripheral_index, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_sw_enable_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, on_off);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_sw_enable_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_sw_enable_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, on_off);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_vreg_ready_status(uint8 pmic_chip, uint8 smps_peripheral_index, boolean* vreg_ready)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_is_vreg_ready_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, vreg_ready);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_volt_level(uint8 pmic_chip, uint8 smps_peripheral_index, pm_volt_level_type volt_level)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_level_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, volt_level);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_volt_level_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_volt_level_type *volt_level)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_level_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, volt_level);
    }
    return err_flag;
}



pm_err_flag_type pm_smps_global_quiet_mode_enable(uint8 pmic_chip, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type reg_comm = 0xFF;
        pm_register_data_type    data = 0;
        pm_pwr_data_type         *pwr_data = &(smps_ptr->pm_pwr_data);

        reg_comm = 0x1000 + pwr_data->pwr_reg_table->QM_MODE;        
        
        if (PM_ON == on_off)
        {
            data = 0xFF;
         }
         else
         {
            data = 0x00;
         }

       err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg_comm, 0x80, data, 0);

    }

    return err_flag;
}

pm_err_flag_type 
pm_smps_quiet_mode_mask(uint8 pmic_chip, uint8 smps_peripheral_index, pm_quiet_mode_type mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
    //uint8 periph_type = 0;

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if(smps_peripheral_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }
    else if (mode >= PM_QUIET_MODE__INVALID)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type    base_addr = 0;
        pm_register_address_type    qm_reg = 0;
        pm_register_address_type    sec_reg = 0;
        pm_pwr_data_type            *pwr_data = &(smps_ptr->pm_pwr_data);
        pm_register_data_type       data = 0;
        pm_register_mask_type       mask = 0x20;

        //periph_type = pwr_data->pwr_specific_info[smps_peripheral_index].periph_type;

        /* SMPSx_CTRL peripheral base address */
        base_addr = pwr_data->pwr_specific_info[smps_peripheral_index].periph_base_address;

        sec_reg =  base_addr + pwr_data->pwr_reg_table->SEC_ACCESS;
        
        qm_reg = (pm_register_address_type)(base_addr + pwr_data->pwr_reg_table->QMODE_SHADOW);

        switch(mode)
        {
        case PM_QUIET_MODE__DISABLE:
            {
                //QM_SHADOW: FOLLOW_QM bit<5> = 0
                mask = 0x20;
                data = 0x0; 
            }
            break;
        case PM_QUIET_MODE__QUIET:
            {
                //QM_SHADOW: FOLLOW_QM bit<5> = 1 
                mask = 0x20;
                data = 0x20; 
            }
            break;
        case PM_QUIET_MODE__SUPER_QUIET:
            {
                //QM_SHADOW: FOLLOW_QM bit<5> = 1
                mask = 0x20;
                data = 0x20; 
            }
            break;
        default:
            err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
            break;
        }

        if (err_flag == PM_ERR_FLAG_SUCCESS)
        {
            /* QM reg needs protected access. Write to SEC_ACCESS register */
            err_flag = pm_comm_write_byte(smps_ptr->comm_ptr->slave_id, sec_reg, 0xA5, 0);

            /* Write to QM register */
            err_flag |= pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, qm_reg, mask, data, 0);
        }
    }

    if (PM_ERR_FLAG_SUCCESS != err_flag)
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_SMPS_QM_ERROR, pmic_chip, periph_type, smps_peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_smps_switching_freq(uint8 pmic_chip, uint8 smps_peripheral_index, pm_smps_switching_freq_type freq)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
    //uint8 periph_type = 0;

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if(smps_peripheral_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }
    else if ( freq >= PM_SWITCHING_FREQ_INVALID )
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type    reg = 0;
        pm_register_address_type    smps_ctrl_base_addr = 0;
        pm_register_address_type    smps_freq_base_addr = 0;
        pm_register_address_type    smps_freq_periph_offset = 0x200;
        pm_pwr_data_type            *pwr_data = &(smps_ptr->pm_pwr_data);

        //periph_type = pwr_data->pwr_specific_info[smps_peripheral_index].periph_type;

        /* SMPSx_CTRL peripheral base address */
        smps_ctrl_base_addr = pwr_data->pwr_specific_info[smps_peripheral_index].periph_base_address;

        /* SMPSx_FREQ peripheral base address */
        smps_freq_base_addr =  smps_ctrl_base_addr + smps_freq_periph_offset;

        /* Implementation is different on FTS and HF bucks */
        if (pwr_data->pwr_specific_info[smps_peripheral_index].periph_type == PM_HW_MODULE_HF_BUCK)
        {
            /* We need to set the freq in SMPSx_FREQ peripheral for HF Bucks */

            /* CLK_DIV needs protected access. Write to SEC_ACCESS register */
            reg =  smps_freq_base_addr + pwr_data->pwr_reg_table->SEC_ACCESS;

            err_flag = pm_comm_write_byte(smps_ptr->comm_ptr->slave_id, reg, 0xA5, 0);

            if (err_flag == PM_ERR_FLAG_SUCCESS)
            {
                /* Write to CLK_DIV register */
                reg =  smps_freq_base_addr + pwr_data->pwr_reg_table->CLK_DIV;

                err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg, 0x1F, (pm_register_data_type)freq, 0); // <0:4>
            }
        }
        else if (pwr_data->pwr_specific_info[smps_peripheral_index].periph_type == PM_HW_MODULE_FTS)
        {
            /* We need to set the freq in SMPSx_CTRL peripheral for FTS Bucks */

            /* CLK_DIV needs protected access. Write to SEC_ACCESS register */
            reg =  smps_ctrl_base_addr + pwr_data->pwr_reg_table->SEC_ACCESS;

            err_flag = pm_comm_write_byte(smps_ptr->comm_ptr->slave_id, reg, 0xA5, 0);

            if (err_flag == PM_ERR_FLAG_SUCCESS)
            {
                /* Write to CLK_DIV register */
                reg =  smps_ctrl_base_addr + pwr_data->pwr_reg_table->CLK_DIV;

                err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg, 0x1F, (pm_register_data_type)freq, 0); // <0:4>
            }
        }
        else
        {
            err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
        }
    }

    if (PM_ERR_FLAG_SUCCESS != err_flag)
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_SMPS_SWITCH_FREQ_ERROR, pmic_chip, periph_type, smps_peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_smps_switching_freq_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_smps_switching_freq_type *freq)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
    //uint8 periph_type = 0;

    if (smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if(smps_peripheral_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }
    else if (freq == NULL)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type    reg = 0;
        pm_register_address_type    smps_freq_periph_offset = 0x200;
        pm_register_data_type       data_read = 0;
        pm_pwr_data_type                      *pwr_data = &(smps_ptr->pm_pwr_data);

        //periph_type = pwr_data->pwr_specific_info[smps_peripheral_index].periph_type;
        
        /* We can read back the frequency from SMPSx_FREQ peripheral CLK_DIV reg for both FTS and HF bucks */
        reg = pwr_data->pwr_specific_info[smps_peripheral_index].periph_base_address + pwr_data->pwr_reg_table->CLK_DIV + smps_freq_periph_offset;

        err_flag = pm_comm_read_byte_mask(smps_ptr->comm_ptr->slave_id, reg, 0x1F, &data_read,0);  // <0:4>

        if (err_flag == PM_ERR_FLAG_SUCCESS)
        {
            *freq = (pm_smps_switching_freq_type)data_read;
        }
        else
        {
            *freq = PM_SWITCHING_FREQ_INVALID;
        }
    }

    if (PM_ERR_FLAG_SUCCESS != err_flag)
    {
        //PM_LOG_MSG_ERROR(PMIC_DRV_SMPS_SWITCH_FREQ_ERROR, pmic_chip, periph_type, smps_peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type 
pm_smps_pull_down(uint8 pmic_chip, uint8 smps_peripheral_index, pm_on_off_type on_off) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

  if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_pull_down_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, on_off);
    }

    return err_flag;
}

pm_err_flag_type
pm_smps_pull_down_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_pull_down_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, on_off);
    }

    return err_flag;
}

pm_err_flag_type 
pm_smps_pin_ctrled(uint8 pmic_chip, uint8 smps_peripheral_index, uint8 select_pin)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

  if (smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_pin_ctrl_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, select_pin);
    }

    return err_flag;
}


pm_err_flag_type pm_smps_pin_ctrled_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_pin_ctrl_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, on_off);
    }

    return err_flag;
}



pm_err_flag_type 
pm_smps_ocp_latched_status_clear(uint8 pmic_chip, uint8 peripheral_index,  boolean *ocp_occured)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

   if (smps_ptr == NULL)
   {
       err_flag |= PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if(peripheral_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
   {
      err_flag |= PM_ERR_FLAG_INVALID_PERIPHERAL;
   }
   else
   {
       err_flag |= pm_pwr_ocp_latched_status_clear_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, peripheral_index, ocp_occured);
   }

   return err_flag;
}


pm_err_flag_type pm_smps_ocp_broadcast(uint8 pmic_chip, uint8  smps_peripheral_index, pm_on_off_type  on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_ocp_broadcast_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, on_off);
    }

    return err_flag;
}




