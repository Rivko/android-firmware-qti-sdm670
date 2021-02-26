/*! \file
*  
*  \brief  pm_schg_misc.c driver implementation.
*  \details charger driver implementation.
*  &copy;
*  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE


when       who     	 what, where, why
--------   ---    	----------------------------------------------------------
02/17/17   pxm      Added API to get sysok reason 
01/10/17    cs      add API to set thermistor source configuration
11/14/16    sm      Added pm_schg_misc_set_buckboost_refresh_pulse()
04/22/16    sm      Added setting 2 more parameters in pm_schg_misc_config_wdog()
03/09/16    sm      Added pm_schg_misc_get_shutdown_status()
02/16/16    va      Adding required API for enabling charger
01/13/16    al      Initial revision
========================================================================== */

/*===========================================================================

					INCLUDE FILES

===========================================================================*/
#include "pm_schg_misc.h"
#include "pm_schg_driver.h"

/*===========================================================================
                       MACRO DECLARATIONS
===========================================================================*/

#define ADDRESS(x) (schg_ptr->schg_register->misc_register->base_address + schg_ptr->schg_register->misc_register->x)
#define SLAVE_ID   (schg_ptr->comm_ptr->slave_id)

/*===========================================================================

FUNCTION DEFINITIONS 

===========================================================================*/

pm_err_flag_type pm_schg_misc_unlock_perph_write(pm_schg_data_type *schg_ptr)
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


pm_err_flag_type pm_schg_misc_irq_enable(uint32 pmic_device, pm_schg_misc_irq_type irq, boolean enable)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_address_type irq_reg;
   pm_register_data_type data = 1 << irq;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

   if (NULL == schg_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SCHG_MISC_IRQ_INVALID)
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

pm_err_flag_type pm_schg_misc_irq_clear(uint32  pmic_device, pm_schg_misc_irq_type irq)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data = 1 << irq;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

   if (NULL == schg_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SCHG_MISC_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else
   {
      pm_register_address_type int_latched_clr = ADDRESS(int_latched_clr);

      err_flag = pm_comm_write_byte(SLAVE_ID, int_latched_clr, data, 0);
   }
   return err_flag;
}


pm_err_flag_type pm_schg_misc_irq_set_trigger(uint32 pmic_device, pm_schg_misc_irq_type irq, pm_irq_trigger_type trigger)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   uint8 mask = 1 << irq;
   pm_register_data_type set_type, polarity_high, polarity_low;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

   if (NULL == schg_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SCHG_MISC_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type int_set_type = ADDRESS(int_set_type);
      pm_register_address_type int_polarity_high = ADDRESS(int_polarity_high);
      pm_register_address_type int_polarity_low = ADDRESS(int_polarity_low);

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
      err_flag = pm_comm_write_byte_mask(SLAVE_ID, int_set_type, mask, set_type, 0);
      err_flag |= pm_comm_write_byte_mask(SLAVE_ID, int_polarity_high, mask, polarity_high, 0);
      err_flag |= pm_comm_write_byte_mask(SLAVE_ID, int_polarity_low, mask, polarity_low, 0);
   }

   return err_flag;
}

pm_err_flag_type pm_schg_misc_irq_status(uint32 pmic_device, pm_schg_misc_irq_type irq, pm_irq_status_type type, boolean *status)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;
   uint8 mask = 1 << irq;
   pm_register_address_type int_sts;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

   if (NULL == schg_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == status)
   {
      err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else if (irq >= PM_SCHG_MISC_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
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




pm_err_flag_type pm_schg_misc_get_power_path(uint32 pmic_device, pm_schg_misc_pwr_pth_type *power_path)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

   if (NULL == schg_ptr || NULL == power_path)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type power_path_status = ADDRESS(power_path_status);

      err_flag = pm_comm_read_byte(SLAVE_ID, power_path_status, &data, 0);

      *power_path = (pm_schg_misc_pwr_pth_type)((data & 0x06)>>0x1);
   }

   return err_flag;
}


pm_err_flag_type pm_schg_misc_get_sysok_reason(uint32 pmic_device, pm_schg_misc_sysok_reason_type *sysok_reason)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

   if (NULL == schg_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == sysok_reason)
   {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else
   {
      pm_register_address_type sysok_reason_status = ADDRESS(sysok_reason_status); 
	  
      err_flag = pm_comm_read_byte(SLAVE_ID, sysok_reason_status, &data, 0);

      *sysok_reason = (pm_schg_misc_sysok_reason_type)(data & 0x03);
    }

    return err_flag;
}


