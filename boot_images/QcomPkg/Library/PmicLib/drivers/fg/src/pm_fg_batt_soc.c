/*! \file
*  
*  \brief  pm_fg_soc.c --- FG Driver SOC Implementation.
*  \details  Fuel Gauge SOC driver implementation.
*  &copy; 
*  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE


when        who     what, where, why
--------    ---     ---------------------------------------------------------- 
05/23/17    sv      Added pm_fg_get_batt_soc_enable_ctl API
06/29/16    va      updating soc api for matching copy read
04/21/16    va      Adding FG Rst APIs
08/20/14    al      Updating comm lib 
08/27/14    va      Driver cleanup update
08/13/14    sm      Made changes to read soc in single API
                    Also corrected SOC calculation in get_monotonic_soc API 
06/25/14    va      Driver update, added protected call
04/07/14    va      New file
========================================================================== */

/*===========================================================================

INCLUDE FILES 

===========================================================================*/
#include "pm_fg_batt_soc.h"
#include "pm_fg_driver.h"
#include "pm_resource_manager.h"

/*===========================================================================
                       MACRO DECLARATIONS
===========================================================================*/

#define ADDRESS(x) (fg_batt_soc_ptr->fg_register->batt_soc_register->base_address + fg_batt_soc_ptr->fg_register->batt_soc_register->x)
#define SLAVE_ID   (fg_batt_soc_ptr->comm_ptr->slave_id)

#define PM_FG_BATT_SOC_MAX_RETRY      5

/*===========================================================================

FUNCTION DEFINITIONS 

===========================================================================*/


pm_err_flag_type pm_fg_soc_grant_sec_access(pm_fg_data_type *fg_batt_soc_ptr)
{

   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

   if (NULL == fg_batt_soc_ptr)
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

pm_err_flag_type pm_fg_batt_soc_irq_enable(uint32 pmic_device, pm_fg_batt_soc_irq_type irq, boolean enable)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_address_type irq_reg;
   pm_register_data_type data = 1 << irq;

   pm_fg_data_type *fg_batt_soc_ptr  = pm_fg_get_data(pmic_device);

   if (NULL == fg_batt_soc_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_FG_BATT_SOC_IRQ_INVALID)
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

pm_err_flag_type pm_fg_batt_soc_irq_clear(uint32  pmic_device, pm_fg_batt_soc_irq_type irq)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data = 1 << irq;
   pm_register_address_type int_latched_clr;
   pm_fg_data_type *fg_batt_soc_ptr  = pm_fg_get_data(pmic_device);

   if (NULL == fg_batt_soc_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_FG_BATT_SOC_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      int_latched_clr = ADDRESS(int_latched_clr);
      err_flag = pm_comm_write_byte(SLAVE_ID, int_latched_clr, data, 0);
   }

   return err_flag;
}


pm_err_flag_type pm_fg_batt_soc_irq_set_trigger(uint32 pmic_device, pm_fg_batt_soc_irq_type irq, pm_irq_trigger_type trigger)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   uint8 mask = 1 << irq;
   pm_register_data_type set_type, polarity_high, polarity_low;
   pm_register_address_type int_set_type, int_polarity_high, int_polarity_low;

   pm_fg_data_type *fg_batt_soc_ptr  = pm_fg_get_data(pmic_device);

   if (NULL == fg_batt_soc_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_FG_BATT_SOC_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      int_set_type = ADDRESS(int_set_type);
      int_polarity_high = ADDRESS(int_polarity_high);
      int_polarity_low = ADDRESS(int_polarity_low);

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


pm_err_flag_type pm_fg_batt_soc_irq_status(uint32 pmic_device, pm_fg_batt_soc_irq_type irq, pm_irq_status_type type, boolean *status)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;
   uint8 mask = 1 << irq;
   pm_register_address_type int_sts;
   pm_fg_data_type *fg_batt_soc_ptr  = pm_fg_get_data(pmic_device);

   if (NULL == fg_batt_soc_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_FG_BATT_SOC_IRQ_INVALID)
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

pm_err_flag_type pm_fg_batt_soc_get_monotonic_soc_cp(uint32 device_index, uint32* soc, boolean* soc_match_cp)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;

  pm_register_data_type data[2] = {0,0};

  pm_fg_data_type *fg_batt_soc_ptr  = pm_fg_get_data(device_index);

  if (NULL == fg_batt_soc_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == soc || NULL == soc_match_cp)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type monotonic_soc = ADDRESS(monotonic_soc);
    err_flag = pm_comm_read_byte_array(SLAVE_ID, monotonic_soc, 2, (uint8*) &data, 0);
    /* compare MONOTONIC_SOC and MONOTONIC_SOC_CP */
    *soc_match_cp = (data[0]==data[1]) ? TRUE : FALSE; 
    /* 0xFF = 100% soc */
    *soc = (data[0] * 100)/0xFF; 
  }

  return err_flag;
}


