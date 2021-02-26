/*! \file
*
*  \brief  pm_schg_chgr.c driver implementation.
*  \details charger driver implementation.
*  &copy;
*  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE


when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/21/17    czq     Change allowed max jeita_cccomp to 4500mA
12/19/16    sm     Added API to get SCHG Charger PmicIndex
05/17/16    aab    Updated pm_schg_chgr_set_chgr_cmd_type()
03/28/16    va     Updated Jeit CcComp Api
03/01/16    sm     Added pm_schg_chgr_jeita_en_cfg()
02/16/16    va     Adding required API for enanlbing charger
04/18/14    al     Initial revision
========================================================================== */

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_schg_chgr.h"
#include "pm_schg_driver.h"

/*===========================================================================
                        MACRO DECLARATIONS
===========================================================================*/

#define ADDRESS(x) (schg_ptr->schg_register->chgr_register->base_address + schg_ptr->schg_register->chgr_register->x)
#define SLAVE_ID   (schg_ptr->comm_ptr->slave_id)
#define PM_SCHG_FLOAT_VOLTAGE_STEP_CONSTANT     75

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

static inline pm_err_flag_type pm_schg_chgr_unlock_perph_write(pm_schg_data_type *schg_ptr)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

   if (NULL == schg_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type sec_access = ADDRESS(sec_access);
      err_flag = pm_comm_write_byte(SLAVE_ID, sec_access, 0xA5, 0);
   }

   return err_flag;
}


pm_err_flag_type pm_schg_chgr_irq_enable(uint32 device_index, pm_schg_chgr_irq_type irq, boolean enable)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_address_type irq_reg;
   pm_register_data_type data = 1 << irq;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

   if (NULL == schg_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SCHG_CHGR_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else
   {
      if (enable)
      {
         irq_reg = ADDRESS(int_en_set);
      }
      else
      {
         irq_reg = ADDRESS(int_en_clr);
      }

      err_flag = pm_comm_write_byte(SLAVE_ID, irq_reg, data, 0);
   }
   return err_flag;
}

pm_err_flag_type pm_schg_chgr_irq_clear(uint32  device_index, pm_schg_chgr_irq_type irq)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data = 1 << irq;

   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

   if (NULL == schg_ptr)
   {
      err_flag =  PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SCHG_CHGR_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type int_latched_clr = ADDRESS(int_latched_clr);
      err_flag = pm_comm_write_byte(SLAVE_ID, int_latched_clr, data, 0);
   }

   return err_flag;
}


pm_err_flag_type pm_schg_chgr_irq_set_trigger(uint32 device_index, pm_schg_chgr_irq_type irq, pm_irq_trigger_type trigger)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   uint8 mask = 1 << irq;
   pm_register_data_type set_type, polarity_high, polarity_low;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

   if (NULL == schg_ptr)
   {
      err_flag =  PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SCHG_CHGR_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else
   {
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
         return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
      }

      pm_register_address_type int_set_type = ADDRESS(int_set_type);
      pm_register_address_type int_polarity_high = ADDRESS(int_polarity_high);
      pm_register_address_type int_polarity_low = ADDRESS(int_polarity_low);

      err_flag = pm_comm_write_byte_mask(SLAVE_ID, int_set_type, mask, set_type, 0);
      err_flag |= pm_comm_write_byte_mask(SLAVE_ID, int_polarity_high, mask, polarity_high, 0);
      err_flag |= pm_comm_write_byte_mask(SLAVE_ID, int_polarity_low, mask, polarity_low, 0);
   }

   return err_flag;
}