/*This API reads the misc temp range status*/
pm_err_flag_type pm_schg_misc_get_temp_range_sts(uint32 device_index, pm_schg_misc_temp_range_status *misc_temp_range_sts)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;

  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == misc_temp_range_sts)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type misc_temp_sts_reg = ADDRESS(temp_range_status);

    err_flag = pm_comm_read_byte(SLAVE_ID, misc_temp_sts_reg, &data, 0);
    if (PM_ERR_FLAG_SUCCESS == err_flag)
    {
      misc_temp_range_sts->thermreg_disabled    = data & 0x01;
      misc_temp_range_sts->temp_below_range     = data & 0x02;
      misc_temp_range_sts->temp_within_range    = data & 0x04;
      misc_temp_range_sts->temp_above_range     = data & 0x08;
      misc_temp_range_sts->alert_level          = data & 0x10;
      misc_temp_range_sts->tlim                 = data & 0x20;
      misc_temp_range_sts->therm_range_active   = data & 0x40;
      misc_temp_range_sts->therm_range_status_7 = 0; /* Reserved */
    }
  }

  return err_flag;
}

/*This API returns Input Current Limit (post aicl and therm adjustment in milliamp*/
pm_err_flag_type pm_schg_misc_get_icl_sts(uint32 device_index, uint8 *misc_icl_current_ma)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;

  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == misc_icl_current_ma)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type misc_icl_sts = ADDRESS(icl_status);
    err_flag = pm_comm_read_byte(SLAVE_ID, misc_icl_sts, &data, 0);
    *misc_icl_current_ma = data;
  }

  return err_flag;
}

/*This API returns Input Current Limit when adapter increments from 5V to 9V in QC2.0 in milliamp*/
pm_err_flag_type pm_schg_misc_get_adapter_5v_icl_status_sts(uint32 device_index, uint8 *misc_5v_icl_current_ma)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;

  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == misc_5v_icl_current_ma)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type misc_5v_icl_sts = ADDRESS(adapter_5v_icl_status);
    err_flag = pm_comm_read_byte(SLAVE_ID, misc_5v_icl_sts, &data, 0);
    *misc_5v_icl_current_ma = data;
  }

  return err_flag;
}


/*This API returns Input Current Limit when adapter increments from 9V to 12V in QC2.0 in milliamp*/
pm_err_flag_type pm_schg_misc_get_adapter_9v_icl_status_sts(uint32 device_index, uint8 *misc_9v_icl_current_ma)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;

  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == misc_9v_icl_current_ma)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type misc_9v_icl_sts = ADDRESS(adapter_9v_icl_status);
    err_flag = pm_comm_read_byte(SLAVE_ID, misc_9v_icl_sts, &data, 0);
    *misc_9v_icl_current_ma = data;
  }

  return err_flag;
}


/*This API reads the misc aicl status*/
pm_err_flag_type pm_schg_misc_get_aicl_sts(uint32 device_index, pm_schg_misc_aicl_status *misc_aicl_sts)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == misc_aicl_sts)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type misc_aicl_sts_reg = ADDRESS(aicl_status);

    err_flag = pm_comm_read_byte(SLAVE_ID, misc_aicl_sts_reg, &data, 0);
    if (PM_ERR_FLAG_SUCCESS == err_flag)
    {
      misc_aicl_sts->aicl_done                = data & 0x01;
      misc_aicl_sts->aicl_fail                = data & 0x02;
      misc_aicl_sts->icl_imin                 = data & 0x04;
      misc_aicl_sts->dcin_ch_collapse         = data & 0x08;
      misc_aicl_sts->usbin_ch_collapse        = data & 0x10;
      misc_aicl_sts->highest_dc               = data & 0x20;
      misc_aicl_sts->soft_limit               = data & 0x40;
      misc_aicl_sts->aicl_status_7            = 0; /* Reserved */
    }
  }

  return err_flag;
}


/*This API returns When bark/bite watchdog interrupt trips, this status bit can be used to determine which of the two wdog timer expired*/
pm_err_flag_type pm_schg_misc_get_misc_wdog_status(uint32 device_index, pm_schg_misc_wdog_status *misc_wdog_sts)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;

  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == misc_wdog_sts)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type misc_wdog_sts_reg = ADDRESS(wdog_status);
    err_flag = pm_comm_read_byte(SLAVE_ID, misc_wdog_sts_reg, &data, 0);
    *misc_wdog_sts = (data & 0x01 )? PM_SCHG_MISC_BITE : PM_SCHG_MISC_BARK;
  }

  return err_flag;
}

