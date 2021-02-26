/*! \file
*
*  \brief  pm_lab.c driver implementation.
*  \details charger driver implementation.
*  &copy;
*  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

when        who     what, where, why
--------    ---     ---------------------------------------------------------
12/02/16    ks      Added support for new IBB Type/Subtype
03/05/15    al      Adding API for setting ISENSE and PS_CTL
08/20/14    al      Updating comm lib
08/12/14    al      Mask and range fixes
07/03/14    al      Updating API to read LAB VREG status
06/20/14    al      Updated coypright info
05/20/14    al      Architecture update
05/29/14    al      Adding API to configure Vdisp
04/30/14    al      Initial revision
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_lab_driver.h"
#include "pm_lab.h"


/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/
#define PM_LAB_VOUT_MIN         4600    /* in mV */
#define PM_LAB_VOUT_MAX         6100    /* in mV */
#define PM_LAB_VOUT_STEP        100     /* in mV */

#define PM_LAB_CURRENT_MIN      200     /* in mA */
#define PM_LAB_CURRENT_MAX      1600    /* in mA */
#define PM_LAB_CURRENT_STEP     200     /* in mA */


/*===========================================================================

                        INTERNAL FUNCTION DEFINITION

===========================================================================*/
static inline pm_err_flag_type pm_lab_unlock_perph_write(pm_lab_data_type *lab_ptr);


/*===========================================================================

                        EXTERNAL FUNCTION DEFINITION

===========================================================================*/
pm_err_flag_type pm_lab_lcd_amoled_sel(uint32 device_index, boolean en_amoled_mode)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;

   pm_lab_data_type *lab_ptr  = pm_lab_get_data(device_index);

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type lcd_amoled_sel = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->lcd_amoled_sel;
      data = (en_amoled_mode)? 0x80 : 0x00;
      err_flag = pm_lab_unlock_perph_write(lab_ptr);
      err_flag |= pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, lcd_amoled_sel, 0x80, data,0);
   }

   return err_flag;
}


pm_err_flag_type pm_lab_lab_module_rdy(uint32 device_index, boolean lab_rdy)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;

   pm_lab_data_type *lab_ptr  = pm_lab_get_data(device_index);

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type module_rdy = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->module_rdy;
      data = (lab_rdy)? 0x80 : 0x00;
      err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, module_rdy, 0x80, data,0);
   }

   return err_flag;
}


pm_err_flag_type pm_lab_en_lab_module(uint32 device_index, boolean en_module)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;

   pm_lab_data_type *lab_ptr  = pm_lab_get_data(device_index);

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type enable_ctl = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->enable_ctl;
      data = (en_module)? 0x80 : 0x00;
      err_flag |= pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, enable_ctl, 0x80, data,0);
   }

   return err_flag;
}


pm_err_flag_type pm_lab_ibb_rdy_en(uint32 device_index, boolean ibb_en_rdy)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;

   pm_lab_data_type *lab_ptr  = pm_lab_get_data(device_index);

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type ibb_en_rdy_reg = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->ibb_en_rdy;
      data = (ibb_en_rdy)? 0x80 : 0x00;
      err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, ibb_en_rdy_reg, 0x80, data,0);
   }

   return err_flag;
}