pm_err_flag_type pm_schg_chgr_irq_status(uint32 device_index, pm_schg_chgr_irq_type irq, pm_irq_status_type type, boolean *status)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;
   uint8 mask = 1 << irq;
   pm_register_address_type int_sts;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

   if (NULL == schg_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SCHG_CHGR_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else if (NULL == status)
   {
      err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else
   {
      switch (type)
      {
      case PM_IRQ_STATUS_RT:
         int_sts = ADDRESS(int_rt_sts);
         break;
      case PM_IRQ_STATUS_LATCHED:
         int_sts = ADDRESS(int_latched_sts);
         break;
      case PM_IRQ_STATUS_PENDING:
         int_sts = ADDRESS(int_pending_sts);
         break;
      default:
         return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
      }

      err_flag = pm_comm_read_byte_mask(SLAVE_ID, int_sts, mask, &data, 0);
      *status = data ? TRUE : FALSE;
   }

   return err_flag;
}


pm_err_flag_type pm_schg_chgr_set_charge_current(uint32 device_index, pm_schg_chgr_current_type charge_current_type, uint32 current)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type    data = 0;
   pm_register_data_type    mask = 0;
   pm_register_address_type reg = 0;
   uint32 step_size = 0;
   uint32 max_value = 0;
   uint32 min_value = 0;

   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

   if (NULL == schg_ptr)
   {
      err_flag =  PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (charge_current_type >= PM_SCHG_CHGR_CURRENT_TYPE_INVALID)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      switch (charge_current_type)
      {
      case PM_SCHG_CHGR_PRE_CHARGE_CURRENT:
         reg = ADDRESS(pre_charge_current_cfg);
         mask = 0x3F;
         step_size = 25;
         max_value = 1575;
         min_value = 0;
         break;

      case PM_SCHG_CHGR_FAST_CHARGE_CURRENT:
         reg = ADDRESS(fast_charge_current_cfg);
         mask = 0xFF;
         step_size = 25;
         max_value = 4500;
         min_value = 0;
         break;

      case PM_SCHG_CHGR_TERMINATION_CURRENT:
         reg = ADDRESS(tccc_charge_current_termination_cfg);
         mask = 0x0F;
         step_size = 50;
         max_value = 750;
         min_value = 0;
         break;

      default:
         return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
         break;
      }

      if (current < min_value)
      {
         current = min_value;
      }
      else if (current > max_value)
      {
         current = max_value;
      }

      /*rounding and getting corresponding register value*/
      data = (current+(step_size/2))/step_size;

      err_flag = pm_comm_write_byte_mask(SLAVE_ID, reg, mask, data, 0);
   }

   return err_flag;
}

pm_err_flag_type pm_schg_chgr_get_charge_current(uint32 device_index, pm_schg_chgr_current_type current_type, uint32 *current)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type    data = 0;
   pm_register_data_type    mask = 0;
   pm_register_address_type reg = 0;
   uint32 step_size = 0;
   uint32 min_value = 0;

   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

   if (NULL == schg_ptr || NULL == current)
   {
      err_flag =  PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (current_type >= PM_SCHG_CHGR_CURRENT_TYPE_INVALID)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      switch (current_type)
      {
      case PM_SCHG_CHGR_PRE_CHARGE_CURRENT:
         reg = ADDRESS(pre_charge_current_cfg);
         mask = 0x3F;
         step_size = 25;
         min_value = 0;
         break;

      case PM_SCHG_CHGR_FAST_CHARGE_CURRENT:
         reg = ADDRESS(fast_charge_current_cfg);
         mask = 0xFF;
         step_size = 25;
         min_value = 0;
         break;

      case PM_SCHG_CHGR_TERMINATION_CURRENT:
         reg = ADDRESS(tccc_charge_current_termination_cfg);
         mask = 0x0F;
         step_size = 50;
         min_value = 0;
         break;

      default:
         return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
      }

      err_flag = pm_comm_read_byte(SLAVE_ID, reg, &data, 0);

      *current = min_value + ((data & mask) * step_size);
   }

   return err_flag;
}


