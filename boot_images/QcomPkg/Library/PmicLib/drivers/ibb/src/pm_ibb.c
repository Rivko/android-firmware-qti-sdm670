/*! \file
*
*  \brief  pm_ibb.c driver implementation.
*  \details charger driver implementation.
*  &copy;
*  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

when        who     what, where, why
--------    ----    ---------------------------------------------------------
12/02/16    ks      Added support for new IBB Type/Subtype
03/29/16    al      Updating for newer target
08/20/14    al      Updating comm lib
07/03/14    al      Adding API for delay config
05/20/14    al      Architecture update
04/30/14    al      Initial revision
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_ibb_driver.h"
#include "pm_ibb.h"


/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/
#define PM_IBB_VOUT_MIN                 400     /* (-ve) in mV */
#define PM_IBB_VOUT_MAX                 6300    /* (-ve) in mV */
#define PM_IBB_VOUT_STEP                100     /* (-ve) in mV */

#define PM_IBB_CURRENT_MAX              1550    /* in mA */
#define PM_IBB_CURRENT_STEP             50      /* in mA */

#define PM_IBB_STARTUP_IPLIMIT_MIN      175     /* in mA */
#define PM_IBB_STARTUP_IPLIMIT_MAX      925     /* in mA */


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
static inline pm_err_flag_type pm_ibb_unlock_perph_write(pm_ibb_data_type *ibb_ptr);


pm_err_flag_type pm_ibb_lcd_amoled_sel(uint32 device_index, boolean en_amoled_mode)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;

   pm_ibb_data_type *ibb_ptr  = pm_ibb_get_data(device_index);

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type lcd_amoled_sel = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->lcd_amoled_sel;
      data = (en_amoled_mode)? 0x80 : 0x00 ;
      err_flag = pm_ibb_unlock_perph_write(ibb_ptr);
      err_flag |= pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, lcd_amoled_sel, 0x80, data,0);
   }

   return err_flag;
}


pm_err_flag_type pm_ibb_ibb_module_rdy(uint32 device_index, boolean ibb_rdy)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;

   pm_ibb_data_type *ibb_ptr  = pm_ibb_get_data(device_index);

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type module_rdy = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->module_rdy;
      data = (ibb_rdy)? 0x80 : 0x00;
      err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, module_rdy, 0x80, data,0);
   }

   return err_flag;
}




pm_err_flag_type pm_ibb_config_ibb_ctrl(uint32 device_index, boolean en_module, boolean swire_rdy)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;

   pm_ibb_data_type *ibb_ptr  = pm_ibb_get_data(device_index);

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type enable_ctl = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->enable_ctl;
      data = (pm_register_data_type)(en_module<<7 | swire_rdy<<6);
      err_flag |= pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, enable_ctl, 0xC0, data,0);
   }

   return err_flag;
}


pm_err_flag_type pm_ibb_set_soft_strt_chgr_resistor(uint32 device_index, uint32 resistor_kohms)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;

   pm_ibb_data_type *ibb_ptr  = pm_ibb_get_data(device_index);

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type soft_start_ctl = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->soft_start_ctl;

      if (resistor_kohms > 32)
      {
         data = (resistor_kohms>64)? 0x0 : 0x1;
      }
      else
      {
         data = (resistor_kohms>16)? 0x2 : 0x3;
      }

      err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, soft_start_ctl, 0x03, data,0);
   }

   return err_flag;
}


pm_err_flag_type pm_ibb_set_swire_output_pulse(uint32 device_index, uint32 volt_millivolt)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;

   pm_ibb_data_type *ibb_ptr  = pm_ibb_get_data(device_index);

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type swire_ctl = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->swire_ctl;

      data = (volt_millivolt > 7600) ? 0x3F : ((volt_millivolt - 1400)/100);

      err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, swire_ctl, 0x3F, data,0);
   }

   return err_flag;
}