pm_err_flag_type pm_lab_get_lab_status(uint32 device_index, pm_lab_status_type* lab_status)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data = 0, lab_sts[4] = {0};

   pm_lab_data_type *lab_ptr  = pm_lab_get_data(device_index);

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == lab_status)
   {
      err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else
   {
      uint8 lab_periph_rev = lab_ptr->periph_revision.periph_dig_major;
      memset(lab_status, 0 , sizeof(pm_lab_status_type));

      if(lab_periph_rev < 3)
      {
        pm_register_address_type lcd_amoled_sel = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->lcd_amoled_sel;
        err_flag |= pm_comm_read_byte(lab_ptr->comm_ptr->slave_id, lcd_amoled_sel, &data,0);
        lab_status->en_amoled_mode = (data & 0x80)? TRUE : FALSE;
      }

      pm_register_address_type module_rdy = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->module_rdy;
      err_flag = pm_comm_read_byte(lab_ptr->comm_ptr->slave_id, module_rdy, &data, 0);
      lab_status->lab_module_rdy =(data & 0x80)? TRUE : FALSE;

      pm_register_address_type enable_ctl = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->enable_ctl;
      err_flag |= pm_comm_read_byte(lab_ptr->comm_ptr->slave_id, enable_ctl, &data,0);
      lab_status->lab_module_en = (data & 0x80)? TRUE : FALSE;

      pm_register_address_type ibb_en_rdy_reg = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->ibb_en_rdy;
      err_flag |= pm_comm_read_byte(lab_ptr->comm_ptr->slave_id, ibb_en_rdy_reg, &data,0);
      lab_status->lab_ibb_en_rdy = (data & 0x80)? TRUE : FALSE;

      pm_register_address_type precharge_ctl = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->precharge_ctl;
      err_flag |= pm_comm_read_byte(lab_ptr->comm_ptr->slave_id, precharge_ctl, &data, 0);
      lab_status->en_fast_precharge = (data & 0x04)? TRUE : FALSE ;
      lab_status->max_prechg_time_usec = ((data & 0x03)*100) + 200;

      pm_register_address_type output_voltage = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->output_voltage;
      err_flag |= pm_comm_read_byte(lab_ptr->comm_ptr->slave_id, output_voltage, &data, 0);
      if(lab_periph_rev < 3)
      {
        lab_status->override_output_volt = (data & 0x80)? TRUE: FALSE;
        lab_status->output_volt = ((data& 0xF)*100) + 4600;
      }
      else
      {
        lab_status->output_volt = ((data& 0xF)*100) + 4600;
      }

      pm_register_address_type status1 = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->status1;
      err_flag |= pm_comm_read_byte_array(lab_ptr->comm_ptr->slave_id, status1, 4, lab_sts, 0);
      lab_status->lab_vreg_ok     = (lab_sts[0] & 0x80) ? TRUE: FALSE;
      lab_status->short_circuit   = (lab_sts[0] & 0x40) ? FALSE: TRUE; //if false then no short circuit else short circuit;
      if(lab_periph_rev < 3)
      {
        lab_status->lab_status_en = (lab_sts[0] & 0x20) ? TRUE: FALSE;
      }
      else
      {
        lab_status->bst_status = (lab_sts[0] & 0x20) ? TRUE : FALSE;
        lab_status->ldo_status = (lab_sts[0] & 0x1C) >> PM_SHIFT_FROM_MASK(0x1C);

        lab_status->ldo_softstart_done = (lab_sts[1] & 0x80) ? TRUE : FALSE;
        lab_status->ldo_over_current_detected = (lab_sts[1] & 0x40) ? TRUE : FALSE;
        lab_status->ldo_vreg_ok = (lab_sts[1] & 0x20) ? TRUE : FALSE;

        lab_status->bst_vset = PM_LAB_VOUT_MIN + (PM_LAB_VOUT_STEP * (lab_sts[2] & 0x0F));    /* Vout = 4600 + (100mV * code)  in mV */

        lab_status->fault_shutdown_sts = (lab_sts[3] & 0x80) ? TRUE : FALSE;
      }

      pm_register_address_type current_sense = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->current_sense;
      err_flag |= pm_comm_read_byte(lab_ptr->comm_ptr->slave_id, current_sense, &data, 0);
      lab_status->isense_tap  = (data & 0x0C) >> PM_SHIFT_FROM_MASK(0x0C);
      lab_status->isense_gain = (data & 0x03);

      pm_register_address_type ps_ctl = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->ps_ctl;
      err_flag |= pm_comm_read_byte(lab_ptr->comm_ptr->slave_id, ps_ctl, &data, 0);
      lab_status->ps_threshold = ((data & 0x3)*10) + 50;
      if(lab_periph_rev < 3)
      {
        lab_status->select_ps_table = (data & 0x04) ? TRUE: FALSE;
        lab_status->select_digital_ps = (data & 0x08) ? TRUE: FALSE;
      }
      lab_status->enable_ps = (data & 0x80) ? TRUE: FALSE;
   }

   return err_flag ;
}


pm_err_flag_type pm_lab_config_precharge_ctrl(uint32 device_index, boolean en_fast_precharge, uint32 max_prechg_time_usec)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;

   pm_lab_data_type *lab_ptr  = pm_lab_get_data(device_index);

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type precharge_ctl = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->precharge_ctl;
      /*range for max_prechg_time_usec 200 to 400*/
      max_prechg_time_usec = (max_prechg_time_usec < 200)? 200 : max_prechg_time_usec;
      max_prechg_time_usec = (max_prechg_time_usec>400)? 0x3 : ((max_prechg_time_usec-200)/100);
      data = (pm_register_data_type)((en_fast_precharge<<2) | max_prechg_time_usec);
      err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, precharge_ctl, 0x07, data,0);
   }

   return err_flag;
}