pm_err_flag_type pm_schg_chgr_set_chgr_cfg(uint32 device_index, pm_schg_chgr_chgr_cgf_type chgr_cfg, boolean set_value)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type    data = (set_value) ? 0xFF : 0x00;
   pm_register_data_type    mask = 0;
   pm_register_address_type reg = 0;

   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

   if (NULL == schg_ptr)
   {
      err_flag =  PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (chgr_cfg >= PM_SCHG_CHGR_CHGR_CFG_INVALID)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      switch (chgr_cfg)
      {
      case PM_SCHG_CHGR_CHGR_CFG_INCREASE_RCHG_TIMEOUT_CFG:
         mask = 0x1 << 1;
         reg = ADDRESS(chgr_cfg1);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_LOAD_BAT:
         mask = 0x1 << 0;
         reg = ADDRESS(chgr_cfg1);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_CHG_EN_SRC:
         mask = 0x1 << 7;
         reg = ADDRESS(chgr_cfg2);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_CHG_EN_POLARITY:
         mask = 0x1 << 6;
         reg = ADDRESS(chgr_cfg2);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_PRETOFAST_TRANSITION_CFG:
         mask = 0x1 << 5;
         reg = ADDRESS(chgr_cfg2);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_BAT_OV_ECC:
         mask = 0x1 << 4;
         reg = ADDRESS(chgr_cfg2);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_I_TERM:
         mask = 0x1 << 3;
         reg = ADDRESS(chgr_cfg2);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_AUTO_RECHG:
         mask = 0x1 << 2;
         reg = ADDRESS(chgr_cfg2);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_EN_ANALOG_DROP_IN_VBATT:
         mask = 0x1 << 1;
         reg = ADDRESS(chgr_cfg2);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_CHARGER_INHIBIT:
         mask = 0x1 << 0;
         reg = ADDRESS(chgr_cfg2);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_CHG_ENB_TIMEOUT_SETTING:
         mask = 0x1 << 1;
         reg = ADDRESS(charger_enable_cfg);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_FORCE_ZERO_CFG:
         mask = 0x1 << 0;
         reg = ADDRESS(charger_enable_cfg);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_CHG_OPTION_PIN_TRIM:
         mask = 0x1 << 7;
         reg = ADDRESS(cfg);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_BATN_SNS_CFG:
         mask = 0x1 << 4;
         reg = ADDRESS(cfg);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_CFG_TAPER_DIS_AFVC:
         mask = 0x1 << 3;
         reg = ADDRESS(cfg);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_BATFET_SHUTDOWN_CFG:
         mask = 0x1 << 2;
         reg = ADDRESS(cfg);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_VDISCHG_EN_CFG:
         mask = 0x1 << 1;
         reg = ADDRESS(cfg);
         break;
      case PM_SCHG_CHGR_CHGR_CFG_VCHG_EN_CFG:
         mask = 0x1 << 0;
         reg = ADDRESS(cfg);
         break;
      default:
         return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
      }

      err_flag = pm_comm_write_byte_mask(SLAVE_ID, reg, mask, data, 0);
   }

   return err_flag;
}


pm_err_flag_type pm_schg_chgr_set_chgr_cmd_type(uint32 device_index, pm_schg_chgr_cmd_type chgr_cmd, boolean set_value)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type    data = (set_value) ? 0x01 : 0x00;
   pm_register_data_type    mask = 0x01;
   pm_register_address_type reg = 0;

   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

   if (NULL == schg_ptr)
   {
      err_flag =  PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (chgr_cmd >= PM_SCHG_CHGR_CMD_INVALID)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      switch (chgr_cmd)
      {
      case PM_SCHG_CHGR_CMD_CHARGING_ENABLE:
         reg = ADDRESS(charging_enable_cmd);
         err_flag |=pm_schg_chgr_set_chgr_cfg(device_index, PM_SCHG_CHGR_CHGR_CFG_CHG_EN_SRC, FALSE);
         err_flag |=pm_schg_chgr_set_chgr_cfg(device_index, PM_SCHG_CHGR_CHGR_CFG_CHG_EN_POLARITY, FALSE);
         break;

      case PM_SCHG_CHGR_CMD_ALLOW_FAST_CHARGING:
         reg = ADDRESS(allow_fast_charging_cmd);
         break;

      case PM_SCHG_CHGR_CMD_QNOVO_PT_ENABLE:
         reg = ADDRESS(qnovo_pt_enable_cmd);
         break;

      default:
         return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
         break;
      }

      err_flag |= pm_comm_write_byte_mask(SLAVE_ID, reg, mask, data, 0);

   }

   return err_flag;
}