/*This API enables automatic fault protection */
pm_err_flag_type pm_schg_misc_en_afp(uint32 pmic_device)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  uint8 mask = PM_BIT(0);
  BOOLEAN enable = TRUE;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);
  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type afp_mode = ADDRESS(afp_mode);
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, afp_mode, mask, (pm_register_data_type)enable, 0);
  }

  return err_flag;
}

/* Writing to this register will pet (reset) BARK and BITE watchdog timers */
pm_err_flag_type pm_schg_misc_bark_bite_wdog_pet(uint32 pmic_device)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);
  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type wdog_pet = ADDRESS(bark_bite_wdog_pet);
    err_flag = pm_comm_write_byte(SLAVE_ID, wdog_pet, 0x01, 0);
  }

  return err_flag;
}

/*This API configures the charger watchdog */
pm_err_flag_type pm_schg_misc_config_wdog(uint32 pmic_device, pm_schg_misc_wdog_cfg_type misc_wd_cfg)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type wd_cfg_reg = ADDRESS(wd_cfg);
    data = ((misc_wd_cfg.watchdog_trigger_afp_en << 0x7) | (misc_wd_cfg.bark_wdog_int_en << 0x6) | (misc_wd_cfg.bite_wdog_int_en << 5) |
             (misc_wd_cfg.wdog_option << 1) | (misc_wd_cfg.wdog_timer_en << 0)) & (0xE3);
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, wd_cfg_reg, 0xE3, data, 0);
  }

  return err_flag;
}


/*This API configures the charger watchdog */
pm_err_flag_type pm_schg_misc_get_config_wdog(uint32 pmic_device, pm_schg_misc_wdog_cfg_type *misc_wd_cfg)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == misc_wd_cfg)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type wd_cfg_reg = ADDRESS(wd_cfg);
    err_flag = pm_comm_read_byte_mask(SLAVE_ID, wd_cfg_reg, 0xE3, &data, 0);

    misc_wd_cfg->watchdog_trigger_afp_en = (data & 0x80)>>7;
    misc_wd_cfg->bark_wdog_int_en        = (data & 0x40)>>6;
    misc_wd_cfg->bite_wdog_int_en        = (data & 0x20)>>5;
    misc_wd_cfg->wdog_option             = (data & 0x02)>>1;
    misc_wd_cfg->wdog_timer_en           = (data & 0x01);
  }

  return err_flag;
}


/*This API configures the charger snarl bark bite watchdog */
pm_err_flag_type pm_schg_misc_set_snarl_bark_bite_wd_cfg(uint32 pmic_device, pm_schg_misc_sbb_wd_cfg misc_wd_sbb_cfg)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if ((misc_wd_sbb_cfg.bite_wdog_timeout  >=  PM_SCHG_MISC_BITE_WDOG_TMOUT_INVALID )||
           (misc_wd_sbb_cfg.bark_wdog_timeout  >= PM_SCHG_MISC_BARK_WDOG_TMOUT_INVALID  )||
           (misc_wd_sbb_cfg.snarl_wdog_timeout >= PM_SCHG_MISC_SNARL_WDOG_TMOUT_INVALID ))
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type wd_sbb_cfg_reg = ADDRESS(snarl_bark_bite_wd_cfg);
    data = ((misc_wd_sbb_cfg.bite_wdog_disable_charging_cfg << 0x7) | (misc_wd_sbb_cfg.snarl_wdog_timeout << 0x4) | (misc_wd_sbb_cfg.bark_wdog_timeout << 2) | (misc_wd_sbb_cfg.bite_wdog_timeout));
    err_flag = pm_comm_write_byte(SLAVE_ID, wd_sbb_cfg_reg, data, 0);
  }

  return err_flag;
}


/*This API gets the charger snarl bark bite watchdog cfg*/
pm_err_flag_type pm_schg_misc_get_snarl_bark_bite_wd_cfg(uint32 pmic_device, pm_schg_misc_sbb_wd_cfg *misc_wd_sbb_cfg)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == misc_wd_sbb_cfg)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type wd_sbb_cfg_reg = ADDRESS(snarl_bark_bite_wd_cfg);
    err_flag = pm_comm_read_byte(SLAVE_ID, wd_sbb_cfg_reg, &data, 0);

    misc_wd_sbb_cfg->bite_wdog_disable_charging_cfg = ((data & 0x80) >> 7);
    misc_wd_sbb_cfg->snarl_wdog_timeout             = ((data & 0x70) >> 4);
    misc_wd_sbb_cfg->bark_wdog_timeout              = ((data & 0x0C) >> 2);
    misc_wd_sbb_cfg->bite_wdog_timeout              = (data & 0x03) ;
  }

  return err_flag;
}