pm_err_flag_type pm_lab_config_output_volt(uint32 device_index, boolean override_output_volt, uint32 volt_millivolt)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;

   pm_lab_data_type *lab_ptr  = pm_lab_get_data(device_index);

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      uint8 lab_periph_rev = lab_ptr->periph_revision.periph_dig_major;
      pm_register_address_type output_voltage = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->output_voltage;

      if(lab_periph_rev < 3)
      {
        /*limit is 4600 to 6100*/
        volt_millivolt = (volt_millivolt < 4600)? 4600 : volt_millivolt;

        volt_millivolt = (volt_millivolt > 6100) ? 0xF : ((volt_millivolt - 4600)/100);

        data = (pm_register_data_type)((override_output_volt << 7) | (0xF & volt_millivolt));

        err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, output_voltage, 0x8F, data,0);
      }
      else
      {
        pm_register_data_type mask = 0;

        if(volt_millivolt < PM_LAB_VOUT_MIN ||
           volt_millivolt > PM_LAB_VOUT_MAX)
        {
          return PM_ERR_FLAG_INVALID_PARAMETER;
        }

        mask = PM_BITS(3, 0);

        /* Output voltage can be set between 4600mV to 6100mV. Vout = 4600 + (100mV * code) */
        volt_millivolt -= PM_LAB_VOUT_MIN;
        data = volt_millivolt / PM_LAB_VOUT_STEP;
        if(volt_millivolt % PM_LAB_VOUT_STEP)
          data += 1;

        err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, output_voltage, mask, data, 0);
      }

   }

   return err_flag;
}



pm_err_flag_type pm_lab_config_current_sense(uint32 device_index, pm_lab_isense_type isense_tap, pm_lab_isense_type isense_gain)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;
   uint32 tap_isense = 0;
   uint32 gain_isense = 0;

   pm_lab_data_type *lab_ptr  = pm_lab_get_data(device_index);

   if (isense_tap >= PM_LAB_ISENSE_INVALID || isense_gain >= PM_LAB_ISENSE_INVALID)
   {
      err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      switch (isense_tap)
      {
        case PM_LAB_ISENSE_0P8: tap_isense = 0x0 ; break;
        case PM_LAB_ISENSE_0P9: tap_isense = 0x1 ; break;
        case PM_LAB_ISENSE_1P0: tap_isense = 0x2 ; break;
        case PM_LAB_ISENSE_1P1: tap_isense = 0x3 ; break;
        default: tap_isense =  0x1; //default value
      }

      switch (isense_gain)
      {
        case PM_LAB_ISENSE_0P5: gain_isense = 0x0 ; break;
        case PM_LAB_ISENSE_1P0: gain_isense = 0x1 ; break;
        case PM_LAB_ISENSE_1P5: gain_isense = 0x2 ; break;
        case PM_LAB_ISENSE_2P0: gain_isense = 0x3 ; break;
        default: gain_isense =  0x1; //default value
      }

      data = (tap_isense <<2)|gain_isense;

      pm_register_address_type current_sense = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->current_sense;

      err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, current_sense, 0x0F,data,0);
   }

   return err_flag;
}

pm_err_flag_type pm_lab_config_pulse_skip_ctrl(uint32 device_index, uint32 ps_threshold, boolean select_ps_table, boolean select_digital_ps, boolean enable_ps)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data= 0x00;

   pm_lab_data_type *lab_ptr  = pm_lab_get_data(device_index);

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      uint8 lab_periph_rev = lab_ptr->periph_revision.periph_dig_major;
      pm_register_address_type ps_ctl = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->ps_ctl;

      if (ps_threshold > 80)
      {
         ps_threshold = 80;
      }
      else if (ps_threshold < 50)
      {
        ps_threshold = 50;
      }

      /*0=50, 1=60, 2=70, 3=80*/
      ps_threshold = (ps_threshold - 50) / 10;

      if(lab_periph_rev < 3)
      {
        data = (enable_ps << 7) | (select_digital_ps << 3) | (select_ps_table << 2) | ps_threshold ;
        err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, ps_ctl, 0x8F, data, 0);
      }
      else
      {
        data = (enable_ps << 7) | ps_threshold;
        err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, ps_ctl, 0x83, data, 0);
      }
   }

   return err_flag;
}