pm_err_flag_type pm_ibb_config_output_volt(uint32 device_index, boolean override_output_volt, uint32 volt_millivolt)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;

   pm_ibb_data_type *ibb_ptr  = pm_ibb_get_data(device_index);

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type output_voltage = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->output_voltage;
      uint8 ibb_subtype = ibb_ptr->periph_revision.periph_subtype;

      if(ibb_subtype == 1)
      {
        /*to make sure value does not go negative in next step*/
        volt_millivolt = (volt_millivolt<1400)? 1400 : volt_millivolt;
        volt_millivolt = (volt_millivolt > 7600) ? 0x3F : ((volt_millivolt - 1400)/100);
        data = (pm_register_data_type)((override_output_volt << 7) | (0x3F & volt_millivolt));

        err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, output_voltage, 0xBF, data,0);
      }
      else
      {
        pm_register_data_type mask = 0;

        if (volt_millivolt < PM_IBB_VOUT_MIN ||
            volt_millivolt > PM_IBB_VOUT_MAX)
        {
            return PM_ERR_FLAG_INVALID_PARAMETER;
        }

        mask = PM_BITS(5, 0);

        /* Output voltage can be set between -0.4V to -6.3V. Vout = -(100mV * code) */
        data = volt_millivolt / PM_IBB_VOUT_STEP;
        if(volt_millivolt % PM_IBB_VOUT_STEP)
          data += 1;

        err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, output_voltage, mask, data, 0);
      }
   }

   return err_flag;
}


pm_err_flag_type pm_ibb_config_pwrup_pwrdn_dly(uint32 device_index, pm_ibb_pwrup_pwrdn_dly_type *delay_config)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;

   pm_ibb_data_type *ibb_ptr  = pm_ibb_get_data(device_index);

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if ((NULL == delay_config)
            ||(delay_config->pwrdn_dly_src >= PM_IBB_PWRDN_DLY_SRC_INVALID)
            ||(delay_config->pwrup_dly_src >= PM_IBB_PWRUP_DLY_SRC_INVALID))
   {
      err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else
   {
      /*valid values are 1ms:0x0,2ms:0x1,4ms:0x2,8ms:0x3*/
      delay_config->pwrup_dly_ms =  (delay_config->pwrup_dly_ms > 4)? 0x3 : (delay_config->pwrup_dly_ms)/2;
      delay_config->pwrdn_dly_ms =  (delay_config->pwrdn_dly_ms > 4)? 0x3 : (delay_config->pwrdn_dly_ms)/2;

      pm_register_address_type pwrup_pwrdn_ctl_1 = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->pwrup_pwrdn_ctl_1;

      data = (pm_register_data_type)((delay_config->pwrdn_dly_ms) | (0x1<<0x3)/*EN_PWRDN_DLY2*/ |(delay_config->pwrdn_dly_src <<2) | (delay_config->pwrup_dly_ms<<4) | (delay_config->en_pwrup_dly<<6) | (delay_config->pwrup_dly_src<<7));

      err_flag |= pm_ibb_unlock_perph_write(ibb_ptr);

      err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, pwrup_pwrdn_ctl_1, 0xFF, data,0);
   }

   return err_flag;
}