pm_err_flag_type pm_schg_chgr_get_chgr_status(uint32 device_index, pm_schg_chgr_status_type *charger_status)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;

   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
   /*between status 2 and status 3 there is gap and hence breaking*/
   pm_register_data_type data[6]={0,0,0,0,0,0};

   if (NULL == schg_ptr || NULL == charger_status)
   {
      err_flag =  PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type status1 = ADDRESS(battery_charger_status_1);
      /*read 2 status register first and then rest 6 in other attempt*/
      err_flag = pm_comm_read_byte_array(SLAVE_ID, status1, 2, data, 0);
      /*status1 related*/
      charger_status->status_1.battery_charger_status = (pm_schg_battery_charger_status)(data[0] & 0x7);
      charger_status->status_1.step_charging_status = (data[0] & 0x38)>> 3;
      charger_status->status_1.cc_soft_terminate    = (data[0] & 0x40) ? TRUE : FALSE;
      charger_status->status_1.bvr_initial_ramp     = (data[0] & 0x80) ? TRUE : FALSE;
      /*status2 related*/
      charger_status->status_2.bat_temp_status_too_cold                = (data[1] & 0x01) ? TRUE : FALSE;
      charger_status->status_2.bat_temp_status_too_hot                 = (data[1] & 0x02) ? TRUE : FALSE;
      charger_status->status_2.bat_temp_status_cold_soft_limit         = (data[1] & 0x04) ? TRUE : FALSE;
      charger_status->status_2.bat_temp_status_hot_soft_limit          = (data[1] & 0x08) ? TRUE : FALSE;
      charger_status->status_2.charger_error_status_bat_term_missing   = (data[1] & 0x10) ? TRUE : FALSE;
      charger_status->status_2.charger_error_status_bat_ov             = (data[1] & 0x20) ? TRUE : FALSE;
      charger_status->status_2.charger_error_status_sft_expire         = (data[1] & 0x40) ? TRUE : FALSE;
      charger_status->status_2.input_current_limited                   = (data[1] & 0x80) ? TRUE : FALSE;

      /*read reset of 6 status registers*/
      DALSYS_memset(data, 0, sizeof(data));

      pm_register_address_type status3 = ADDRESS(battery_charger_status_3);
      err_flag |= pm_comm_read_byte_array(SLAVE_ID, status3, 6, data, 0);
      /*status3 related*/
      charger_status->status_3 = data[0];
      /*status4 related*/
      charger_status->status_4 = data[1];
      /*status 5 related*/
      charger_status->status_5.taper_region                 = (data[2] & 0x01) ? TRUE : FALSE;
      charger_status->status_5.enable_taper_sensor          = (data[2] & 0x02) ? TRUE : FALSE;
      charger_status->status_5.enable_chg_sensors           = (data[2] & 0x04) ? TRUE : FALSE;
      charger_status->status_5.taper                        = (data[2] & 0x08) ? TRUE : FALSE;
      charger_status->status_5.charging_enable              = (data[2] & 0x10) ? TRUE : FALSE;
      charger_status->status_5.force_zero_charge_current    = (data[2] & 0x20) ? TRUE : FALSE;
      charger_status->status_5.disable_charging             = (data[2] & 0x40) ? TRUE : FALSE;
      charger_status->status_5.valid_input_power_source     = (data[2] & 0x80) ? TRUE : FALSE;
      /*status 6 related*/
      charger_status->status_6 = data[3];
      /*status 7 related*/
       charger_status->status_7.cold_sl_adc           = (data[4] & 0x01) ? TRUE : FALSE;
       charger_status->status_7.hot_sl_adc            = (data[4] & 0x02) ? TRUE : FALSE;
       charger_status->status_7.too_hot_adc           = (data[4] & 0x04) ? TRUE : FALSE;
       charger_status->status_7.too_cold_adc          = (data[4] & 0x08) ? TRUE : FALSE;
       charger_status->status_7.enable_fullon_mode    = (data[4] & 0x10) ? TRUE : FALSE;
       charger_status->status_7.enable_fast_charging  = (data[4] & 0x20) ? TRUE : FALSE;
       charger_status->status_7.enable_pre_charging   = (data[4] & 0x40) ? TRUE : FALSE;
       charger_status->status_7.enable_trickle        = (data[4] & 0x80) ? TRUE : FALSE;
       /*status 8 related*/
       charger_status->status_8 = data[5];
   }

   return err_flag;
}

