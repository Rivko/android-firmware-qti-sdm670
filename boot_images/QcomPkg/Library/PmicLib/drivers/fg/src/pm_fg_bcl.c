/*! \file
*  
*  \brief  pm_fg_bcl.c --- FG Driver BCL Implementation.
*  \details  Fuel Gauge BCL driver implementation.
*  &copy; 
*  Copyright (c) 2014 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE


when        who     what, where, why
--------    ---     ---------------------------------------------------------- 
05/24/16    sm      Added API to enable goc
03/15/16   aab      Added/updated pm_fg_bcl_get_vbatt() and pm_fg_bcl_get_vbatt_match_cp() 
02/16/16    va      Adding required API for enanlbing charger
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
#include "pm_fg_bcl.h"
#include "pm_fg_driver.h"
#include "pm_resource_manager.h"

/*===========================================================================
                       MACRO DECLARATIONS
===========================================================================*/

#define ADDRESS(x) (fg_bcl_ptr->fg_register->bcl_register->base_address + fg_bcl_ptr->fg_register->bcl_register->x)
#define SLAVE_ID   (fg_bcl_ptr->comm_ptr->slave_id)

#define PM_FG_BCL_IBATT_LSB_VALUE    80
#define PM_FG_BCL_VBATT_LSB_VALUE    39

pm_err_flag_type pm_fg_bcl_get_vbatt_match_cp(uint32 pmic_device, int32 *vbatt_mv, boolean *vbatt_match_cp);

/*===========================================================================

FUNCTION DEFINITIONS 

===========================================================================*/




pm_err_flag_type pm_fg_bcl_unlock_perph_write(pm_fg_data_type *fg_bcl_ptr)
{

   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

   if (NULL == fg_bcl_ptr)
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


pm_err_flag_type pm_fg_bcl_irq_enable(uint32 pmic_device, pm_fg_bcl_irq_type irq, boolean enable)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_address_type irq_reg;
   pm_register_data_type data = 1 << irq;

   pm_fg_data_type *fg_bcl_ptr  = pm_fg_get_data(pmic_device);

   if (NULL == fg_bcl_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_FG_BCL_IRQ_INVALID)
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

pm_err_flag_type pm_fg_bcl_irq_clear(uint32  pmic_device, pm_fg_bcl_irq_type irq)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data = 1 << irq;
   pm_register_address_type int_latched_clr;
   pm_fg_data_type *fg_bcl_ptr  = pm_fg_get_data(pmic_device);

   if (NULL == fg_bcl_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_FG_BCL_IRQ_INVALID)
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


pm_err_flag_type pm_fg_bcl_irq_set_trigger(uint32 pmic_device, pm_fg_bcl_irq_type irq, pm_irq_trigger_type trigger)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   uint8 mask = 1 << irq;
   pm_register_data_type set_type, polarity_high, polarity_low;
   pm_register_address_type int_set_type, int_polarity_high, int_polarity_low;

   pm_fg_data_type *fg_bcl_ptr  = pm_fg_get_data(pmic_device);

   if (NULL == fg_bcl_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_FG_BCL_IRQ_INVALID)
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


pm_err_flag_type pm_fg_bcl_irq_status(uint32 pmic_device, pm_fg_bcl_irq_type irq, pm_irq_status_type type, boolean *status)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;
   uint8 mask = 1 << irq;
   pm_register_address_type int_sts;
   pm_fg_data_type *fg_bcl_ptr  = pm_fg_get_data(pmic_device);

   if (NULL == fg_bcl_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_FG_BCL_IRQ_INVALID)
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


pm_err_flag_type pm_fg_bcl_get_ibat(uint32 pmic_device, int32 *current_ma, boolean *current_match_cp)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data[3] = {0,0,0};

   int8   temp_data = 0;
   int32  temp_data1 = 0;
   
   pm_fg_data_type *fg_bcl_ptr  = pm_fg_get_data(pmic_device);

   if (NULL == fg_bcl_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type ibatt = ADDRESS(ibatt); 
      err_flag = pm_comm_read_byte_array(SLAVE_ID, ibatt, 3, data, 0);
      *current_match_cp = (data[0]==data[2]) ? TRUE : FALSE; //compare IBATT and IBATT_CP

      //ibat_final = (data[0] & 0x80)? ((-1) * ((~data[0] +1)  * PM_FG_BCL_IBATT_LSB_VALUE)) : (data[0] * PM_FG_BCL_IBATT_LSB_VALUE);
      //ibat_temp = (int32)data[0]; /*extending signed representation to int32*/
      temp_data = data[0];//Convert uint8 to int8
      temp_data1 = temp_data; //Convert int8 to int32
      *current_ma = temp_data1 * PM_FG_BCL_IBATT_LSB_VALUE;
   }

   return err_flag;
}