pm_err_flag_type pm_ibb_get_ibb_status(uint32 device_index, pm_ibb_status_type *ibb_status)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data = 0, ibb_sts[4] = {0};

   pm_ibb_data_type *ibb_ptr  = pm_ibb_get_data(device_index);

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == ibb_status)
   {
      err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else
   {
     uint8 ibb_subtype = ibb_ptr->periph_revision.periph_subtype;
     memset(ibb_status, 0 , sizeof(pm_ibb_status_type));

     pm_register_address_type output_voltage = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->output_voltage;
     err_flag = pm_comm_read_byte(ibb_ptr->comm_ptr->slave_id, output_voltage, &data, 0);
     if(ibb_subtype == 1)
     {
       ibb_status->override_output_volt = (data & 0x80)? TRUE: FALSE;
       ibb_status->output_volt = ((data& 0x3F)*100) + 1400;
     }
     else
     {
       ibb_status->output_volt = (data & 0x3F) * 100;
     }

     pm_register_address_type swire_ctl = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->swire_ctl;
     err_flag |= pm_comm_read_byte(ibb_ptr->comm_ptr->slave_id, swire_ctl, &data,0);
     if(ibb_subtype == 1)
     {
       ibb_status->swire_pulse_mv = ((data & 0x3F)*100) + 1400;
     }
     else
     {
       ibb_status->swire_pulse_mv = (data & 0x3F) * 100;
     }

     if(ibb_subtype == 1)
     {
       pm_register_address_type soft_start_ctl = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->soft_start_ctl;
       err_flag |= pm_comm_read_byte(ibb_ptr->comm_ptr->slave_id, soft_start_ctl, &data,0);
       switch (data & 0x03)
       {
          case 0: ibb_status->chgr_resistor_kohms = 300; break;
          case 1: ibb_status->chgr_resistor_kohms =  64; break;
          case 2: ibb_status->chgr_resistor_kohms =  32; break;
          case 3: ibb_status->chgr_resistor_kohms =  16; break;
          default: ibb_status->chgr_resistor_kohms = 0;
       }
     }

     pm_register_address_type enable_ctl = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->enable_ctl;
     err_flag |= pm_comm_read_byte(ibb_ptr->comm_ptr->slave_id, enable_ctl, &data,0);
     ibb_status->ibb_module_en = (data & 0x80) ? TRUE : FALSE;
     ibb_status->swire_rdy = (data & 0x40)? TRUE : FALSE;

     pm_register_address_type module_rdy = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->module_rdy;
     err_flag |= pm_comm_read_byte(ibb_ptr->comm_ptr->slave_id, module_rdy, &data, 0);
     ibb_status->ibb_module_rdy = (data & 0x80)? TRUE : FALSE;

     if(ibb_subtype == 1)
     {
       pm_register_address_type lcd_amoled_sel = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->lcd_amoled_sel;
       err_flag |= pm_comm_read_byte(ibb_ptr->comm_ptr->slave_id, lcd_amoled_sel, &data,0);
       ibb_status->en_amoled_mode = (data & 0x80)? TRUE : FALSE;
     }

     pm_register_address_type pwrup_pwrdn_ctl_1 = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->pwrup_pwrdn_ctl_1;
     err_flag |= pm_comm_read_byte(ibb_ptr->comm_ptr->slave_id, pwrup_pwrdn_ctl_1, &data,0);
     ibb_status->ibb_dly_config.pwrdn_dly_ms     = 1<< (data & 0x03);
     ibb_status->ibb_dly_config.pwrdn_dly_src    = (pm_ibb_pwrdn_dly_src_type)((data & 0x04)>>2);
     ibb_status->ibb_dly_config.pwrup_dly_ms     = 1<<((data & 0x30)>>4);
     ibb_status->ibb_dly_config.en_pwrup_dly     = (data & 0x40)? TRUE : FALSE;
     ibb_status->ibb_dly_config.pwrup_dly_src    = (pm_ibb_pwrup_dly_src_type)((data & 0x80)>>7);

     pm_register_address_type status1 = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->status1;
     err_flag |= pm_comm_read_byte_array(ibb_ptr->comm_ptr->slave_id, status1, 4, ibb_sts, 0);
     ibb_status->ibb_vreg_ok  =  (ibb_sts[0] & 0x80) ? TRUE: FALSE;
     if(ibb_subtype == 1)
     {
       ibb_status->short_circuit = (ibb_sts[0] & 0x40) ? TRUE: FALSE;
     }
     else
     {
       ibb_status->overload = (ibb_sts[0] & 0x40) ? TRUE: FALSE;
     }
     ibb_status->ilimit_error =  (ibb_sts[0] & 0x20) ? TRUE: FALSE;

     ibb_status->ibb_mode_status = (ibb_sts[2] & 0x0F);

     ibb_status->swire_edge_count = (ibb_sts[3] & 0x7F);
   }

   return err_flag ;
};