/*This API configures the floating voltage. Valid range is 3600mV to 4500 mv*/
pm_err_flag_type pm_schg_chgr_set_float_volt(uint32 device_index, uint32 float_volt_mv)
{
  pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0x00;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type fv_cfg = ADDRESS(float_voltage_cfg);
    if (float_volt_mv < 3487)
    {
      data = 0x0;
    }
    else if (float_volt_mv >= 4920)
    {
      data = 0xBF;
    }
    else
    {
      /* Float voltage setting = 3.4875V + (DATA x 7.5mV) */
      data =(((float_volt_mv * 10 ) - 34875) / PM_SCHG_FLOAT_VOLTAGE_STEP_CONSTANT);
    }
    err_flag = pm_schg_chgr_unlock_perph_write(schg_ptr);
    err_flag |= pm_comm_write_byte(SLAVE_ID, fv_cfg, data, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_schg_chgr_get_float_volt_cfg(uint32 device_index, uint32 *float_volt_mv)
{
  pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == float_volt_mv)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type fv_cfg = ADDRESS(float_voltage_cfg);
    err_flag = pm_comm_read_byte(SLAVE_ID, fv_cfg, &data, 0);
    if (data >= 0xBF)
    {
       *float_volt_mv = 4920;
    }
    else
    {
      /* Float voltage setting = 3.4875V + (DATA x 7.5mV) */
      *float_volt_mv = ((data * PM_SCHG_FLOAT_VOLTAGE_STEP_CONSTANT ) + 34875 ) /10;
    }
  }

  return err_flag;
}


/*This API sets the pre-charge to full charger threshold. Valid range is 2400mV to 3000mV*/
pm_err_flag_type pm_schg_chgr_set_p2f_threshold(uint32 device_index, pm_schg_chgr_p2f_thrd_cfg p2f_thrd_cfg)

{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
   if (NULL == schg_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if(p2f_thrd_cfg >= PM_SCHG_CHGR_P2F_INVALID)
   {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else
  {
      pm_register_address_type cfg_p2f = ADDRESS(pre_to_fast_charge_threshold_cfg);
      err_flag = pm_schg_chgr_unlock_perph_write(schg_ptr);
      err_flag |= pm_comm_write_byte_mask(SLAVE_ID, cfg_p2f, (1 << p2f_thrd_cfg), (1 << p2f_thrd_cfg), 0);
   }

   return err_flag;
}

/*This API reads the pre-charge to full charger threshold. Valid range is 2400mV to 3000mV*/
pm_err_flag_type pm_schg_chgr_get_p2f_threshold(uint32 device_index, pm_schg_chgr_p2f_thrd_cfg *p2f_thrd_cfg)
{
  pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == p2f_thrd_cfg)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type cfg_p2f = ADDRESS(pre_to_fast_charge_threshold_cfg);
    err_flag = pm_comm_read_byte_mask(SLAVE_ID, cfg_p2f, 0x3, &data, 0);
    *p2f_thrd_cfg = (pm_schg_chgr_p2f_thrd_cfg)data;
  }

  return err_flag;
}


/* This function gets HW JEITA related parameters status */
pm_err_flag_type pm_schg_chgr_get_jeita_cfg(uint32 device_index, pm_schg_chgr_jeita_cfg_type jeita_cfg, boolean *enable)
{
  pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (PM_SCHG_CHGR_JEITA_INVALID == jeita_cfg)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type jeita_en_cfg = ADDRESS(jeita_en_cfg);
    err_flag = pm_comm_read_byte(SLAVE_ID, jeita_en_cfg, &data, 0);
    *enable = (data & (0x1 << jeita_cfg)) ? TRUE : FALSE;
  }

  return err_flag;
}