/**
* @brief This function returns the battery voltage and also a flag to tell if the reading of 
          FG_BCL_VBATT and FG_BCL_VBATT_CP is a match
 */
pm_err_flag_type pm_fg_bcl_get_vbatt_match_cp(uint32 pmic_device, int32 *vbatt_mv, boolean *vbatt_match_cp)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data[3] = {0,0,0};
   int32 vbat_temp = 0;

   pm_fg_data_type *fg_bcl_ptr  = pm_fg_get_data(pmic_device);

   if (NULL == fg_bcl_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == vbatt_mv)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type vbatt = ADDRESS(vbatt); 

      err_flag = pm_comm_read_byte_array(SLAVE_ID, vbatt, 3, (uint8*)&data, 0);

      *vbatt_match_cp = (data[0]==data[2]) ? TRUE : FALSE; //compare VBATT and VBATT_CP

      //vbat_final = (data[0] & 0x80)? ((-1) * ((~data[0] +1)  * PM_FG_BCL_VBATT_LSB_VALUE)) : (data[0] * PM_FG_BCL_VBATT_LSB_VALUE);
      vbat_temp = (int32)data[0]; /*extending signed representation to int32*/
      *vbatt_mv = vbat_temp * PM_FG_BCL_VBATT_LSB_VALUE;
   }

   return err_flag;
}


pm_err_flag_type pm_fg_bcl_get_vbatt(uint32 pmic_device, int32 *vbatt_mv)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;

   boolean vbatt_match_cp = FALSE;
   uint8 count =0;

   if (NULL == vbatt_mv)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      err_flag |= pm_fg_bcl_get_vbatt_match_cp(pmic_device, vbatt_mv, &vbatt_match_cp);

      //If vbatt_match_cp is FALSE, retry
      if (vbatt_match_cp == FALSE)
      {
         for (count =0; count <5; count++) 
         {
            err_flag |= pm_fg_bcl_get_vbatt_match_cp(pmic_device, vbatt_mv, &vbatt_match_cp);
            if (vbatt_match_cp == TRUE)
            {
               break;
            }
         }
      }

      if (vbatt_match_cp == FALSE)
      {
         err_flag |= PM_ERR_FLAG_PERIPHERAL_ERR;
      }
   }
   
   return err_flag;
}
   

/**
 This function enable/disables  Fule Gauge Algorithm BCL (battery current limiting s/w use) 
*
*/
pm_err_flag_type pm_fg_bcl_enable_ctl(uint32 pmic_device, boolean enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint8 mask = 0x80;

  pm_fg_data_type *fg_bcl_ptr  = pm_fg_get_data(pmic_device);


  if (NULL == fg_bcl_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type fg_bcl_en_ctl_reg = ADDRESS(en_ctl);
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, fg_bcl_en_ctl_reg, mask, (pm_register_data_type)(enable << 7), 0);
  }

  return err_flag;
}



pm_err_flag_type pm_fg_get_bcl_enable_ctl(uint32 pmic_device, boolean *enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint8 mask = 0x80;
  pm_register_data_type data = 0;

  pm_fg_data_type *fg_bcl_ptr  = pm_fg_get_data(pmic_device);
  if (NULL == fg_bcl_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == enable)
  {
     err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type fg_bcl_en_ctl_reg = ADDRESS(en_ctl);
    err_flag = pm_comm_read_byte_mask(SLAVE_ID, fg_bcl_en_ctl_reg, mask, &data, 0);
    *enable = (boolean )(data & 0x80);
  }

  return err_flag;
}


pm_err_flag_type pm_fg_set_bcl_cfg(uint32 pmic_device, pm_fg_bcl_cfg bcl_cfg, boolean enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint8 mask = 0x80;

  pm_fg_data_type *fg_bcl_ptr  = pm_fg_get_data(pmic_device);
  if (NULL == fg_bcl_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (bcl_cfg >= PM_FG_BCL_INVALID)
  {
     err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type bcl_cfg_reg = ADDRESS(bcl_cfg);
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, bcl_cfg_reg, mask, (pm_register_data_type)(enable << bcl_cfg), 0);
  }

  return err_flag;

}

pm_err_flag_type pm_fg_bcl_enable_goc(uint32 pmic_device, boolean enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint8 mask = 0x80;

  pm_fg_data_type *fg_bcl_ptr  = pm_fg_get_data(pmic_device);


  if (NULL == fg_bcl_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type fg_bcl_cfg_reg = ADDRESS(bcl_cfg);
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, fg_bcl_cfg_reg, mask, (pm_register_data_type)(enable << 7), 0);
  }

  return err_flag;
}