static inline pm_err_flag_type pm_ibb_unlock_perph_write(pm_ibb_data_type *ibb_ptr)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type sec_access = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->sec_access;
      err_flag = pm_comm_write_byte(ibb_ptr->comm_ptr->slave_id, sec_access, 0xA5, 0);
   }

   return err_flag;
}


/* For IBB 3.0/+ Revision */
pm_err_flag_type pm_ibb_get_rt_status(uint8 pmic_index,
    pm_ibb_rt_status_type *rt_status
    )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0;
    pm_register_address_type ibb_int_rt_sts = 0;
    pm_ibb_data_type *ibb_ptr = pm_ibb_get_data(pmic_index);

    if (NULL == ibb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (NULL == rt_status)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    ibb_int_rt_sts = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->int_rt_sts;

    memset(rt_status, 0 , sizeof(pm_ibb_rt_status_type));

    err_flag |= pm_comm_read_byte(ibb_ptr->comm_ptr->slave_id, ibb_int_rt_sts, &data, 0);

    if(PM_ERR_FLAG__SUCCESS == err_flag)
    {
        rt_status->fault_shutdown = (PM_BIT(2) & data) ? TRUE : FALSE;
        rt_status->overload = (PM_BIT(1) & data) ? TRUE : FALSE;
        rt_status->ilimit_error = (PM_BIT(0) & data) ? TRUE : FALSE;
    }

    return err_flag;
}

