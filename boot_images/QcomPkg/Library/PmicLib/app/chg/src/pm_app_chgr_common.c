/*! \file
*  
*  \brief  pm_app_chgr_common.c
*  \details Implementation file for pmic temperature monitoring functions commonly used by XBL loader and XBL core.
*    
*  \n &copy; Copyright (c)2017  by QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================
                                Edit History
This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/17   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_app_chgr_common.h"
#include "CoreVerify.h"
#include "pm_pbs_client.h"
#include "pm_comm.h"
#include "pm_target_information.h"
#include "pm_fg_batt_soc.h"


/*===========================================================================

                     FUNCTION IMPLEMENTATION 

===========================================================================*/
pm_err_flag_type
pm_app_chgr_enable_temp_monitoring(uint8 hot_limit_val, uint8 cold_limit_val)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   uint32 slave_id;
   boolean fg_enable_sts = FALSE;
   pm_app_chg_temp_monitor_config_type *temp_monitor_info_ptr = NULL;
   
   temp_monitor_info_ptr = (pm_app_chg_temp_monitor_config_type*)pm_target_information_get_specific_info(PM_PROP_APP_CHG_TEMP_MONITOR_CONFIG_DATA);
   if(temp_monitor_info_ptr == NULL)
   {
      return PM_ERR_FLAG_INVALID_PARAMETER; 
   }
   
   err_flag |= pm_fg_get_batt_soc_enable_ctl( temp_monitor_info_ptr->pmic_index, &fg_enable_sts);
   if(FALSE == temp_monitor_info_ptr->enable_afp )
   {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;  //if AFP config disabled then skip PBS AFP.
   }
   
   err_flag = pm_get_slave_id(temp_monitor_info_ptr->pmic_index, temp_monitor_info_ptr->limit_reg_sid_index, &slave_id);
   if (err_flag != PM_ERR_FLAG_SUCCESS)
   {
      return err_flag;
   }

   //Config hot/cold thresholds
   err_flag |= pm_comm_write_byte(slave_id, temp_monitor_info_ptr->hot_limit_reg_addr,  hot_limit_val, 0);
   err_flag |= pm_comm_write_byte(slave_id, temp_monitor_info_ptr->cold_limit_reg_addr, cold_limit_val, 0);

   //Enable AFP PBS Sequence
   err_flag |= pm_pbs_client_sw_trigger(temp_monitor_info_ptr->pmic_index, temp_monitor_info_ptr->client, temp_monitor_info_ptr->client_trig_num, temp_monitor_info_ptr->pbs_afp_enable_trigger_arg_value, NULL);
   
   return err_flag; 
}


