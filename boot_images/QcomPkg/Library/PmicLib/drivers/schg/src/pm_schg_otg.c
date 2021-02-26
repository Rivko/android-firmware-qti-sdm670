/*! \file
*  
*  \brief  pm_schg_otg.c driver implementation.
*  \details charger driver implementation.
*  &copy;
*  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE


when        who          what, where, why
--------    ---          ---------------------------------------------------------- 
02/08/17    ll           Added API for OTG workaround for OTG device with large cap
12/06/16    sm           Added API to return OTG BOOST SoftStart done status
06/09/16    sm           Redefined pm_schg_otg_status_type struct and pm_schg_otg_get_status() API
05/05/16    al           Changing OTG config to CMD register when enabling OTG by SW
04/18/14    al           Initial revision
========================================================================== */

/*===========================================================================

                    INCLUDE FILES

===========================================================================*/

#include "pm_schg_otg.h"
#include "pm_schg_driver.h"
#include "pm_resource_manager.h"

/*===========================================================================
                       MACRO DECLARATIONS
===========================================================================*/

#define ADDRESS(x) (schg_ptr->schg_register->otg_register->base_address + schg_ptr->schg_register->otg_register->x)
#define SLAVE_ID   (schg_ptr->comm_ptr->slave_id)

#define PM_SCHG_OTG_RT_STS_NOT_OK_MASK                  0x7

/*===========================================================================

FUNCTION DEFINITIONS 

===========================================================================*/


pm_err_flag_type pm_schg_otg_unlock_perph_write(pm_schg_data_type *schg_ptr)
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

