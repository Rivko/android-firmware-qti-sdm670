/*! \file pm_sbl_boot.c
*  \n
*  \brief This file contains PMIC device initialization function where initial PMIC
*  \n SBL settings are configured through the PDM auto-generated code.
*  \n
*  \n &copy; Copyright 2013-2017 Qualcomm Technologies Inc, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/17   czq     Use Spare reg PON_PERPH_RB_SPARE instead of PON_DVDD_RB_SPARE
08/01/17   ks      Updated pm_sbl_chg_init() to call updated temp monitoring function
02/17/17   pxm     Called function to check if auto power on. 
01/11/17   pxm     add support for PBS triggered AFP
12/27/16   al      Porting fix to set LDO5 voltage with micro volt's.  
10/21/16   akm     Added pm_get_pon_reason_history, pm_pbs_header_info_init in pm_sbl_pre_config
10/07/16   aab     Call pm_sbl_config_skin_temp_threshold() for targets that require charging
09/13/16   aab     Updated logging text 
08/25/16   pxm     Config skin temperature threshold for all target
07/31/16   aab     Added num spmi transaction conditional logging
07/14/16   aab     Updated to support target with out PMI8998
06/02/16   aab     Updated pm_device_programmer_init()
03/23/16   aab     Updated pm_config_ssc_active_and_retention_level()
02/18/16   aab     Updated pm_sbl_chg_init()
02/18/16   aab     Added pm_system_init()
02/18/16   aab     Renamed pm_set_ssc_active_and_retention_levels_mv()  to pm_config_ssc_active_and_retention_level() 
02/05/16   aab     Added pm_set_ssc_active_and_retention_levels_mv()
01/25/16   aab     Updated pm_device_init() and pm_sbl_chg_init()to support RUMI targets with out PMIC 
12/22/15   aab     Added Support for PMI8998 SBL Charging
12/10/15   aab     Added Cx/Mx pvs_retention_data[]  
12/04/15   aab     Updated to support msm8998
10/14/15   aab     Branch from 8996 target
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_boot.h"
#include "pm_ldo.h"
#include "pm_sbl_boot.h"
#include "pm_device.h"
#include "pm_config_sbl.h"
#include "pm_config_sbl_test.h"
#include "pm_app_chgr.h"
#include "pm_app_chgr_common.h"
#include "pm_pbs_info.h"
#include "pm_target_information.h"
#include "pm_sbl_boot_target.h"
#include "pm_log_utils.h"
#include "pm_pbs.h"
#include "DALDeviceId.h"
#include "DDIPlatformInfo.h"
#include "DDIChipInfo.h"
#include "pmio_pon.h"
#include "CoreVerify.h"
#include "railway.h"

/*===========================================================================

                     PROTOTYPES 

===========================================================================*/
static pm_err_flag_type pm_sbl_pre_config(void);
static pm_err_flag_type pm_sbl_target_detect(void);
static pm_err_flag_type pm_config_wlan_active_and_retention_level(void);


/*===========================================================================

                        GLOBALS and TYPE DEFINITIONS 

===========================================================================*/
static boolean pm_ram_image_loaded_flag = FALSE;
static boolean verbose_log = FALSE;
static pm_sbl_specific_data_type      *sbl_param_ptr     = NULL;
static pm_sbl_schg_specific_data_type *sbl_chg_param_ptr = NULL;

#define PMI_INDEX 0