/*This api sets floating voltage conpmensation code. Valid value is from 3600 to 4380 mV*/
pm_err_flag_type pm_schg_chgr_set_jeita_fvcomp_cfg(uint32 device_index, uint32 fv_comp)
{
  pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  uint32 float_volt_mv = 0;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type fv_cmp_cfg = ADDRESS(jeita_fvcomp_cfg);
    err_flag |= pm_schg_chgr_get_float_volt_cfg(device_index, &float_volt_mv);
    if(fv_comp > 472)
    {
      return PM_ERR_FLAG_INVALID_PARAMETER;
    }
    /* 7.5.mv = 0x01 */
    data = (fv_comp * 10)/ PM_SCHG_FLOAT_VOLTAGE_STEP_CONSTANT;
    err_flag = pm_schg_chgr_unlock_perph_write(schg_ptr);
    err_flag |= pm_comm_write_byte(SLAVE_ID, fv_cmp_cfg, data, 0);
  }

  return err_flag;
}


/*This api reads floating voltage conpmensation code settings. Valid value is from 0 to 63*/
pm_err_flag_type pm_schg_chgr_get_jeita_fvcomp_cfg(uint32 device_index, uint32 *fv_comp)
{
  pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == fv_comp)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type fv_cmp_cfg = ADDRESS(jeita_fvcomp_cfg);
    err_flag = pm_comm_read_byte(SLAVE_ID, fv_cmp_cfg, &data, 0);
    *fv_comp = ( data * PM_SCHG_FLOAT_VOLTAGE_STEP_CONSTANT)/10;
  }

  return err_flag;
}


/*This API configures the fast-charger current compensation value in milliamp. Valid values are 0 to 1575 mA */
pm_err_flag_type pm_schg_chgr_set_jeita_ccomp(uint32 device_index, uint32 fast_chg_i_cmp_ma)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if(fast_chg_i_cmp_ma > 4500)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }else
  {
    pm_register_address_type fcc_cmp_cfg = ADDRESS(jeita_cccomp_cfg);

    /* JEITA Charge Current Compensation when in battery temperature soft-limit
    JEITA CC = min(Pre Charge Current, Fast Charge Current - DATA x 25mA) if in pre-charge
    JEITA CC = Fast Charge Current - DATA x 25mA if in fast-charge */

    data = (fast_chg_i_cmp_ma + (25/12))/25;

    err_flag |= pm_comm_write_byte(SLAVE_ID, fcc_cmp_cfg, data, 0);
  }

  return err_flag;
}

/*This api reads floating voltage conpmensation code settings. Valid value is from 0 to 63*/
pm_err_flag_type pm_schg_chgr_get_jeita_ccomp_cfg(uint32 device_index, uint32 *fc_comp)
{
  pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == fc_comp)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    /* JEITA Charge Current Compensation when in battery temperature soft-limit
    JEITA CC = min(Pre Charge Current, Fast Charge Current - DATA x 25mA) if in pre-charge
    JEITA CC = Fast Charge Current - DATA x 25mA if in fast-charge */

    pm_register_address_type fc_cmp_cfg = ADDRESS(jeita_cccomp_cfg);
    err_flag = pm_comm_read_byte(SLAVE_ID, fc_cmp_cfg, &data, 0);

    *fc_comp = data * 25;
  }

  return err_flag;
}


pm_err_flag_type pm_schg_chgr_config_pre_charge_safety_timer(uint32 device_index, pm_schg_chgr_pre_charge_safety_timeout chg_sfty_timeout)
{
  pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (chg_sfty_timeout >= PM_SCHG_CHGR_PRE_CHARGE_SAFETY_TIMEOUT_INVAID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type pc_sft_cfg = ADDRESS(pre_charge_safety_timer_cfg);
    data = chg_sfty_timeout & 0x03;
    err_flag = pm_schg_chgr_unlock_perph_write(schg_ptr);
    err_flag |= pm_comm_write_byte_mask(SLAVE_ID, pc_sft_cfg, 0x03, data, 0);
  }

  return err_flag;
}

