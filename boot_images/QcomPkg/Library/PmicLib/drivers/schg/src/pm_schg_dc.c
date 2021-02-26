/*! \file
*  
*  \brief  pm_schg_dc.c driver implementation.
*  \details charger driver implementation.
*  &copy;
*  Copyright (c) 2014 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE


when       who     	 what, where, why
--------   ---  	----------------------------------------------------------
02/08/17   ll       Added API for OTG workaround for OTG device with large cap
05/27/16   pxm      Add ZIN configuration function 
04/18/14    al           Initial revision
========================================================================== */

/*===========================================================================

					INCLUDE FILES

===========================================================================*/

#include "pm_schg_dc.h"
#include "pm_schg_driver.h"
#include "pm_resource_manager.h"

/*===========================================================================
                       MACRO DECLARATIONS
===========================================================================*/

#define ADDRESS(x) (schg_ptr->schg_register->dc_register->base_address + schg_ptr->schg_register->dc_register->x)
#define SLAVE_ID   (schg_ptr->comm_ptr->slave_id)
        
#define PM_SCHG_DC_DCIN_MAX_CURRENT_LIMIT_CFG_MA      3000
#define PM_SCHG_DC_DCIN_CURRENT_LIMIT_CFG_STEP_SIZE    25
#define PM_SCHG_DC_IVREF_OTG_SS_MASK                   0x7

/*===========================================================================

FUNCTION DEFINITIONS 

===========================================================================*/

/*Unlock peripheral for secured write access*/

pm_err_flag_type pm_schg_dc_unlock_perph_write(pm_schg_data_type *schg_ptr)
{
   pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;

   if (NULL == schg_ptr)
   {
      errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type sec_access = ADDRESS(sec_access);
      errFlag = pm_comm_write_byte(SLAVE_ID, sec_access, 0xA5, 0);
   }

   return errFlag;
}


pm_err_flag_type pm_schg_dc_irq_enable(uint32 device_index, pm_schg_dc_irq_type irq, boolean enable)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;
   pm_register_address_type irq_reg;
   pm_register_data_type data = 1 << irq;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

   if (NULL == schg_ptr)
   {
      errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SCHG_DC_IRQ_INVALID)
   {
      errFlag =  PM_ERR_FLAG_INVALID_PARAMETER;
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

      errFlag = pm_comm_write_byte(SLAVE_ID, irq_reg, data, 0);
   }
   return errFlag;
}

pm_err_flag_type pm_schg_dc_irq_clear(uint32  device_index, pm_schg_dc_irq_type irq)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data = 1 << irq;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

   if (NULL == schg_ptr)
   {
      errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SCHG_DC_IRQ_INVALID)
   {
      errFlag =  PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else
   {
      pm_register_address_type int_latched_clr = ADDRESS(int_latched_clr);

      errFlag = pm_comm_write_byte(SLAVE_ID, int_latched_clr, data, 0);
   }
   return errFlag;
}


pm_err_flag_type pm_schg_dc_irq_set_trigger(uint32 device_index, pm_schg_dc_irq_type irq, pm_irq_trigger_type trigger)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;
   uint8 mask = 1 << irq;
   pm_register_data_type set_type, polarity_high, polarity_low;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

   if (NULL == schg_ptr)
   {
      errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SCHG_DC_IRQ_INVALID)
   {
      errFlag =  PM_ERR_FLAG_INVALID_PARAMETER;
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
      errFlag = pm_comm_write_byte_mask(SLAVE_ID, int_set_type, mask, set_type, 0);
      errFlag |= pm_comm_write_byte_mask(SLAVE_ID, int_polarity_high, mask, polarity_high, 0);
      errFlag |= pm_comm_write_byte_mask(SLAVE_ID, int_polarity_low, mask, polarity_low, 0);
   }

   return errFlag;
}

pm_err_flag_type pm_schg_dc_irq_status(uint32 device_index, pm_schg_dc_irq_type irq, pm_irq_status_type type, boolean *status)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data;
   uint8 mask = 1 << irq;
   pm_register_address_type int_sts;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

   if ((NULL == schg_ptr) || (NULL == status))
   {
      errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SCHG_DC_IRQ_INVALID)
   {
      errFlag =  PM_ERR_FLAG_INVALID_PARAMETER;
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

      errFlag = pm_comm_read_byte_mask(SLAVE_ID, int_sts, mask, &data, 0);
      *status = data ? TRUE : FALSE;
   }

   return errFlag;
}