/*===========================================================================

                        FUNCTION DEFINITIONS 

===========================================================================*/
pm_err_flag_type 
pm_device_init ( void )
{
   static pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   uint32 initial_num_spmi_trans = pm_log_num_spmi_transaction(0, FALSE);

   err_flag |= pm_device_setup();

   //PMICs are Not currently supported in RUMI
   uint8 pmic_index = (uint8) PMIC_A_SLAVEID_PRIM / 2;
   pm_model_type pmic_model = PMIC_IS_INVALID;
   pmic_model = pm_get_pmic_model(pmic_index);
   if((pmic_model == PMIC_IS_UNKNOWN) || (pmic_model == PMIC_IS_INVALID))
   {
       return err_flag = PM_ERR_FLAG_SUCCESS; //Temp Fix:  PMICs are Not currently supported in RUMI
   }

   pm_target_information_init();
   pm_comm_info_init();

   err_flag |= pm_device_pre_init();

   err_flag |= pm_pon_init();

   err_flag |= pm_pbs_info_rom_init();  /* Read PBS INFO for the pmic rom devices */

   err_flag |= pm_sbl_pre_config(); /* SBL Pre Configuration */

   err_flag |= pm_sbl_config(); /* SBL Configuration */
   if (err_flag == PM_ERR_FLAG_SUCCESS)
   {
      pm_ram_image_loaded_flag = TRUE;
   }

   err_flag |= pm_sbl_config_test(); /* SBL Configuration validation, only executes complete code if spare reg 0x88F bit 0 is set*/

   err_flag |= pm_pbs_info_ram_init();  /* Read PBS INFO for the pmic ram devices */

   err_flag |= pm_pbs_ram_version_validation_test(); /* PBS RAM Version validation, only executes complete code if spare reg 0x88F bit 0 is set*/
   
   err_flag |= pm_device_post_init(); /* Initialize PMIC with the ones PDM can not perform */

   pm_log_num_spmi_transaction(initial_num_spmi_trans, verbose_log);
   
  //Write to Spare bit for pm_device_init_status
   if(err_flag == PM_ERR_FLAG_SUCCESS)
   {
       err_flag = pm_comm_write_byte_mask(PMIC_A_SLAVEID_PRIM, PMIO_PON_PERPH_RB_SPARE_ADDR, PON_PERPH_RB_SPARE_DEVICE_INIT_MASK,PON_PERPH_RB_SPARE_DEVICE_INIT_MASK, 0); 
   }

   return err_flag; /* NON ZERO return means an ERROR */
}



pm_err_flag_type
pm_sbl_chg_init (void)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   boolean debug_board_present      = FALSE;
   uint32 initial_num_spmi_trans = pm_log_num_spmi_transaction(0, FALSE);
   pm_app_chg_temp_limit_config_type *temp_limit_info_ptr = NULL;
   temp_limit_info_ptr = (pm_app_chg_temp_limit_config_type*)pm_target_information_get_specific_info(PM_PROP_APP_CHG_TEMP_LIMIT_CONFIG_DATA);
   if(temp_limit_info_ptr == NULL)
   {
      return PM_ERR_FLAG_INVALID_PARAMETER; 
   }
  
   uint32 device_index = 0; //Charger on PM660
   DalPlatformInfoPlatformType platform_type = DALPLATFORMINFO_TYPE_UNKNOWN;
   pm_model_type pmic_model = PMIC_IS_UNKNOWN;
   uint8 primary_pmic_index = 0;
  
   if(!(pm_driver_init_status()))
   {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;	 
   }
  
   platform_type = DalPlatformInfo_Platform();
   
   //Check if PMIC is supported in RUMI
   primary_pmic_index = (uint8) PMIC_A_SLAVEID_PRIM / 2;
   pmic_model = pm_get_pmic_model(primary_pmic_index);
   if((pmic_model == PMIC_IS_UNKNOWN) || (pmic_model == PMIC_IS_INVALID))
   {
      return err_flag = PM_ERR_FLAG_SUCCESS; //Temp Fix:  PMICs are Not currently supported in this RUMI
   }
  
   pmic_model = pm_get_pmic_model(device_index);
   if( ((platform_type == DALPLATFORMINFO_TYPE_MTP)
     || (platform_type == DALPLATFORMINFO_TYPE_FLUID)
     || (platform_type == DALPLATFORMINFO_TYPE_LIQUID)
     || (platform_type == DALPLATFORMINFO_TYPE_QRD)) &&
      ( (pmic_model == PMIC_IS_PM660)) )
   {
      err_flag |= pm_sbl_chg_pre_init();

      //WA: [660]WA for Erroneous USBIN auto-power-on(CR2005474)
      err_flag |= pm_app_auto_power_on_check(device_index);
      if(err_flag != PM_ERR_FLAG_SUCCESS )
      {
         return err_flag;
      }
      err_flag |= pm_sbl_enable_batt_detection(device_index);
      if(err_flag != PM_ERR_FLAG_SUCCESS )
      {
         return err_flag;
      }
      CORE_VERIFY_PTR(sbl_chg_param_ptr);
      err_flag |= pm_sbl_fake_batt_id_range_config(device_index, sbl_chg_param_ptr);// If 7.5k battery id needs to charge then the default fake battery range needs to modified  
      if(err_flag != PM_ERR_FLAG_SUCCESS )
      {
         return err_flag;
      }
      err_flag |= pm_sbl_get_debug_board_presence(device_index, sbl_chg_param_ptr, &debug_board_present);//Check debug board presence
      if(err_flag != PM_ERR_FLAG_SUCCESS )
      {
         return err_flag;
      }
      if( debug_board_present == TRUE)
      {//BOOTUP: Since Debug Board is being used
         return err_flag;
      }

      err_flag |= pm_app_chgr_enable_temp_monitoring(temp_limit_info_ptr->hot_limit, temp_limit_info_ptr->cold_limit);
      err_flag |= pm_sbl_config_chg_parameters(device_index);
      err_flag |= pm_sbl_config_jetta_threshold(device_index);
      err_flag |= pm_sbl_config_thermister_coefficent(device_index);
      err_flag |= pm_sbl_config_charger_and_skin_temp_threshold(device_index);
      err_flag |= pm_sbl_config_therm_ground_sel(device_index);

      err_flag |= pm_sbl_chg_check_weak_battery_status(device_index);
      if(err_flag != PM_ERR_FLAG_SUCCESS)
      {
         pm_log_message("ERROR: pm_sbl_chg_check_weak_battery_status()");
      }		

      err_flag |= pm_sbl_chg_post_init();  
   }
  
   if (verbose_log)
   {//Log total heap mem used by PMIC XBL loader driver
      pm_log_message("PMIC Total Heap MEM Size: %d", pm_memory_allocated());
   }
   pm_log_num_spmi_transaction(initial_num_spmi_trans, verbose_log);
  
   return err_flag;
}