/*This API configures misc aicl cfg */
pm_err_flag_type pm_schg_misc_set_aicl_cfg(uint32 pmic_device, pm_schg_misc_aicl_cfg misc_aicl_cfg, pm_schg_misc_aicl_rerun_type aicl_rerun_type)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = aicl_rerun_type << misc_aicl_cfg;
  uint8 mask = 1 << misc_aicl_cfg;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

  if (NULL == schg_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if(aicl_rerun_type >= PM_SCHG_MISC_AICL_RERUN_INVALID)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type aicl_cfg_reg = ADDRESS(aicl_cfg);
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, aicl_cfg_reg, mask, data, 0);
  }

  return err_flag;
}


/*This API gets misc aicl cfg */
pm_err_flag_type pm_schg_misc_get_aicl_cfg(uint32 pmic_device, pm_schg_misc_aicl_cfg misc_aicl_cfg, pm_schg_misc_aicl_rerun_type *aicl_rerun_type)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  uint8 mask = 0x1 << misc_aicl_cfg;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type aicl_cfg_reg = ADDRESS(aicl_cfg);
    err_flag = pm_comm_read_byte_mask(SLAVE_ID, aicl_cfg_reg, mask, &data, 0);

    *aicl_rerun_type = (pm_schg_misc_aicl_rerun_type)(data >> misc_aicl_cfg); 
  }

  return err_flag;
}

/*This API configures misc aicl rerun time */
pm_err_flag_type pm_schg_misc_set_aicl_rerun_time_cfg(uint32 pmic_device, pm_schg_misc_aicl_rerun_time_cfg aicl_rerun_time)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (PM_SCHG_MISC_AICL_RERUN_TIME_CFG_INVALID < aicl_rerun_time)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type aicl_cfg_reg = ADDRESS(aicl_rerun_time_cfg);
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, aicl_cfg_reg, 0x03, aicl_rerun_time , 0);
  }

  return err_flag;
}


/*This API gets misc aicl cfg */
pm_err_flag_type pm_schg_misc_get_aicl_rerun_cfg(uint32 pmic_device, pm_schg_misc_aicl_rerun_time_cfg *misc_aicl_cfg)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);
  pm_register_data_type data = 0;

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == misc_aicl_cfg)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type aicl_cfg_reg = ADDRESS(aicl_rerun_time_cfg);
    err_flag = pm_comm_read_byte_mask(SLAVE_ID, aicl_cfg_reg, 0x03, &data, 0);
    *misc_aicl_cfg = (pm_schg_misc_aicl_rerun_time_cfg) data;
  }

  return err_flag;
}


/*This API gets shutdown status */
pm_err_flag_type pm_schg_misc_get_shutdown_status(uint32 pmic_device, boolean *shdn_sts)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);
  pm_register_data_type data = 0;

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == shdn_sts)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type shdn_cmd = ADDRESS(shdn_cmd);
    err_flag = pm_comm_read_byte(SLAVE_ID, shdn_cmd, &data, 0);
    *shdn_sts = (boolean) data;
  }

  return err_flag;
}



/*This API sets buckboost refreshing pulse rate*/
pm_err_flag_type pm_schg_misc_set_buckboost_refresh_pulse(uint32 device_index, pm_schg_misc_buckboost_refresh_pulse_type refresh_pulse_type)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  uint8                 mask = 0x80;
  pm_register_data_type data = (refresh_pulse_type << 7);

  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (PM_SCHG_MISC_BUCKBOOST_REFRESH_PULSE_INVALID <= refresh_pulse_type)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type misc_eng_buckboost_cfg1 = ADDRESS(eng_buckboost_cfg1);
    err_flag = pm_schg_misc_unlock_perph_write(schg_ptr);
    err_flag |= pm_comm_write_byte_mask(SLAVE_ID, misc_eng_buckboost_cfg1, mask, data, 0);
  }

  return err_flag;
}


/* This API sets thermistor source configuration for charging thermal mitigation (inov)*/
pm_err_flag_type pm_schg_misc_set_thermreg_src_cfg(uint32 device_index, pm_schg_misc_therm_src_cfg_type src_cfg_type, boolean enable)
{
  pm_err_flag_type      err_flag = PM_ERR_FLAG_SUCCESS;
  uint8                 mask = (1 << src_cfg_type);
  pm_register_data_type data = (enable << src_cfg_type);

  pm_schg_data_type *schg_ptr = pm_schg_get_data(device_index);

  if (NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (PM_SCHG_MISC_THERM_SRC_CFG_INVALID <= src_cfg_type)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type misc_therm_src_cfg = ADDRESS(thermreg_src_cfg);
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, misc_therm_src_cfg, mask, data, 0);
  }

  return err_flag;
}