pm_err_flag_type pm_schg_dc_set_cmd_il(uint32 device_index, pm_schg_dc_cmd_il_type cmd_il_type, boolean set_value)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data = (set_value)? 0xFF : 0x00;
   pm_register_data_type mask = 0x1 << cmd_il_type;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

   if (NULL == schg_ptr)
   {
      errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (cmd_il_type >= PM_SCHG_DC_CMD_IL_TYPE_INVALID)
   {
      errFlag =  PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else
   {
      pm_register_address_type cmd_il = ADDRESS(cmd_il);

      errFlag = pm_comm_write_byte_mask(SLAVE_ID, cmd_il, mask, data, 0);
   }
   return errFlag;
}


pm_err_flag_type pm_schg_dc_set_usbin_current_limit_cfg(uint32 device_index, uint32 dcin_current_limit)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data = 0x0;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);


   if (NULL == schg_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      if (dcin_current_limit>PM_SCHG_DC_DCIN_MAX_CURRENT_LIMIT_CFG_MA)
      {
         data = 0x78;
      }
      else
      {
         data = (dcin_current_limit+ (PM_SCHG_DC_DCIN_CURRENT_LIMIT_CFG_STEP_SIZE/2))/PM_SCHG_DC_DCIN_CURRENT_LIMIT_CFG_STEP_SIZE;
      }
      pm_register_address_type dcin_current_limit_cfg = ADDRESS(dcin_current_limit_cfg);

      err_flag = pm_comm_write_byte(SLAVE_ID, dcin_current_limit_cfg, data, 0);
   }

   return err_flag;
}

pm_err_flag_type pm_schg_dc_set_zin_icl(uint32 device_index, pm_schg_dc_zin_icl_res_type zin_icl_res_type, uint32 ilmit_level_ma)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type data = 0x0;
   pm_register_address_type zin_icl = 0x0;
   pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);


   if (NULL == schg_ptr)
   {
      err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (zin_icl_res_type>=PM_SCHG_DC_ZIN_ICL_INVALID)
   {
      err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else
   {
	  if (ilmit_level_ma>PM_SCHG_DC_DCIN_MAX_CURRENT_LIMIT_CFG_MA)
      {
         data = 0x78;
      }
	  else
	  {
	     data = ilmit_level_ma/25;
	  }
	  
	  switch(zin_icl_res_type)
	  {
	  case PM_SCHG_DC_ZIN_ICL_PT:
		 zin_icl = ADDRESS(zin_icl_pt);
         err_flag = pm_comm_write_byte(SLAVE_ID, zin_icl, data, 0);
		 break;
	  case PM_SCHG_DC_ZIN_ICL_LV:
		 zin_icl = ADDRESS(zin_icl_lv);
         err_flag = pm_comm_write_byte(SLAVE_ID, zin_icl, data, 0);
		 break;
	  case PM_SCHG_DC_ZIN_ICL_HV:
		 zin_icl = ADDRESS(zin_icl_hv);
         err_flag = pm_comm_write_byte(SLAVE_ID, zin_icl, data, 0);
		 break;
	  case PM_SCHG_DC_ZIN_ICL_PT_HV:
		 zin_icl = ADDRESS(zin_icl_pt_hv);
         err_flag = pm_comm_write_byte(SLAVE_ID, zin_icl, data, 0);
		 break;
	  case PM_SCHG_DC_ZIN_ICL_MID_LV:
		 zin_icl = ADDRESS(zin_icl_mid_lv);
         err_flag = pm_comm_write_byte(SLAVE_ID, zin_icl, data, 0);
		 break;
	  case PM_SCHG_DC_ZIN_ICL_MID_HV:
		 zin_icl = ADDRESS(zin_icl_mid_hv);
         err_flag = pm_comm_write_byte(SLAVE_ID, zin_icl, data, 0);
		 break;
	  default:
         err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;		  
	  }

   }

   return err_flag;
}

pm_err_flag_type pm_schg_dc_set_ivref_otg_ss(uint32 device_index, uint8 ivref_fpfet_curLimit)
{
	pm_err_flag_type    err_flag = PM_ERR_FLAG_SUCCESS;
	pm_schg_data_type *schg_ptr = pm_schg_get_data(device_index);
	uint8 mask = PM_SCHG_DC_IVREF_OTG_SS_MASK;
	UINT8 data = ivref_fpfet_curLimit;

	if (NULL == schg_ptr)
	{
		err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
	}
	else
	{
		pm_register_address_type dc_ivref_otg_ss_addr = ADDRESS(eng_ssupply_cfg2);
		err_flag = pm_schg_dc_unlock_perph_write(schg_ptr);
		err_flag |= pm_comm_write_byte_mask(SLAVE_ID, dc_ivref_otg_ss_addr, mask, data, 0);
	}

	return err_flag;
}