pm_err_flag_type pm_ibb_set_default_voltage(uint8 pmic_index,
    uint32 op_voltage_mv
    )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type ibb_default_voltage = 0;
    pm_ibb_data_type *ibb_ptr = pm_ibb_get_data(pmic_index);

    if (NULL == ibb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (op_voltage_mv < PM_IBB_VOUT_MIN ||
        op_voltage_mv > PM_IBB_VOUT_MAX)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    ibb_default_voltage = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->default_voltage;

    mask = PM_BITS(5, 0);

    /* Output voltage can be set between -0.4V to -6.3V. Vout = -(100mV * code) */
    data = op_voltage_mv / PM_IBB_VOUT_STEP;
    if(op_voltage_mv % PM_IBB_VOUT_STEP)
        data += 1;

    err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, ibb_default_voltage, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_ibb_pd_ctrl(uint8 pmic_index,
    pm_ibb_pd_ctrl_type *pd_ctrl
    )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type ibb_pd_ctl = 0;
    pm_ibb_data_type *ibb_ptr = pm_ibb_get_data(pmic_index);

    if (NULL == ibb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (NULL == pd_ctrl ||
             pd_ctrl->swire_pd_updt >= PM_IBB_PD_CTRL_SWIRE_PD_UPDT__INVALID ||
             pd_ctrl->pd_strenght >= PM_IBB_PD_CTRL_PD_STRENGHT__INVALID)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    ibb_pd_ctl = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->pd_ctl;

    mask = PM_BIT(7);
    data = pd_ctrl->enable_pd << PM_SHIFT_FROM_MASK(mask);

    if(pd_ctrl->enable_pd)
    {
        mask |= PM_BIT(1);
        data |= pd_ctrl->swire_pd_updt << PM_SHIFT_FROM_MASK(mask);

        mask |= PM_BIT(0);
        data |= pd_ctrl->pd_strenght << PM_SHIFT_FROM_MASK(mask);
    }

    err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, ibb_pd_ctl, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_ibb_iplimit_startup_ctrl(uint8 pmic_index,
    pm_ibb_iplimit_startup_ctrl_type *iplimit_ctrl
    )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    uint32 startup_iplimit = 0;
    pm_register_address_type ibb_iplimit_startup_ctl_1 = 0, ibb_iplimit_startup_ctl_2 = 0;
    pm_ibb_data_type *ibb_ptr = pm_ibb_get_data(pmic_index);

    if (NULL == ibb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (NULL == iplimit_ctrl ||
             iplimit_ctrl->deb_percentage >= PM_IBB_ILIMIT_DEB_PERCENTAGE__INVALID ||
             iplimit_ctrl->startup_iplimit < PM_IBB_STARTUP_IPLIMIT_MIN ||
             iplimit_ctrl->startup_iplimit > PM_IBB_STARTUP_IPLIMIT_MAX ||
             iplimit_ctrl->current_step_size >= PM_IBB_ILIMIT_CURRENT_STEP_SIZE__INVALID ||
             iplimit_ctrl->iplimit_stepper_cycles >= PM_IBB_IPLIMIT_STEPPER_CYCLES__INVALID)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    ibb_iplimit_startup_ctl_1 = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->iplimit_startup_ctl_1;
    ibb_iplimit_startup_ctl_2 = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->iplimit_startup_ctl_2;

    /* IBB_IPLIMIT_STARTUP_CTL_1 */
    mask  = PM_BIT(7);
    data  = iplimit_ctrl->en_iplimit_stepper << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(1, 0);
    data |= iplimit_ctrl->deb_percentage << PM_SHIFT_FROM_MASK(mask);

    err_flag  = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, ibb_iplimit_startup_ctl_1, mask, data, 0);

    /* IBB_IPLIMIT_STARTUP_CTL_2 */
    startup_iplimit = iplimit_ctrl->startup_iplimit - PM_IBB_STARTUP_IPLIMIT_MIN;
    mask  = PM_BITS(7, 4);

    /* Startup IpLimit can be set between 175mA to 925mA. IpLimit = 175 + (50 * code) */
    data = startup_iplimit / PM_IBB_CURRENT_STEP;
    if(startup_iplimit % PM_IBB_CURRENT_STEP)
        data += 1;
    data <<= PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(3, 2);
    data |= iplimit_ctrl->current_step_size << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(1, 0);
    data |= iplimit_ctrl->iplimit_stepper_cycles << PM_SHIFT_FROM_MASK(mask);

    err_flag |= pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, ibb_iplimit_startup_ctl_2, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_ibb_current_limit_cfg(uint8 pmic_index,
    pm_ibb_current_limit_cfg_type *current_limit_cfg
    )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0, curr_cfg = 0;
    pm_register_address_type ibb_current_limit = 0, ibb_sec_access = 0;
    pm_ibb_data_type *ibb_ptr = pm_ibb_get_data(pmic_index);

    if (NULL == ibb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (NULL == current_limit_cfg ||
             current_limit_cfg->ilimit_count >= PM_IBB_IPLIMIT_COUNT__INVALID ||
             current_limit_cfg->current_max > PM_IBB_CURRENT_MAX)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    ibb_sec_access = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->sec_access;
    ibb_current_limit = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->current_limit;

    mask = PM_BIT(7);
    data = current_limit_cfg->en_current_limit << PM_SHIFT_FROM_MASK(mask);

    if(current_limit_cfg->en_current_limit)
    {
        mask |= PM_BITS(6, 5);
        data |= current_limit_cfg->ilimit_count << PM_SHIFT_FROM_MASK(mask);

        mask |= PM_BITS(4, 0);

        /* Output Current can be set between 0mA to 1550mA. Ilim = 50*code */
        curr_cfg = current_limit_cfg->current_max / PM_IBB_CURRENT_STEP;
        if(current_limit_cfg->current_max % PM_IBB_CURRENT_STEP)
            curr_cfg += 1;
        data |= curr_cfg;
    }

    err_flag  = pm_comm_write_byte(ibb_ptr->comm_ptr->slave_id, ibb_sec_access, 0xA5, 0);

    err_flag |= pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, ibb_current_limit, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_ibb_ps_ctrl(uint8 pmic_index,
    boolean en_ps,
    pm_ibb_ps_threshold ps_threshold
    )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type ibb_ps_ctl = 0;
    pm_ibb_data_type *ibb_ptr = pm_ibb_get_data(pmic_index);

    if (NULL == ibb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (ps_threshold >= PM_IBB_PS_THRESHOLD__INVALID)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    ibb_ps_ctl = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->ps_ctl;

    mask  = PM_BIT(7);
    data  = en_ps << PM_SHIFT_FROM_MASK(mask);

    if(en_ps)
    {
        mask |= PM_BITS(2, 0);
        data |= ps_threshold << PM_SHIFT_FROM_MASK(mask);
    }

    err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, ibb_ps_ctl, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_ibb_vreg_ok_ctrl(uint8 pmic_index,
    pm_ibb_vreg_ok_ctrl_type *vreg_ok_ctrl
    )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type ibb_vreg_ok_ctl = 0;
    pm_ibb_data_type *ibb_ptr = pm_ibb_get_data(pmic_index);

    if (NULL == ibb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (NULL == vreg_ok_ctrl ||
             vreg_ok_ctrl->overload_deb >= PM_IBB_VREG_OK_OVERLOAD_DEB__INVALID ||
             vreg_ok_ctrl->vok_deb >= PM_IBB_VREG_OK_VOK_DEB__INVALID)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    ibb_vreg_ok_ctl = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->vreg_ok_ctl;

    mask  = PM_BIT(7);
    data  = vreg_ok_ctrl->en_overload_blank << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(6, 5);
    data |= vreg_ok_ctrl->overload_deb << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(1, 0);
    data |= vreg_ok_ctrl->vok_deb << PM_SHIFT_FROM_MASK(mask);

    err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, ibb_vreg_ok_ctl, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_ibb_swire_ctrl(uint8 pmic_index,
    boolean en_swire_vout_updt,
    uint32 op_voltage_at_one_edge
    )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0, vout_cfg = 0;
    pm_register_address_type ibb_swire_ctl = 0;
    pm_ibb_data_type *ibb_ptr = pm_ibb_get_data(pmic_index);

    if (NULL == ibb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (op_voltage_at_one_edge < PM_IBB_VOUT_MIN ||
             op_voltage_at_one_edge > PM_IBB_VOUT_MAX)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    ibb_swire_ctl = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->swire_ctl;

    mask  = PM_BIT(7);
    data  = en_swire_vout_updt << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(5, 0);

    /* Output voltage can be set between -400mV to -6300mV. Vout = -(100mV * code) */
    vout_cfg = op_voltage_at_one_edge / PM_IBB_VOUT_STEP;
    if(op_voltage_at_one_edge % PM_IBB_VOUT_STEP)
        vout_cfg += 1;
    data |= vout_cfg;

    err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, ibb_swire_ctl, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_ibb_slew_ctrl(uint8 pmic_index,
    pm_ibb_slew_ctrl_type *slew_ctrl
    )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type ibb_slew_ctl = 0;
    pm_ibb_data_type *ibb_ptr = pm_ibb_get_data(pmic_index);

    if (NULL == ibb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (NULL == slew_ctrl ||
             slew_ctrl->slew_rate_speed >= PM_IBB_SLEW_CTRL_SLEW_RATE_SPEED__INVALID ||
             slew_ctrl->trans_time_fast >= PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__INVALID ||
             slew_ctrl->trans_time_slow >= PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__INVALID)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    ibb_slew_ctl = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->output_slew_ctl;

    mask = PM_BIT(7);
    data = slew_ctrl->en_digital_slew_ctl << PM_SHIFT_FROM_MASK(mask);

    if(slew_ctrl->en_digital_slew_ctl)
    {
        mask |= PM_BIT(6);
        data |= slew_ctrl->slew_rate_speed << PM_SHIFT_FROM_MASK(mask);

        mask |= PM_BITS(5, 3);
        data |= slew_ctrl->trans_time_fast << PM_SHIFT_FROM_MASK(mask);

        mask |= PM_BITS(2, 0);
        data |= slew_ctrl->trans_time_slow << PM_SHIFT_FROM_MASK(mask);
    }

    err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, ibb_slew_ctl, mask, data, 0);

    return err_flag;
}