pm_err_flag_type pm_schg_otg_enable(uint32 pmic_device,boolean enable)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;

  pm_register_data_type data = (enable)? 0xFF : 0x00;

  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

  if(NULL == schg_ptr) 
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    err_flag = pm_schg_otg_config_otg(pmic_device, PM_SCHG_OTG_CFG_OTG_EN_SRC_CFG, FALSE); //0 = OTG Enable Source Command

    pm_register_address_type cmd_otg = ADDRESS(cmd_otg);

    err_flag |= pm_comm_write_byte_mask(SLAVE_ID, cmd_otg, 0x1, data, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_schg_otg_config_otg(uint32 pmic_device, pm_schg_otg_cfg_type otg_cfg_type, boolean set_value)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type mask = 0x1<<otg_cfg_type;
  pm_register_data_type data = (set_value)? 0xFF : 0x00;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

  if(NULL == schg_ptr) 
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type otg_cfg = ADDRESS(otg_cfg);

    err_flag |= pm_comm_write_byte_mask(SLAVE_ID, otg_cfg, mask, data, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_schg_otg_set_otg_batt_uvlo(uint32 pmic_device, uint32 milli_volt)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

  if(NULL == schg_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type bat_uvlo_threshold = ADDRESS(bat_uvlo_threshold_cfg);

    if(milli_volt < 2900)
    {
      data = 0x00;
    }
    else if(milli_volt < 3100)
    {
      data = 0x01;
    }
    else if(milli_volt < 3300)
    {
      data = 0x02;
    }
    else
    {
      data = 0x03;
    }

    err_flag |= pm_comm_write_byte_mask(SLAVE_ID, bat_uvlo_threshold, 0x3, data, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_schg_otg_set_otg_i_limit(uint32 pmic_device, uint32 otg_limit_ma)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

  if(NULL == schg_ptr) 
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type otg_current_limit_cfg   = ADDRESS(otg_current_limit_cfg);

    if(otg_limit_ma < 500)
    {
      data = 0x0;
    }
    else if(otg_limit_ma > 1750)
    {
      data = 0x7;
    }
    else
    {
      data = (otg_limit_ma/250) - 1;
    }
    

    err_flag |= pm_comm_write_byte_mask(SLAVE_ID, otg_current_limit_cfg, 0x7, data, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_schg_otg_irq_enable(uint32 pmic_device, pm_schg_otg_irq_type irq, boolean enable)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_address_type irq_reg;
   pm_register_data_type data = 1 << irq;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

   if (NULL == schg_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SCHG_OTG_IRQ_INVALID)
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

pm_err_flag_type pm_schg_otg_irq_clear(uint32  pmic_device, pm_schg_otg_irq_type irq)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data = 1 << irq;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

   if (NULL == schg_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SCHG_OTG_IRQ_INVALID)
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


pm_err_flag_type pm_schg_otg_irq_set_trigger(uint32 pmic_device, pm_schg_otg_irq_type irq, pm_irq_trigger_type trigger)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   uint8 mask = 1 << irq;
   pm_register_data_type set_type, polarity_high, polarity_low;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

   if (NULL == schg_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SCHG_OTG_IRQ_INVALID)
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

pm_err_flag_type pm_schg_otg_irq_status(uint32 pmic_device, pm_schg_otg_irq_type irq, pm_irq_status_type type, boolean *status)
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
   else if (irq >= PM_SCHG_OTG_IRQ_INVALID)
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


pm_err_flag_type pm_schg_otg_get_status(uint32 pmic_device, pm_schg_otg_status_type* otg_status)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data;
  uint8 read_mask = 0x07;

  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

  if((NULL == schg_ptr) || (NULL == otg_status)) 
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    /*OTG enable*/
    pm_register_address_type otg_status_addr = ADDRESS(otg_status);
    err_flag = pm_comm_read_byte_mask(SLAVE_ID, otg_status_addr, read_mask, &data, 0);

    *otg_status = (pm_schg_otg_status_type)data;
  }

  return err_flag;
}


pm_err_flag_type pm_schg_otg_get_settings(uint32 pmic_device, pm_schg_otg_cfg_setting_type* otg_settings)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data;

  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

  if((NULL == schg_ptr) || (NULL == otg_settings)) 
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    /*OTG enable*/
    pm_register_address_type cmd_otg = ADDRESS(cmd_otg);
    err_flag |= pm_comm_read_byte(SLAVE_ID, cmd_otg, &data, 0);
    otg_settings->otg_enable = (data & 0x1)? TRUE : FALSE;
    data = 0;
    /*OTG cfg*/
    pm_register_address_type otg_cfg  = ADDRESS(otg_cfg);
    err_flag |= pm_comm_read_byte(SLAVE_ID, otg_cfg, &data, 0);
    otg_settings->cfg_status.concurrent_mode_cfg  = (data & 0x1)? TRUE : FALSE;
    otg_settings->cfg_status.otg_en_src_cfg  = (data & 0x2)? TRUE : FALSE;
    otg_settings->cfg_status.enable_otg_in_debug_mode  = (data & 0x4)? TRUE : FALSE;
    otg_settings->cfg_status.increase_dfp_time  = (data & 0x8)? TRUE : FALSE;
    /*OTG UVLO*/
    pm_register_address_type bat_uvlo_threshold = ADDRESS(bat_uvlo_threshold_cfg);
    err_flag |= pm_comm_read_byte(SLAVE_ID, bat_uvlo_threshold, &data, 0);
    otg_settings->milli_volt = 2700+ ((data & 0x3)*200);
    /*OTG ILIMIT*/
    pm_register_address_type otg_current_limit_cfg   = ADDRESS(otg_current_limit_cfg);
    err_flag |= pm_comm_read_byte(SLAVE_ID, otg_current_limit_cfg, &data, 0);
    otg_settings->otg_limit_ma = 250*(1+data);
  }

  return err_flag;
}

pm_err_flag_type pm_schg_otg_get_boost_softstart_done_status(uint32 pmic_device, boolean *softstart_done)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0x00;
  uint8 read_mask = 0x08;

  pm_schg_data_type *schg_ptr  = pm_schg_get_data(pmic_device);

  if((NULL == schg_ptr) || (NULL == softstart_done)) 
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type otg_status_addr = ADDRESS(otg_status);
    err_flag = pm_comm_read_byte_mask(SLAVE_ID, otg_status_addr, read_mask, &data, 0);

    *softstart_done = (data) ? TRUE : FALSE;
  }

  return err_flag;
}

pm_err_flag_type pm_schg_otg_halt_1_in_8_mode(uint32 pmic_device, boolean enable)
{
	pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
	uint8 bit_position = 0;
	uint8 mask = 1 << bit_position;
	pm_register_data_type data = ((uint8) enable) << bit_position;
	pm_schg_data_type *schg_ptr = pm_schg_get_data(pmic_device);

	if (NULL == schg_ptr)
	{
		err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
	}
	else
	{
		pm_register_address_type otg_buckboost_cfg_addr = ADDRESS(otg_eng_otg_cfg);
		err_flag = pm_schg_otg_unlock_perph_write(schg_ptr);
		err_flag |= pm_comm_write_byte_mask(SLAVE_ID, otg_buckboost_cfg_addr, mask, data, 0);
	}

	return err_flag;
}

pm_err_flag_type pm_schg_otg_get_rt_sts_ok(uint32 pmic_device, boolean *pbOtgOK)
{
	pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
	pm_register_data_type data = 0;
	pm_schg_data_type *schg_ptr = pm_schg_get_data(pmic_device);

	if (NULL == schg_ptr)
	{
		err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
	}
	else
	{
		pm_register_address_type otg_rt_sts_addr = ADDRESS(int_rt_sts);
		err_flag = pm_comm_read_byte(SLAVE_ID, otg_rt_sts_addr, &data, 0);
	}
	*pbOtgOK = (data & PM_SCHG_OTG_RT_STS_NOT_OK_MASK) ? FALSE : TRUE;
	return err_flag;
}