static inline pm_err_flag_type pm_lab_unlock_perph_write(pm_lab_data_type *lab_ptr)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type sec_access = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->sec_access;
      err_flag = pm_comm_write_byte(lab_ptr->comm_ptr->slave_id, sec_access, 0xA5, 0);
   }

   return err_flag;
}


/* For LAB 3.0/+ Revision */
pm_err_flag_type pm_lab_get_rt_status(uint8 pmic_index,
    pm_lab_rt_status_type *rt_status
    )
{
    pm_err_flag_type err_flag  = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0;
    pm_register_address_type lab_int_rt_sts = 0;
    pm_lab_data_type *lab_ptr = pm_lab_get_data(pmic_index);

    if (NULL == lab_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (NULL == rt_status)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    lab_int_rt_sts = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->int_rt_sts;

    err_flag = pm_comm_read_byte(lab_ptr->comm_ptr->slave_id, lab_int_rt_sts, &data, 0);

    if(PM_ERR_FLAG__SUCCESS == err_flag)
    {
        rt_status->sc_error = (PM_BIT(1) & data) ? TRUE : FALSE;
        rt_status->vreg_not_ok = (PM_BIT(0) & data) ? TRUE : FALSE;
    }

    return err_flag;
}

pm_err_flag_type pm_lab_swire_ctrl(uint8 pmic_index,
    pm_lab_swire_pgm_ctl swire_ctl,
    boolean enable
    )
{
    pm_err_flag_type err_flag  = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type mask = 0, data = 0;
    pm_register_address_type lab_swire_pgm_ctl = 0;
    pm_lab_data_type *lab_ptr = pm_lab_get_data(pmic_index);

    if (NULL == lab_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    lab_swire_pgm_ctl = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->swire_pgm_ctl;

    switch(swire_ctl)
    {
        case PM_LAB_SWIRE_CTL__EN_VOUT:
            mask = PM_BIT(7);
            data = enable << PM_SHIFT_FROM_MASK(mask);
            break;
        case PM_LAB_SWIRE_CTL__EN_PD:
            mask = PM_BIT(6);
            data = enable << PM_SHIFT_FROM_MASK(mask);
            break;
        default :
            return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, lab_swire_pgm_ctl, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_lab_set_default_voltage(uint8 pmic_index,
    uint32 op_voltage_mv
    )
{
    pm_err_flag_type err_flag  = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type mask = 0, data = 0;
    pm_register_address_type lab_op_voltage = 0;
    pm_lab_data_type *lab_ptr = pm_lab_get_data(pmic_index);

    if (NULL == lab_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(op_voltage_mv < PM_LAB_VOUT_MIN ||
            op_voltage_mv > PM_LAB_VOUT_MAX)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    lab_op_voltage = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->vout_default;

    mask = PM_BITS(3, 0);

    /* Output voltage can be set between 4600mV to 6100mV. Vout = 4600 + (100mV * code) */
    op_voltage_mv -= PM_LAB_VOUT_MIN;
    data = op_voltage_mv / PM_LAB_VOUT_STEP;
    if(op_voltage_mv % PM_LAB_VOUT_STEP)
        data += 1;

    err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, lab_op_voltage, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_lab_current_limit_cfg(uint8 pmic_index,
    boolean en_current_limit,
    uint32 current_max
    )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0, curr_cfg = 0;
    pm_register_address_type lab_current_limit = 0;
    pm_lab_data_type *lab_ptr = pm_lab_get_data(pmic_index);

    if (NULL == lab_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (current_max < PM_LAB_CURRENT_MIN ||
             current_max > PM_LAB_CURRENT_MAX)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    lab_current_limit = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->current_limit;

    mask = PM_BIT(7);
    data = en_current_limit << PM_SHIFT_FROM_MASK(mask);

    if(en_current_limit)
    {
        mask |= PM_BITS(2, 0);

        /* Output voltage can be set between 0mA to 1600mA. Ilim = 200mA * (code+1) */
        curr_cfg = current_max / PM_LAB_CURRENT_STEP;
        if(current_max % PM_LAB_CURRENT_STEP)
            curr_cfg += 1;
        data |= curr_cfg;
    }

    err_flag |= pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, lab_current_limit, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_lab_sw_high_psrr_ctrl(uint8 pmic_index,
    pm_lab_sw_high_psrr_ctl sw_high_psrr_ctl,
    boolean enable
    )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type lab_sw_high_psrr_ctl = 0;
    pm_lab_data_type *lab_ptr = pm_lab_get_data(pmic_index);

    if (NULL == lab_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    lab_sw_high_psrr_ctl = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->sw_high_psrr_ctl;

    switch(sw_high_psrr_ctl)
    {
        case PM_LAB_SW_HIGH_PSRR_CTL__ENABLE_OVERRIDE:
            mask = PM_BIT(7);
            data = enable << PM_SHIFT_FROM_MASK(mask);
            break;
        case PM_LAB_SW_HIGH_PSRR_CTL__ENABLE_FORCE:      /* Works only if bit 7 is set to 1 : SW Override Enabled */
            mask  = PM_BIT(7);
            data  = enable << PM_SHIFT_FROM_MASK(mask);
            mask |= PM_BIT(0);
            data |= enable << PM_SHIFT_FROM_MASK(mask);
            break;
        default :
            return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, lab_sw_high_psrr_ctl, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_lab_ldo_sw_dbg_ctrl(uint8 pmic_index,
    boolean en_sw_ldo_dbg_mode,
    pm_lab_sw_ldo_dbg_mode sw_ldo_dbg_mode
    )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type lab_ldo_sw_dbg_ctl = 0;
    pm_lab_data_type *lab_ptr = pm_lab_get_data(pmic_index);

    if (NULL == lab_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    lab_ldo_sw_dbg_ctl = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->ldo_sw_dbg_ctl;

    mask = PM_BIT(7);
    data = en_sw_ldo_dbg_mode << PM_SHIFT_FROM_MASK(mask);

    if(en_sw_ldo_dbg_mode)
    {
        switch(sw_ldo_dbg_mode)
        {
            case PM_LAB_SW_LDO_DBG_MODE__VBIAS_EN:
                mask |= PM_BIT(5);
                data |= 1 << PM_SHIFT_FROM_MASK(mask);
                break;
            case PM_LAB_SW_LDO_DBG_MODE__OCP_EN:
                mask |= PM_BIT(4);
                data |= 1 << PM_SHIFT_FROM_MASK(mask);
                break;
            case PM_LAB_SW_LDO_DBG_MODE__NPM_EN:
                mask |= PM_BIT(3);
                data |= 1 << PM_SHIFT_FROM_MASK(mask);
                break;
            case PM_LAB_SW_LDO_DBG_MODE__BYP_EN:
                mask |= PM_BIT(0);
                data |= 1 << PM_SHIFT_FROM_MASK(mask);
                break;
            default:
                return PM_ERR_FLAG_INVALID_PARAMETER;
        }
    }

    err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, lab_ldo_sw_dbg_ctl, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_lab_pd_ctrl(uint8 pmic_index,
    boolean enable
    )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type lab_ldo_pd_ctl = 0;
    pm_lab_data_type *lab_ptr = pm_lab_get_data(pmic_index);

    if (NULL == lab_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    lab_ldo_pd_ctl = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->ldo_pd_ctl;

    mask = PM_BIT(7);
    data = enable << PM_SHIFT_FROM_MASK(mask);

    err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, lab_ldo_pd_ctl, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_lab_vph_envelop_ctrl(uint8 pmic_index,
    pm_lab_vph_vref_high_psrr_sel vref_high_psrr_sel,
    pm_lab_vph_hw_high_psrr_src_sel hw_high_psrr_src_sel
    )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type lab_vph_envelop_ctl = 0;
    pm_lab_data_type *lab_ptr = pm_lab_get_data(pmic_index);

    if (NULL == lab_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(vref_high_psrr_sel >= PM_LAB_VPH_VREF_HIGH_PSRR_SEL__INVALID ||
            hw_high_psrr_src_sel > PM_LAB_VPH_HW_SRC_SEL__INVALID)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    lab_vph_envelop_ctl = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->vph_envelop_ctl;

    mask  = PM_BITS(7, 6);
    data  = vref_high_psrr_sel << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(1, 0);
    data |= hw_high_psrr_src_sel << PM_SHIFT_FROM_MASK(mask);

    err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, lab_vph_envelop_ctl, mask, data, 0);

    return err_flag;
}