pm_err_flag_type pm_fg_batt_soc_get_monotonic_soc(uint32 device_index, uint32* soc)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  boolean soc_match_cp = FALSE;
  uint32  soc_read = 0;
  uint8  count =0;
  pm_fg_data_type *fg_batt_soc_ptr  = pm_fg_get_data(device_index);

  if (NULL == fg_batt_soc_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == soc)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    err_flag |= pm_fg_batt_soc_get_monotonic_soc_cp(device_index, &soc_read, &soc_match_cp);
    
    //If ibatt_match_cp is FALSE, retry
    if (soc_match_cp == FALSE)
    {
      for (count =0; count < PM_FG_BATT_SOC_MAX_RETRY; count++) 
      {
        err_flag |= pm_fg_batt_soc_get_monotonic_soc_cp(device_index, &soc_read, &soc_match_cp);
        if (soc_match_cp == TRUE)
        {
          break;
        }
      }
    }
  }
  if (soc_match_cp == FALSE)
  {
    err_flag |= PM_ERR_FLAG_DATA_VERIFY_ERR;
  }
  else
  {
    /* Success in reading SOC */
    *soc = soc_read;
  }

  return err_flag;
}

pm_err_flag_type pm_fg_batt_soc_enable_fg_algorithm(uint32 device_index, boolean enable)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;

   pm_register_data_type data = (enable) ? 0xFF : 0x00;
   
   pm_fg_data_type *fg_batt_soc_ptr  = pm_fg_get_data(device_index);

   if (NULL == fg_batt_soc_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type en_ctl = ADDRESS(en_ctl);
      err_flag = pm_comm_write_byte_mask(SLAVE_ID, en_ctl, 0x80, data, 0);
   }

   return err_flag;
}

pm_err_flag_type pm_fg_batt_soc_restart(uint32 device_index, boolean restart)
{
   pm_err_flag_type      err_flag          = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data              = (restart) ? 0xFF : 0x00;
   pm_fg_data_type       *fg_batt_soc_ptr  = pm_fg_get_data(device_index);

   if (NULL == fg_batt_soc_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type restart = ADDRESS(restart);
      err_flag = pm_comm_write_byte_mask(SLAVE_ID, restart, 0x1, data, 0);
   }

   return err_flag;
}

pm_err_flag_type pm_fg_batt_soc_set_fg_rst_ctrl(uint32 device_index, pm_fg_batt_soc_rst_ctrl fg_rst_ctrl, boolean enable)
{
  pm_err_flag_type         err_flag         = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type fg_rst_ctrl_reg  = 0;
  pm_fg_data_type          *fg_batt_soc_ptr = pm_fg_get_data(device_index);
  
  if (NULL == fg_batt_soc_ptr)
  {
     err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_rst_ctrl_reg = ADDRESS(restart);
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, fg_rst_ctrl_reg, (enable << fg_rst_ctrl), (pm_register_data_type)(enable << fg_rst_ctrl), 0);
  }

  return err_flag;
}

pm_err_flag_type pm_fg_batt_soc_get_fg_rst_ctrl(uint32 device_index, pm_fg_batt_soc_rst_ctrl fg_rst_ctrl, boolean *enable)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data        = 0;
  pm_register_address_type fg_rst_ctrl_reg  = 0;
  pm_fg_data_type       *fg_batt_soc_ptr  = pm_fg_get_data(device_index);

  if (NULL == fg_batt_soc_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == enable )
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_rst_ctrl_reg = ADDRESS(restart);
    err_flag = pm_comm_read_byte(SLAVE_ID, fg_rst_ctrl_reg, &data, 0);
    if (PM_ERR_FLAG_SUCCESS == err_flag)
    {
       *enable = (data & (0x1 << fg_rst_ctrl)) ? TRUE : FALSE;
    }
  }

  return err_flag;
}

pm_err_flag_type pm_fg_get_batt_soc_enable_ctl(uint32 pmic_device, boolean *enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint8 soc_enable_mask = 0x80;
  pm_register_data_type data = 0;

  pm_fg_data_type *fg_batt_soc_ptr  = pm_fg_get_data(pmic_device);
  if (NULL == fg_batt_soc_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == enable)
  {
     err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type fg_batt_soc_en_ctl_reg = ADDRESS(en_ctl);
    err_flag = pm_comm_read_byte(SLAVE_ID, fg_batt_soc_en_ctl_reg, &data, 0);
    if((data & soc_enable_mask )== 0x80)
    {
       *enable = TRUE;
    }
    else
    {
        *enable = FALSE;
    }
  }

  return err_flag;
}