pm_err_flag_type
pm_device_programmer_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  
  err_flag |= pm_device_setup();
  pm_target_information_init();
  pm_comm_info_init();

  err_flag |= pm_pon_init();
    
  return err_flag;
}



static pm_err_flag_type
pm_sbl_pre_config(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  if (sbl_param_ptr == NULL)
  {
     sbl_param_ptr = (pm_sbl_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_SPECIFIC_DATA);
  }
  CORE_VERIFY_PTR(sbl_param_ptr);

  if (sbl_chg_param_ptr == NULL)
  {
     sbl_chg_param_ptr = (pm_sbl_schg_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_CHG_SPECIFIC_DATA);
  }
  CORE_VERIFY_PTR(sbl_chg_param_ptr);

  verbose_log = sbl_param_ptr->verbose_uart_logging;
  err_flag |= pm_log_pon_reasons(verbose_log);

  err_flag |= pm_sbl_target_detect();

  err_flag = pm_pbs_header_info_init();
  	
  pm_pbs_get_pon_reason_hist_dataset();
  
  return err_flag;
}



pm_err_flag_type
pm_sbl_target_detect(void) /* SBL Target detect */
{
   static pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS; 

   return err_flag;
}



pm_err_flag_type pm_system_init(void)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

   if(pm_get_pmic_model(PMI_INDEX) == PMIC_IS_PM660)
   {
     err_flag = pm_config_wlan_active_and_retention_level(); 
   }

   return err_flag;
}



static pm_err_flag_type 
pm_config_wlan_active_and_retention_level(void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    uint32 retention_voltage_mv = 0x00;
    uint8  retention_level[2] = {0};
    uint32 active_voltage_mv = 0x00;
    uint8  active_level[2] = {0};
    uint8  rail = rail_id("vdda_wlan");
    uint8 primary_pmic_index = 0;

    //Set Retention Voltage
    retention_voltage_mv = railway_get_corner_voltage(rail,  RAILWAY_RETENTION) / 1000; //get Retention voltage level
    retention_level[0]  =  (uint8)( retention_voltage_mv & 0xFF);
    retention_level[1]  =  (uint8)((retention_voltage_mv >> 8) & 0xFF);
    err_flag |= pm_comm_write_byte_array(MISC_SLAVE_ADDR, MISC_VSET1_L5_RET_ADDR_LB, 2, retention_level, 1);

    //Set Active Voltage
    active_voltage_mv = railway_get_corner_voltage(rail,  RAILWAY_NOMINAL ) / 1000; //get Active voltage level
    active_level[0] = (uint8)( active_voltage_mv & 0xFF);      
    active_level[1] = (uint8)((active_voltage_mv >> 8) & 0xFF);
    err_flag |= pm_comm_write_byte_array(MISC_SLAVE_ADDR, MISC_VSET2_L5_ACTIVE_ADDR_LB, 2, active_level, 1);

    //Configure L5A with Active Level
    err_flag |= pm_ldo_volt_level(primary_pmic_index, PM_LDO_5, active_voltage_mv * 1000);

    return err_flag;
}



pm_err_flag_type 
pm_check_pbs_ram_warm_reset_seq_presence(uint32 device_index)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    return err_flag;
}