pm_err_flag_type pm_schg_chgr_config_fast_charge_safety_timer(uint32 device_index, pm_schg_chgr_fast_charge_safety_timeout chg_sfty_timeout)
{
  pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (chg_sfty_timeout >= PM_SCHG_CHGR_FAST_CHARGE_SAFETY_TIMEOUT_INVAID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type sft_cfg = ADDRESS(fast_charge_safety_timer_cfg);
    data = chg_sfty_timeout & 0x03;
    err_flag = pm_schg_chgr_unlock_perph_write(schg_ptr);
    err_flag |= pm_comm_write_byte_mask(SLAVE_ID, sft_cfg, 0x03, data, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_schg_chgr_get_safety_timer_pre_charge_config(uint32 device_index, pm_schg_chgr_pre_charge_safety_timeout *chg_sfty_timeout)
{
  pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == chg_sfty_timeout)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type sft_cfg = ADDRESS(pre_charge_safety_timer_cfg);
    err_flag = pm_comm_read_byte(SLAVE_ID, sft_cfg, &data, 0);
    switch (data & 0x3)
    {
      case 0:
      *chg_sfty_timeout = PM_SCHG_CHGR_PRE_CHARGE_TIMEOUT_24_MIN;  break;
      case 1:
      *chg_sfty_timeout = PM_SCHG_CHGR_PRE_CHARGE_TIMEOUT_48_MIN;  break;
      case 2:
      *chg_sfty_timeout = PM_SCHG_CHGR_PRE_CHARGE_TIMEOUT_96_MIN;  break;
      case 3:
      default:
      *chg_sfty_timeout = PM_SCHG_CHGR_PRE_CHARGE_TIMEOUT_192_MIN;
      break;
    }
  }

  return err_flag;
}

pm_err_flag_type pm_schg_chgr_get_safety_timer_fast_charge_config(uint32 device_index, pm_schg_chgr_fast_charge_safety_timeout *chg_sfty_timeout)
{
  pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == chg_sfty_timeout)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type sft_cfg  = ADDRESS(fast_charge_safety_timer_cfg);
    err_flag = pm_comm_read_byte(SLAVE_ID, sft_cfg, &data, 0);
    switch (data & 0x03)
    {
      case 0:
      *chg_sfty_timeout = PM_SCHG_CHGR_FAST_CHARGE_TIMEOUT_192_MIN;  break;
      case 1:
      *chg_sfty_timeout = PM_SCHG_CHGR_FAST_CHARGE_TIMEOUT_384_MIN;  break;
      case 2:
      *chg_sfty_timeout = PM_SCHG_CHGR_FAST_CHARGE_TIMEOUT_768_MIN;  break;
      default:
      *chg_sfty_timeout = PM_SCHG_CHGR_FAST_CHARGE_TIMEOUT_1536_MIN;
      break;
    }
  }

   return err_flag;
}

pm_err_flag_type pm_schg_chgr_jeita_en_cfg(uint32 device_index, pm_schg_chgr_jeita_cfg_type jeita_cfg_type, boolean enable)
{
  pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (jeita_cfg_type >= PM_SCHG_CHGR_JEITA_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type jeita_en_cfg = ADDRESS(jeita_en_cfg);
    err_flag |= pm_comm_write_byte_mask(SLAVE_ID, jeita_en_cfg, (uint8)(1 << jeita_cfg_type), (pm_register_data_type)(enable << jeita_cfg_type), 0);
  }

   return err_flag;
}

pm_err_flag_type pm_schg_chgr_get_schg_pmic_info(uint8 *pmic_index, uint8 *slave_id)
{
    if(!pmic_index || !slave_id)
        return PM_ERR_FLAG_INVALID_PARAMETER;

    *pmic_index = (PM_MAX_NUM_PMICS + 1);
    *slave_id   = (PM_MAX_NUM_SLAVE_IDS + 1);

    pm_schg_get_pmic_info(pmic_index, slave_id);

    if(*pmic_index >= PM_MAX_NUM_PMICS || *slave_id >= PM_MAX_NUM_SLAVE_IDS)
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        return PM_ERR_FLAG_SUCCESS;
    }
}

