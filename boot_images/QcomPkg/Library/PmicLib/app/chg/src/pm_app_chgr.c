/*! \file
*  
*  \brief  pm_app_schgr.c
*  \details Implementation file for pmic sbl charging algorithm
*    
*  \n &copy; Copyright (c)2016-2017  by QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================
                                Edit History
This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/07/18   ks      updated pm_sbl_schg_config_red_led api
01/03/18   st      Moved pm_app_auto_power_on_check api to pm_sbl_bool_oem.c
10/12/17   sv      Updated charging RGB led api to support different targets.
07/19/17   aab     Remove pm_app_chgr_set_hw_afp_thresholds(), moved functionality to common code
06/28/17   sv      Updated pm_app_auto_power_on_check API name
04/25/17   sv      Updated pm_sbl_auto_power_on_check with pon reason.
07/10/17   pxm     Move PmicFgSram_Init to pm_sbl_bool_oem.c. 
03/15/17   aab     Remove bat id conversion config from pm_sbl_enable_batt_detection()
03/14/17   pxm     Need Manually setting DCP ICL to 1500mA. 
02/17/17   pxm     Added function to check if auto power on.
02/02/17   cs      SBL needs to set the batt id missing threshold to 0 to get max threshold.
01/19/17   cs      let UEFI enable battery missing detection to add more delay before enable batt missing.
01/11/17   pxm     add support for PBS triggered AFP
12/16/16   pxm     Added fake batt_id configuration function
10/20/16   aab     Added Batt_therm Out of bound temperature check
10/13/16   pxm     Pet (reset) BARK and BITE watchdog timers in charging loop
09/30/16   aab     Updated logging:  Used  pm_log_messag() macro
08/25/16   pxm     Add skin temperature threshold configuration function 
09/17/16   aab     Updated pm_sbl_schg_get_charger_progress_status() to latest WiPower Algorithm
09/17/16   aab     Removed toggling of SHDN_N_CLEAR_CMD Reg when DC_IN path detected 
08/12/16   aab     Added debugboard detection support 
08/02/16   yps     Fixed compiling issue for Clang38
07/26/16   aab     Updated pm_sbl_schg_config_red_led()
07/07/16   aab     Updated to support saving Entry Vbatt/SOC state to smem
05/27/16   pxm     Implement wipower power customization
03/15/16   aab     Driver update (Support for PMI8998)
02/16/16   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "com_dtypes.h"
#include "boothw_target.h"
#include "boot_api.h"
#include "CoreVerify.h"

#include "pm_chg.h"
#include "pm_app_chgr.h"
#include "pm_schg_chgr.h"
#include "pm_schg_batif.h"
#include "pm_schg_dc.h"
#include "pm_schg_misc.h"
#include "pm_fg_bcl.h"
#include "pm_fg_batt_info.h"
#include "pm_fg_batt_soc.h"
#include "pm_fg_sram.h"
#include "pm_fg_adc_rr.h"
#include "pm_pbs_client.h"

#include "pm_comm.h"
#include "pm_utils.h"
#include "pm_log_utils.h"
#include "pm_rgb.h"
#include "pm_target_information.h"
#include "smem.h"
#include "DDIAdc.h"


/*=========================================================================== 
 
                     GLOBAL TYPE DEFINITIONS
 
===========================================================================*/
#define  PM_WEAK_BATTERY_CHARGING_DELAY         500 * 1000   //500ms
#define  PM_WIPOWER_START_CHARGING_DELAY       3500 * 1000   //3.5sec
#define  PM_WEAK_BATTERY_CHARGING_AFP_DELAY     500 * 1000   //500ms
#define  PM_FAKE_BATT_ID_CFG_WAIT_POLL           50 * 1000   //50ms
#define  PM_FAKE_BATT_ID_CFG_WAIT_MAX          1200 * 1000   //1.2sec
#define  PM_JEITA_EN_OP_DELAY                  1000 * 1000   //1s

#define  PM_SBL_WP_CHG_POWER_MAX_UW           20000000
#define  PM_SBL_WP_CHG_POWER_MIN_UW           5000000
#define  PM_SBL_DC_ZIN_ICL_PT_VOLT_MV         8000
#define  PM_SBL_DC_ZIN_ICL_PT_HV_VOLT_MV      9000
#define  PM_SBL_DC_ZIN_ICL_LV_VOLT_MV         5500
#define  PM_SBL_DC_ZIN_ICL_MID_LV_VOLT_MV     6500
#define  PM_SBL_DC_ZIN_ICL_MID_HV_VOLT_MV     8000
#define  PM_SBL_DC_ZIN_ICL_HV_VOLT_MV         11000

static pm_sbl_schg_vlowbatt_threshold_data_type pm_dbc_bootup_volt_threshold;
static boolean verbose_chg_log = TRUE;        //TBD: Needs update sbl charging logging option.  Probably need to use boot-profiler which keeps log messages default off 
static pm_sbl_schg_info_type schg_info;
static pm_schg_usb_apsd_result_status_type chgr_src_detected;
static uint64 wipower_chg_wait_time_ms = 0x00;
static uint32 batt_id_ohms = 0;
static boolean batt_therm_coeff_cfg_flag = FALSE;
static boolean icl_flag = FALSE;

/*===========================================================================

                     PROTOTYPES 

===========================================================================*/
pm_err_flag_type pm_sbl_schg_get_charger_path(uint32 device_index, pm_schg_misc_pwr_pth_type* charger_path);
pm_err_flag_type pm_sbl_schg_check_jetta_temp_limit_status(uint32 device_index, boolean *jetia_hard_limit_status);
pm_err_flag_type pm_sbl_schg_config_charger(uint32 device_index, boolean chg_prog_message_flag);
pm_err_flag_type pm_sbl_schg_get_charger_progress_status(uint32 device_index, pm_schg_misc_pwr_pth_type chgr_power_path,
                                                         pm_sbl_schg_wipower_specific_data_type *wipower_chg_param_ptr,
                                                         boolean *chg_status);
pm_err_flag_type pm_sbl_schg_config_red_led( boolean toggle_led, boolean delay);
pm_err_flag_type pm_sbl_config_wipower_chg_ilimit(uint32 device_index, uint32 wipower_chg_power_uw);
pm_err_flag_type pm_sbl_schg_info_init(uint32 device_index);
pm_err_flag_type pm_app_chgr_check_batt_temp_limits(uint32 device_index, pm_sbl_schg_batt_therm_type *batt_therm_ptr);
pm_err_flag_type pm_sbl_schg_set_icl(uint32 device_index);


/*===========================================================================

                     FUNCTION IMPLEMENTATION 

===========================================================================*/
pm_err_flag_type pm_sbl_chg_check_weak_battery_status(uint32 device_index)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

   static pm_sbl_schg_specific_data_type *chg_param_ptr = NULL;
   static pm_sbl_schg_wipower_specific_data_type *wipower_chg_param_ptr = NULL;
   static pm_sbl_schg_batt_therm_type             *batt_therm_ptr = NULL;

   uint32  vbat_adc = 0;
   uint32  entry_vbat_adc = 0;
   int32   ibat_adc = 0;
   uint32  bootup_threshold;
   boolean apsd_done = FALSE;

   boolean vbatt_weak_status        = TRUE;
   boolean toggle_led               = FALSE;
   boolean bat_missing              = FALSE;
   boolean chg_progress_status      = FALSE;
   boolean jetia_hard_limit_status  = FALSE;
   boolean chg_prog_message_flag    = FALSE;
   boolean usbin_present            = FALSE;
   boolean cold_bat_hard_lim_rt_sts = FALSE;
   pm_schg_misc_pwr_pth_type  charger_power_path = PM_SCHG_MISC_POWER_PATH__INVALID;

   chg_param_ptr = (pm_sbl_schg_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_CHG_SPECIFIC_DATA);
   CORE_VERIFY_PTR(chg_param_ptr);
   verbose_chg_log = chg_param_ptr->verbose_uart_logging;

   wipower_chg_param_ptr = (pm_sbl_schg_wipower_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_CHG_WIPOWER_SPECIFIC_DATA);
   CORE_VERIFY_PTR(wipower_chg_param_ptr);

   batt_therm_ptr = (pm_sbl_schg_batt_therm_type*)pm_target_information_get_specific_info(PM_PROP_SBL_CHG_BATT_THERM_CONFIG_DATA);
   CORE_VERIFY_PTR(batt_therm_ptr);


   if(chg_param_ptr->dbc_bootup_volt_threshold.enable_config == PM_ENABLE_CONFIG)
   {
      //Configure Vlowbatt threshold: Used by PMI on next bootup
      pm_dbc_bootup_volt_threshold = (pm_sbl_schg_vlowbatt_threshold_data_type)chg_param_ptr->dbc_bootup_volt_threshold;
      err_flag  |= pm_sbl_chg_config_vbat_low_threshold(device_index, pm_dbc_bootup_volt_threshold); 
   }

   //Check Battery presence
   err_flag |= pm_schg_batif_irq_status(device_index, PM_SCHG_BATIF_BAT_THERM_OR_ID_MISSING, PM_IRQ_STATUS_RT, &bat_missing);
   if( bat_missing == TRUE )
   {
      pm_log_message("BOOTUP, NO Battery"); 
      return err_flag;
   }

   err_flag |= pm_schg_misc_get_power_path(device_index, &charger_power_path);
   if (charger_power_path == PM_SCHG_MISC_POWER_PATH__DCIN)
   {
      bootup_threshold = chg_param_ptr->wipwr_bootup_battery_theshold_mv;
   }
   else
   {
      bootup_threshold = chg_param_ptr->bootup_battery_theshold_mv;
   }

   err_flag |= pm_fg_bcl_enable_ctl(device_index, TRUE); //Enable BCL Fule Gauge Algorithm
   err_flag |= pm_fg_batt_soc_enable_fg_algorithm(device_index, TRUE);  

   err_flag |= pm_schg_usb_set_usbin_adptr_allowance(device_index, PM_SCHG_USB_USBIN_ADAPTER_ALLOW_5V_TO_9V); 
   if ( err_flag != PM_ERR_FLAG_SUCCESS )  
   { 
      return err_flag;
   }


   err_flag |= pm_fg_batt_info_get_vbatt(device_index, &schg_info.sbl_entry_mV);
   entry_vbat_adc = (uint32)schg_info.sbl_entry_mV;
   if(verbose_chg_log)
   {
      pm_log_message("EntryVbatt: %d",entry_vbat_adc);
   }

   err_flag |= pm_sbl_schg_info_init(device_index);
   if ( err_flag != PM_ERR_FLAG_SUCCESS )  
   { 
      return err_flag;
   }

   //APSD rerun
   err_flag |= pm_schg_usb_get_apsd_result_status(device_index, &chgr_src_detected); 

   if ( err_flag != PM_ERR_FLAG_SUCCESS )   
   {    
      if(chgr_src_detected == PM_SCHG_USB_APSD_RESULT_STATUS_INVALID) 

      return err_flag; 
   } 
   
    pm_log_message("ADSP result: %d", chgr_src_detected); 
  
   //Get APSD Done status 
   err_flag |= pm_schg_usb_get_apsd_status(device_index, PM_SCHG_USB_APSD_STATUS_APSD_DTC_STATUS_DONE, &apsd_done); 
   if (FALSE == apsd_done) 
   { 
      pm_log_message("APSD in progress", 0); 
   } 
   else if(chgr_src_detected == PM_SCHG_USB_APSD_RESULT_STATUS_INVALID)
   {
      err_flag |= pm_schg_usb_irq_status(device_index, PM_SCHG_USB_IRQ_USBIN_PLUGIN, PM_IRQ_STATUS_RT, &usbin_present);
      if(usbin_present == TRUE) 
      {
		 pm_log_message("APSD re-run", 0);
         err_flag |= pm_schg_usb_set_cmd_apsd(device_index, PM_SCHG_USB_APSD_RERUN, TRUE);
      }
      if ( err_flag != PM_ERR_FLAG_SUCCESS )  
      { 
         return err_flag;
      }
   }

   //Battery over-discharge and short circuit WA
   err_flag |= pm_fg_batt_info_get_batt_temp_sts(device_index, PM_FG_BATT_INFO_JEITA_TOO_COLD_STS, &cold_bat_hard_lim_rt_sts);
   if(cold_bat_hard_lim_rt_sts)
   {
      err_flag |= pm_schg_chgr_jeita_en_cfg(device_index, PM_SCHG_CHGR_JEITA_HARD_LIMIT, FALSE);
      err_flag |= pm_busy_wait(PM_JEITA_EN_OP_DELAY);
      err_flag |= pm_schg_chgr_jeita_en_cfg(device_index, PM_SCHG_CHGR_JEITA_HARD_LIMIT, TRUE);
   }       

   while( vbatt_weak_status == TRUE )  //While battery is in weak state
   {
      //set ICL and APSD rerun
      if(icl_flag == FALSE)
      {
          err_flag |= pm_sbl_schg_set_icl(device_index);
      }

      //pet charger WDOG
      err_flag |= pm_schg_misc_bark_bite_wdog_pet(device_index);

      //Check batt_therm Temp out of bound condition
      err_flag |= pm_app_chgr_check_batt_temp_limits(device_index, batt_therm_ptr);

      err_flag |= pm_fg_batt_info_get_vbatt(device_index, &vbat_adc);
      err_flag |= pm_fg_batt_info_get_ibatt(device_index, &ibat_adc);
      if ( err_flag != PM_ERR_FLAG_SUCCESS )  
      { 
          return err_flag;
      }
   
      //Check if JEITA check is enabled
      if (chg_param_ptr->enable_jeita_hard_limit_check == TRUE)
      {
         err_flag |= pm_sbl_schg_check_jetta_temp_limit_status(device_index, &jetia_hard_limit_status);
         if (jetia_hard_limit_status == TRUE )  
         {
            err_flag |= pm_busy_wait(PM_WEAK_BATTERY_CHARGING_DELAY); 
            pm_log_message("Vbatt: %d;  JETTA Temp limit exceeded...", vbat_adc);
         }
      }

      //Check if ADC reading is within limit
      if ( vbat_adc >=  bootup_threshold)  //Compare it with SW bootup threshold
      {
         vbatt_weak_status = FALSE;
         break; //BOOTUP
      }

      pm_log_message("Vbatt: %d; Ibatt: %d", vbat_adc, ibat_adc);

      err_flag |= pm_sbl_schg_config_charger(device_index, chg_prog_message_flag);   //Detect charger type and configure

      err_flag |= pm_sbl_schg_get_charger_progress_status(device_index, charger_power_path, wipower_chg_param_ptr, &chg_progress_status);
      if (chg_progress_status == FALSE)
      {
         pm_log_message("ERROR, Charging is NOT in progress: Shutting Down");
         boot_hw_powerdown();
      }
      else
      {
         pm_log_message("SBL Charging in progress....");
         chg_prog_message_flag = TRUE;
      }

      //Toggle Red LED
      toggle_led = (toggle_led == FALSE)?TRUE:FALSE;
      err_flag |= pm_sbl_schg_config_red_led(toggle_led, TRUE);

   }//while

   toggle_led = FALSE;
   err_flag |= pm_sbl_schg_config_red_led(toggle_led, FALSE); //Ensure LED is turned of at the end of SBL charging

   if (err_flag != PM_ERR_FLAG_SUCCESS)
   {
      pm_log_message("ERROR, In SBL Charging ...");
   }
   else if (chg_prog_message_flag == TRUE)
   {
      pm_log_message("ExitVbatt: %d", vbat_adc);
      if(verbose_chg_log)
      {
         pm_log_message("SBL Charging completed.");
      }
   }

   return err_flag; 
}


pm_err_flag_type
pm_sbl_schg_set_icl(uint32 device_index)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   boolean usbin_present            = FALSE;
   boolean apsd_done                = FALSE;

   //get APSD and usbin status
   err_flag |= pm_schg_usb_irq_status(device_index, PM_SCHG_USB_IRQ_USBIN_PLUGIN, PM_IRQ_STATUS_RT, &usbin_present);
   err_flag |= pm_schg_usb_get_apsd_status(device_index, PM_SCHG_USB_APSD_STATUS_APSD_DTC_STATUS_DONE, &apsd_done);
   
   if((usbin_present == TRUE) && (apsd_done == TRUE))
   {
      //get charger type   
      err_flag |= pm_schg_usb_get_apsd_result_status(device_index, &chgr_src_detected); 

      //setting USB high current mode and using SW to control ICL 
      err_flag |= pm_schg_usb_configure_usbin_icl_options(device_index, PM_SCHG_USB_ICL_OPTIONS_USBIN_MODE_CHG, TRUE);
      err_flag |= pm_schg_usb_enable_icl_override_after_apsd(device_index, TRUE);
       
      if((chgr_src_detected != PM_SCHG_USB_APSD_RESULT_STATUS_SDP_CHARGER)   &&
         (chgr_src_detected != PM_SCHG_USB_APSD_RESULT_STATUS_FLOAT_CHARGER) &&
         (chgr_src_detected != PM_SCHG_USB_APSD_RESULT_STATUS_INVALID))
      {               
         err_flag |= pm_schg_usb_set_usbin_current_limit_cfg(device_index, 1500);
         pm_log_message("Manually set ICL 1500mA");              
      }
      else
      {
         pm_log_message("Manually set ICL 500mA");
      }

      icl_flag = TRUE;
   }

   if (err_flag != PM_ERR_FLAG_SUCCESS)
   {
      pm_log_message("ERROR, pm_sbl_schg_set_icl()");
      return err_flag;
   }

   return err_flag;
}

pm_err_flag_type 
pm_sbl_fake_batt_id_range_config(uint32 device_index, pm_sbl_schg_specific_data_type *chg_param_ptr)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   pm_schg_chgr_status_type vbatt_chging_status;
   uint32 fake_batt_id_min_ohms = 0;
   uint32 fake_batt_id_max_ohms = 0;
   uint32 fake_batt_id_waitt_us = 0;

   CORE_VERIFY_PTR(chg_param_ptr);

   //Check if enable fake battery id configuration 
   if(chg_param_ptr->fake_batt_id_range.enable_config == PM_ENABLE_CONFIG)
   {
      fake_batt_id_min_ohms = chg_param_ptr->fake_batt_id_range.min_value;
      fake_batt_id_max_ohms = chg_param_ptr->fake_batt_id_range.max_value;
      
      if(fake_batt_id_max_ohms > PM_FG_ADC_RR_FG_FAKE_BATT_MAX || fake_batt_id_max_ohms < fake_batt_id_min_ohms)
      {
         pm_log_message("ERROR: FG fake_batt_id out of range");   
      }
      else
      {
         err_flag |= pm_fg_adc_rr_set_fake_batt_id(device_index, fake_batt_id_min_ohms, fake_batt_id_max_ohms);                          
         err_flag |= pm_fg_adc_rr_batt_id_trig(device_index, TRUE);              
         err_flag |= pm_fg_adc_rr_make_sw_req(device_index);             

         //need about 200~300 ms delay                   
         do
         {
            pm_busy_wait(PM_FAKE_BATT_ID_CFG_WAIT_POLL);
            err_flag |= pm_schg_chgr_get_chgr_status(device_index, &vbatt_chging_status);
            fake_batt_id_waitt_us += PM_FAKE_BATT_ID_CFG_WAIT_POLL;
         }
         while ( vbatt_chging_status.status_5.charging_enable == FALSE && fake_batt_id_waitt_us <= PM_FAKE_BATT_ID_CFG_WAIT_MAX);

         err_flag |= pm_fg_adc_rr_batt_id_trig(device_index, FALSE);

         if (err_flag != PM_ERR_FLAG_SUCCESS)
         {
            pm_log_message("ERROR, pm_sbl_fake_batt_range_change()");
            return err_flag;
         }
      }
   }

   return err_flag; 
}



pm_err_flag_type 
pm_sbl_get_debug_board_presence(uint32 device_index, pm_sbl_schg_specific_data_type *chg_param_ptr, boolean *debug_board_present)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   boolean bat_missing       = FALSE;

   CORE_VERIFY_PTR(chg_param_ptr);
   CORE_VERIFY_PTR(debug_board_present);

   //Check Battery presence before doing batt_id ADC reading
   err_flag |= pm_schg_batif_irq_status(device_index, PM_SCHG_BATIF_BAT_THERM_OR_ID_MISSING, PM_IRQ_STATUS_RT, &bat_missing);
   if (err_flag != PM_ERR_FLAG_SUCCESS)
   {
      pm_log_message("ERROR, pm_schg_batif_irq_status()");
      return err_flag;
   }

   if( bat_missing == TRUE )
   {
      pm_log_message("BOOTUP, NO Battery"); 
      *debug_board_present = TRUE;
      return err_flag;
   }

   if ( (batt_id_ohms >=chg_param_ptr->debug_board_batt_id_pd_res_range.min_vlue) &&
        (batt_id_ohms <=chg_param_ptr->debug_board_batt_id_pd_res_range.max_value) )
   {
      *debug_board_present = TRUE;
      pm_log_message("BOOTUP, Debug Board being used"); 
   }
   else
   {
      *debug_board_present = FALSE;
   }

   return err_flag; 
}



pm_err_flag_type 
pm_sbl_enable_batt_detection(uint32 device_index)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   pm_fg_batt_info_batt_missing_cfg  batt_missing_cfg;

   //Read BatID value
   err_flag |= pm_sbl_get_batt_id(&batt_id_ohms);
   if (err_flag != PM_ERR_FLAG_SUCCESS)
   {
      pm_log_message("ERROR, pm_sbl_get_batt_id()");
   }
  
    //set batt missing ID threshold to 0
   err_flag |= pm_fg_batt_info_get_batt_missing_cfg(device_index, &batt_missing_cfg);
   batt_missing_cfg.bm_from_batt_id = FALSE;
   batt_missing_cfg.bm_batt_id_th = 0;
   err_flag |= pm_fg_batt_info_set_batt_missing_cfg(device_index, batt_missing_cfg);

   return err_flag; 
}



pm_err_flag_type pm_sbl_schg_check_jetta_temp_limit_status(uint32 device_index, boolean *jetia_hard_limit_status)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   boolean hot_bat_hard_lim_rt_sts  = FALSE;
   boolean cold_bat_hard_lim_rt_sts = FALSE;

   //Read JEITA condition
   err_flag |= pm_fg_batt_info_get_batt_temp_sts(device_index, PM_FG_BATT_INFO_JEITA_TOO_HOT_STS, &hot_bat_hard_lim_rt_sts);
   err_flag |= pm_fg_batt_info_get_batt_temp_sts(device_index, PM_FG_BATT_INFO_JEITA_TOO_COLD_STS, &cold_bat_hard_lim_rt_sts);

   if ( ( hot_bat_hard_lim_rt_sts  == TRUE ) || (cold_bat_hard_lim_rt_sts == TRUE) )  
   {
      *jetia_hard_limit_status = TRUE; 
   }
   else
   {
      *jetia_hard_limit_status = FALSE; 
   }
   return err_flag;
}



pm_err_flag_type pm_sbl_schg_config_charger(uint32 device_index, boolean chg_prog_message_flag)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

   //Check if USB charger is SDP
   err_flag |= pm_schg_usb_get_apsd_result_status(device_index, &chgr_src_detected);  
   switch(chgr_src_detected)
   {
      case PM_SCHG_USB_APSD_RESULT_STATUS_SDP_CHARGER:
      {
         pm_log_message("Charger SRC: SDP");     
      }
      break;
      case PM_SCHG_USB_APSD_RESULT_STATUS_OCP_CHARGER:
      {
         pm_log_message("Charger SRC: OCP");
      }
      break;
      case PM_SCHG_USB_APSD_RESULT_STATUS_CDP_CHARGER:
      {
         pm_log_message("Charger SRC: CDP");
      }
      break;
      case PM_SCHG_USB_APSD_RESULT_STATUS_DCP_CHARGER:
      {
         //This WA was used on PMI8994.   It may not be needed for PMI8998
         //err_flag |= pm_schg_usb_set_cmd_apsd(device_index, PM_SCHG_USB_ICL_OVERRIDE, FALSE);
         pm_log_message("Charger SRC: DCP");
      }
      break;
      case PM_SCHG_USB_APSD_RESULT_STATUS_FLOAT_CHARGER:
      {
         pm_log_message("Charger SRC: FLOAT");
      }
      break;
      case PM_SCHG_USB_APSD_RESULT_STATUS_QC_2P0:
      {
         pm_log_message("Charger SRC: QC_2P0");
      }
      break;
      case PM_SCHG_USB_APSD_RESULT_STATUS_QC_3P0:
      {
         pm_log_message("Charger SRC: QC_3P0");
      }
      break;
      default:
      {
         pm_log_message("Charger SRC: OTHER; Type: %d", chgr_src_detected);
      }
      break;
    }

    if (chg_prog_message_flag == FALSE)
    {
        //Ensure that Charging is enabled
       err_flag |= pm_schg_chgr_set_chgr_cmd_type(device_index, PM_SCHG_CHGR_CMD_CHARGING_ENABLE, TRUE);
       err_flag |= pm_busy_wait(PM_WEAK_BATTERY_CHARGING_DELAY);
    }
   return err_flag;
}


pm_err_flag_type pm_sbl_schg_get_charger_progress_status(uint32 device_index, 
                                                         pm_schg_misc_pwr_pth_type chgr_power_path,
                                                         pm_sbl_schg_wipower_specific_data_type *wipower_chg_param_ptr, 
                                                         boolean *chg_progress_status)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   pm_schg_chgr_status_type vbatt_chging_status;

   //Check if Charging in progress
   err_flag |= pm_schg_chgr_get_chgr_status(device_index, &vbatt_chging_status);
   if( vbatt_chging_status.status_5.charging_enable == FALSE )//If charging is not in progress
   {
      if (chgr_power_path == PM_SCHG_MISC_POWER_PATH__DCIN) 
      { //WiPower Charger detected
         if(wipower_chg_param_ptr->wipower_chg_wait.enable_config == PM_DISABLE_CONFIG)//if WiPower wait is disabled.
         {
            *chg_progress_status = TRUE; //wait indefinate time 
         }
         else
         { //enable_config == PM_DISABLE_CONFIG
            //Delay a specific amount of time (while looping)for charging to begin, and check charging status again prior to shutting down.
            wipower_chg_wait_time_ms += 500; //approximate time for one iteration
            if ( wipower_chg_wait_time_ms > wipower_chg_param_ptr->wipower_chg_wait.wait_time_ms )
            {
               err_flag |= pm_schg_chgr_get_chgr_status(device_index, &vbatt_chging_status);
               if(vbatt_chging_status.status_5.charging_enable == FALSE )
               {
                  *chg_progress_status = FALSE;
               }
               else
               {
                  *chg_progress_status = TRUE;
               }
            }
         }
      }
      else
      {
         *chg_progress_status = FALSE; 
      }
    }
    else
    {
       *chg_progress_status = TRUE;
    }

   return err_flag;
}



pm_err_flag_type pm_sbl_schg_config_red_led(boolean toggle_led, boolean delay)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   uint32 pmic_index =0;

   err_flag = pm_rgb_get_pmic_index(PM_RGB_CHARGER_LED, &pmic_index); 
   if(pmic_index == PMIC_IS_INVALID)
   {
     return PM_ERR_FLAG_SUCCESS;                    // no RGB present on this target
   } 
   err_flag |= pm_rgb_led_config(pmic_index, PM_RGB_1, PM_RGB_SEGMENT_R,  PM_RGB_VOLTAGE_SOURCE_VPH, PM_RGB_DIM_LEVEL_MID, toggle_led); 
   if(delay)
   {
      err_flag |= pm_busy_wait(PM_WEAK_BATTERY_CHARGING_DELAY); 
   }

   return err_flag;
}



pm_err_flag_type pm_sbl_schg_get_charger_path(uint32 device_index, pm_schg_misc_pwr_pth_type* charger_power_path)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

   boolean usbin_uv_status = TRUE;
   boolean usbin_ov_status = TRUE;
   boolean dcbin_uv_status = TRUE;
   boolean dcbin_ov_status = TRUE;

   //DC charger present, if DCIN_UV_RT_STS and DCIN_UV_RT_STS status is 0 (INT_RT_STS : 0x1410[3] and [2] == 0)
   err_flag |= pm_schg_dc_irq_status(device_index, PM_SCHG_DC_IRQ_DCIN_UV, PM_IRQ_STATUS_RT, &dcbin_uv_status);
   err_flag |= pm_schg_dc_irq_status(device_index, PM_SCHG_DC_IRQ_DCIN_OV, PM_IRQ_STATUS_RT, &dcbin_ov_status);
   
   //USB charger present, if USBIN_UV_RT_STS and USBIN_OV_RT_STS status is 0 ( INT_RT_STS : 0x1310[3] and [2] == 0)
   err_flag |= pm_schg_usb_irq_status(device_index, PM_SCHG_USB_IRQ_USBIN_UV, PM_IRQ_STATUS_RT, &usbin_uv_status);
   err_flag |= pm_schg_usb_irq_status(device_index, PM_SCHG_USB_IRQ_USBIN_OV, PM_IRQ_STATUS_RT, &usbin_ov_status);  
   
   if((dcbin_uv_status == FALSE) && (dcbin_ov_status == FALSE))
   {
      *charger_power_path = PM_SCHG_MISC_POWER_PATH__DCIN;
   }
   else if((usbin_uv_status == FALSE) && (usbin_ov_status == FALSE))
   {
      *charger_power_path = PM_SCHG_MISC_POWER_PATH__USBIN;
   }
   else
   {
      *charger_power_path = PM_SCHG_MISC_POWER_PATH__INVALID;
   }

   return err_flag;
}



pm_err_flag_type pm_sbl_chg_config_vbat_low_threshold(uint32 device_index, pm_sbl_schg_vlowbatt_threshold_data_type vlowbatt_threshold_data)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

   if (vlowbatt_threshold_data.enable_config == PM_ENABLE_CONFIG)
   {
      if (vlowbatt_threshold_data.vlowbatt_threshold  >= PM_SCHG_BATIF_LOW_BATTERY_THRESH_INVALID)
      {
         err_flag = PM_ERR_FLAG_INVALID_PERIPHERAL;
         return err_flag;
      }

      err_flag = pm_schg_batif_set_low_batt_detect_threshold(device_index, vlowbatt_threshold_data.vlowbatt_threshold);
   }
   return err_flag; 
}



pm_err_flag_type pm_sbl_config_charger_and_skin_temp_threshold(uint32 device_index)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   int skin_temp_threshold = 0;
   int charger_temp_threshold = 0;

   static pm_sbl_schg_specific_data_type *chg_param_ptr = NULL;
   
   if (chg_param_ptr == NULL)
   {
      chg_param_ptr = (pm_sbl_schg_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_CHG_SPECIFIC_DATA);
      CORE_VERIFY_PTR(chg_param_ptr);
   }
   
   //Fg skin hot threshold            
   if (chg_param_ptr->fg_skin_hot_threshold.enable_config == PM_ENABLE_CONFIG)
   {
      skin_temp_threshold = chg_param_ptr->fg_skin_hot_threshold.temp_threshold;
      
      if ((skin_temp_threshold >= PM_FG_ADC_RR_FG_CHARGER_SKIN_TEMP_MIN) && (skin_temp_threshold <= PM_FG_ADC_RR_FG_CHARGER_SKIN_TEMP_MAX))
      {
         err_flag |= pm_sbl_daladc_fg_set_temp_threshold(device_index,  ADC_FG_THRESH_SKIN_HOT, skin_temp_threshold);
      }
      else
      {
         err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
         pm_log_message("ERR: FG skin hot threshold out of range");
      }
   }

   //Fg skin too hot threshold            
   if (chg_param_ptr->fg_skin_too_hot_threshold.enable_config == PM_ENABLE_CONFIG)
   {

      skin_temp_threshold = chg_param_ptr->fg_skin_too_hot_threshold.temp_threshold;
          
      if ((skin_temp_threshold >= PM_FG_ADC_RR_FG_CHARGER_SKIN_TEMP_MIN) && (skin_temp_threshold <= PM_FG_ADC_RR_FG_CHARGER_SKIN_TEMP_MAX))
      {
         err_flag |= pm_sbl_daladc_fg_set_temp_threshold(device_index,  ADC_FG_THRESH_SKIN_TOO_HOT, skin_temp_threshold);
      }
      else
      {
         err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
         pm_log_message("ERR: FG skin too hot threshold out of range");
      }  
    }
   
    //FG charger hot threshold 
    if (chg_param_ptr->fg_charger_hot_threshold.enable_config == PM_ENABLE_CONFIG) 
    {
       charger_temp_threshold = chg_param_ptr->fg_charger_hot_threshold.temp_threshold; 
       if ((charger_temp_threshold >= PM_FG_ADC_RR_FG_CHARGER_SKIN_TEMP_MIN) && (charger_temp_threshold <= PM_FG_ADC_RR_FG_CHARGER_SKIN_TEMP_MAX)) 
       {
          err_flag |= pm_sbl_daladc_fg_set_temp_threshold(device_index, ADC_FG_THRESH_CHARGER_HOT, charger_temp_threshold); 
       }
       else
       {
          err_flag |= PM_ERR_FLAG_INVALID_PARAMETER; 
          pm_log_message("ERR: FG charger hot threshold out of range"); 
       }
    }
   
   
    //FG Charger too hot thereshold 
    if (chg_param_ptr->fg_charger_too_hot_threshold.enable_config == PM_ENABLE_CONFIG) 
    {
       charger_temp_threshold = chg_param_ptr->fg_charger_too_hot_threshold.temp_threshold; 
       if ((charger_temp_threshold >= PM_FG_ADC_RR_FG_CHARGER_SKIN_TEMP_MIN) && (charger_temp_threshold <= PM_FG_ADC_RR_FG_CHARGER_SKIN_TEMP_MAX)) 
       {
          err_flag |= pm_sbl_daladc_fg_set_temp_threshold(device_index, ADC_FG_THRESH_CHARGER_TOO_HOT, charger_temp_threshold); 
       }
       else
       {
          err_flag |= PM_ERR_FLAG_INVALID_PARAMETER; 
          pm_log_message("ERR: FG charger too hot threshold out of range"); 
       } 
    } 
    return err_flag; 
}



pm_err_flag_type pm_sbl_config_jetta_threshold(uint32 device_index)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

   static pm_sbl_schg_jetta_threshold_config_type *jetta_threshold_ptr;
   
   if(jetta_threshold_ptr == NULL)
   {
      jetta_threshold_ptr = (pm_sbl_schg_jetta_threshold_config_type*)pm_target_information_get_specific_info(PM_PROP_SBL_CHG_JETTA_CONFIG_DATA);
      CORE_VERIFY_PTR(jetta_threshold_ptr);
   }

   if (jetta_threshold_ptr->jetta_hard_cold_threshold.enable_config == PM_ENABLE_CONFIG)
   {
      err_flag |= pm_fg_batt_info_set_jeita_threshold_value(device_index, PM_FG_BATT_INFO_JEITA_THRESHOLD_TOO_COLD, jetta_threshold_ptr->jetta_hard_cold_threshold.value);
   }


   if (jetta_threshold_ptr->jetta_soft_cold_threshold.enable_config == PM_ENABLE_CONFIG)
   {
      err_flag |= pm_fg_batt_info_set_jeita_threshold_value(device_index, PM_FG_BATT_INFO_JEITA_THRESHOLD_COLD,     jetta_threshold_ptr->jetta_soft_cold_threshold.value);
   }

   if (jetta_threshold_ptr->jetta_soft_hot_threshold.enable_config == PM_ENABLE_CONFIG)
   {
      err_flag |= pm_fg_batt_info_set_jeita_threshold_value(device_index, PM_FG_BATT_INFO_JEITA_THRESHOLD_HOT,      jetta_threshold_ptr->jetta_soft_hot_threshold.value);
   }

   if (jetta_threshold_ptr->jetta_hard_hot_threshold.enable_config == PM_ENABLE_CONFIG)
   {
      err_flag |= pm_fg_batt_info_set_jeita_threshold_value(device_index, PM_FG_BATT_INFO_JEITA_THRESHOLD_TOO_HOT,  jetta_threshold_ptr->jetta_hard_hot_threshold.value);
   }

  return err_flag;
}



pm_err_flag_type pm_sbl_config_therm_ground_sel(uint32 device_index)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_fg_adc_rr_aux_therm_cfg_type auxThermConfig;
        
        static pm_sbl_schg_specific_data_type *therm_ground_sel_ptr = NULL;
        
    if (therm_ground_sel_ptr == NULL)
    {
      therm_ground_sel_ptr = (pm_sbl_schg_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_CHG_SPECIFIC_DATA);
      CORE_VERIFY_PTR(therm_ground_sel_ptr);
    }

    //config batt therm ground selection
    pm_fg_adc_rr_bias_wait_type batt_therm_cfg_bias_wait = therm_ground_sel_ptr->batt_therm_cfg.bias_wait;
    boolean gnd_sel = therm_ground_sel_ptr->batt_therm_cfg.ground_sel;      
    err_flag = pm_fg_set_adc_rr_batt_therm_cfg(device_index, batt_therm_cfg_bias_wait, gnd_sel);

        //config aux therm ground selection
    auxThermConfig.aux_therm_cfg_bias_wait = therm_ground_sel_ptr->aux_therm_cfg.bias_wait;
    auxThermConfig.tmp_hyst = PM_FG_ADC_RR_AUX_THERM_CFG_NO_HYST;
    auxThermConfig.batt_therm_sync = FALSE;
    auxThermConfig.aux_therm_mode = FALSE;
    auxThermConfig.gnd_sel = therm_ground_sel_ptr->aux_therm_cfg.ground_sel;
    err_flag |= pm_fg_adc_rr_aux_therm_cfg(device_index, &auxThermConfig);

    return err_flag;
}



pm_err_flag_type pm_sbl_config_thermister_coefficent(uint32 device_index)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   static pm_sbl_schg_batt_therm_type *batt_therm_ptr = NULL;
   boolean batt_therm_coeff_c1_cfg_done = FALSE;
   boolean batt_therm_coeff_c2_cfg_done = FALSE;
   boolean batt_therm_coeff_c3_cfg_done = FALSE;

   boolean batt_aux_therm_coeff_c1_cfg_done = FALSE;
   boolean batt_aux_therm_coeff_c2_cfg_done = FALSE;
   boolean batt_aux_therm_coeff_c3_cfg_done = FALSE;

   int32 batt_temp;
  
   
   if(batt_therm_ptr == NULL)
   {
      batt_therm_ptr = (pm_sbl_schg_batt_therm_type*)pm_target_information_get_specific_info(PM_PROP_SBL_CHG_BATT_THERM_CONFIG_DATA);
      CORE_VERIFY_PTR(batt_therm_ptr);
   }

   
   if (batt_therm_ptr->enable_batt_therm_cfg == PM_ENABLE_CONFIG) 
   {
   //Batt Therm
      err_flag |= pm_fg_batt_info_set_therm_coeff_value(device_index, PM_FG_BATT_INFO_THERM_COEFF_C1, batt_therm_ptr->batt_therm_cfg.coeff_c1);
      if(err_flag == PM_ERR_FLAG_SUCCESS)
      {
         batt_therm_coeff_c1_cfg_done = TRUE;
      }


      err_flag |= pm_fg_batt_info_set_therm_coeff_value(device_index, PM_FG_BATT_INFO_THERM_COEFF_C2, batt_therm_ptr->batt_therm_cfg.coeff_c2);
      if(err_flag == PM_ERR_FLAG_SUCCESS)
      {
         batt_therm_coeff_c2_cfg_done = TRUE;
      }

      err_flag |= pm_fg_batt_info_set_therm_coeff_value(device_index, PM_FG_BATT_INFO_THERM_COEFF_C3, batt_therm_ptr->batt_therm_cfg.coeff_c3);
      if(err_flag == PM_ERR_FLAG_SUCCESS)
      {
         batt_therm_coeff_c3_cfg_done = TRUE;
      }

      //Aux Batt Therm
      err_flag |= pm_fg_adc_rr_set_aux_therm_coefficient(device_index, PM_FG_ADC_RR_AUX_THERM_COEF_C1, batt_therm_ptr->batt_aux_therm_cfg.aux_coeff_c1);
      if(err_flag == PM_ERR_FLAG_SUCCESS)
      {
         batt_aux_therm_coeff_c1_cfg_done = TRUE;
      }

      err_flag |= pm_fg_adc_rr_set_aux_therm_coefficient(device_index, PM_FG_ADC_RR_AUX_THERM_COEF_C2, batt_therm_ptr->batt_aux_therm_cfg.aux_coeff_c2);
      if(err_flag == PM_ERR_FLAG_SUCCESS)
      {
         batt_aux_therm_coeff_c2_cfg_done = TRUE;
      }

      err_flag |= pm_fg_adc_rr_set_aux_therm_coefficient(device_index, PM_FG_ADC_RR_AUX_THERM_COEF_C3, batt_therm_ptr->batt_aux_therm_cfg.aux_coeff_c3);
      if(err_flag == PM_ERR_FLAG_SUCCESS)
      {
         batt_aux_therm_coeff_c3_cfg_done = TRUE;
      }
   }


   if( (batt_therm_coeff_c1_cfg_done == TRUE) &&
       (batt_therm_coeff_c2_cfg_done == TRUE) &&
       (batt_therm_coeff_c3_cfg_done == TRUE) &&
       (batt_aux_therm_coeff_c1_cfg_done == TRUE) &&
       (batt_aux_therm_coeff_c2_cfg_done == TRUE) &&
       (batt_aux_therm_coeff_c3_cfg_done == TRUE) ) 
   {
      batt_therm_coeff_cfg_flag = TRUE; 
      err_flag |= pm_fg_batt_info_get_batt_temp(device_index, &batt_temp); //dummy read
   }

   return err_flag;
}



pm_err_flag_type pm_sbl_config_wipower_chg_ilimit(uint32 device_index, uint32 wipower_chg_power_uw)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   uint32 ilmit_level_ma = 0;

   //Configure PM_SCHG_DC_ZIN_ICL_PT;  Use 8V :  W=VI
   ilmit_level_ma = wipower_chg_power_uw / PM_SBL_DC_ZIN_ICL_PT_VOLT_MV;
   err_flag |= pm_schg_dc_set_zin_icl(device_index, PM_SCHG_DC_ZIN_ICL_PT, ilmit_level_ma);  
   
   //Configure PM_SCHG_DC_ZIN_ICL_PT_HV;  9V 
   ilmit_level_ma = wipower_chg_power_uw / PM_SBL_DC_ZIN_ICL_PT_HV_VOLT_MV;
   err_flag |= pm_schg_dc_set_zin_icl(device_index, PM_SCHG_DC_ZIN_ICL_PT_HV, ilmit_level_ma);  
   
   //Configure PM_SCHG_DC_ZIN_ICL_LV; Use 5.5V 
   ilmit_level_ma = wipower_chg_power_uw / PM_SBL_DC_ZIN_ICL_LV_VOLT_MV;
   err_flag |= pm_schg_dc_set_zin_icl(device_index, PM_SCHG_DC_ZIN_ICL_LV, ilmit_level_ma); 
   
   //Configure PM_SCHG_DC_ZIN_ICL_MID_LV; Use 6.5V 
   ilmit_level_ma = wipower_chg_power_uw / PM_SBL_DC_ZIN_ICL_MID_LV_VOLT_MV;
   err_flag |= pm_schg_dc_set_zin_icl(device_index, PM_SCHG_DC_ZIN_ICL_MID_LV, ilmit_level_ma); 
   
   //Configure PM_SCHG_DC_ZIN_ICL_MID_HV; Use 8V 
   ilmit_level_ma = wipower_chg_power_uw / PM_SBL_DC_ZIN_ICL_MID_HV_VOLT_MV;
   err_flag |= pm_schg_dc_set_zin_icl(device_index, PM_SCHG_DC_ZIN_ICL_MID_HV, ilmit_level_ma);  
   
   //Configure PM_SCHG_DC_ZIN_ICL_HV; Use 11V 
   ilmit_level_ma = wipower_chg_power_uw / PM_SBL_DC_ZIN_ICL_HV_VOLT_MV;
   err_flag |= pm_schg_dc_set_zin_icl(device_index, PM_SCHG_DC_ZIN_ICL_HV, ilmit_level_ma);

   return err_flag;
}



pm_err_flag_type pm_sbl_config_chg_parameters(uint32 device_index)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

   static pm_sbl_schg_specific_data_type *chg_param_ptr;
   static pm_sbl_schg_wipower_specific_data_type *wipower_chg_param_ptr = NULL;
   
   if(chg_param_ptr == NULL)
   {
      chg_param_ptr = (pm_sbl_schg_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_CHG_SPECIFIC_DATA);
      CORE_VERIFY_PTR(chg_param_ptr);
   }
   
   if(wipower_chg_param_ptr == NULL)
   {
      wipower_chg_param_ptr = (pm_sbl_schg_wipower_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_CHG_WIPOWER_SPECIFIC_DATA);
      CORE_VERIFY_PTR(wipower_chg_param_ptr);
   }

   //Vlowbatt Threshold  
   //  - Done on:  pm_sbl_chg_config_vbat_low_threshold()
   
   //Battery Missing Detection Source 
   if (chg_param_ptr->bat_miss_detect_src.enable_config == PM_ENABLE_CONFIG)
   {
      boolean en_bat_missing_via_batid_therm_pin = chg_param_ptr->bat_miss_detect_src.en_bat_missing_via_batid_therm_pin;
      err_flag |= pm_schg_batif_cfg_batt_missing_src(device_index, PM_SCHG_BAT_MISS_SRC_CFG__PIN_SRC_EN, en_bat_missing_via_batid_therm_pin);
   }

   //WDOG Timeout      
   if (chg_param_ptr->wdog_timeout.enable_config == PM_ENABLE_CONFIG)
   {
      pm_schg_misc_sbb_wd_cfg wdog_timeout = chg_param_ptr->wdog_timeout.wdog_timeout;

      if( (wdog_timeout.snarl_wdog_timeout < PM_SCHG_MISC_SNARL_WDOG_TMOUT_INVALID) ||
          (wdog_timeout.bark_wdog_timeout  < PM_SCHG_MISC_BARK_WDOG_TMOUT_INVALID)  ||
          (wdog_timeout.bite_wdog_timeout  < PM_SCHG_MISC_BITE_WDOG_TMOUT_INVALID) )
      {
         err_flag |= pm_schg_misc_set_snarl_bark_bite_wd_cfg(device_index, wdog_timeout);
      }
      else
      {
         err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
      }
   }


   //FAST Charging Current            
   if (chg_param_ptr->fast_chg_i.enable_config == PM_ENABLE_CONFIG)
   {

      uint32 fast_chg_i_ma = chg_param_ptr->fast_chg_i.fast_chg_i_ma;
      if ((fast_chg_i_ma >= 300) && (fast_chg_i_ma <= 3000) )
      {
          err_flag |= pm_schg_chgr_set_charge_current(device_index, PM_SCHG_CHGR_FAST_CHARGE_CURRENT, fast_chg_i_ma);
      }
      else
      {
         err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
      }
   }

   //Pre Charge Current               
   if (chg_param_ptr->pre_chg_i.enable_config == PM_ENABLE_CONFIG)
   {
      uint32 pre_chg_i_ma = chg_param_ptr->pre_chg_i.pre_chg_i_ma;
      if ((pre_chg_i_ma >= 100) && (pre_chg_i_ma <= 550) )
      {
          err_flag |= pm_schg_chgr_set_charge_current(device_index, PM_SCHG_CHGR_PRE_CHARGE_CURRENT, pre_chg_i_ma);
      }
      else
      {
         err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
      }
   }


   //Float Voltage : 3600mV to 4500 mv                   
   if (chg_param_ptr->float_volt_theshold_mv.enable_config == PM_ENABLE_CONFIG)
   {
      uint32 float_volt_mv = chg_param_ptr->float_volt_theshold_mv.float_volt_theshold_mv;
      if ((float_volt_mv >= 3600) && (float_volt_mv <= 4500))
      {
          err_flag |= pm_schg_chgr_set_float_volt(device_index, float_volt_mv);
      }
      else
      {
         err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
      }
   }


   //USBIN Input Current Limit  :Valid value is 300 to 3000mAmp      
   if (chg_param_ptr->usbin_input_current_limit.enable_config == PM_ENABLE_CONFIG)
   {
      uint32 usbin_i_limit_ma = chg_param_ptr->usbin_input_current_limit.usbin_input_current_limit;
      if ((usbin_i_limit_ma >= 300) && (usbin_i_limit_ma <= 3000))
      {
          err_flag |= pm_schg_usb_set_usbin_current_limit_cfg(device_index, usbin_i_limit_ma);
      }
      else
      {
         err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
      }
   }


   //DCIN Input Current Limit : valid range is 300 to 2000 mAmp         
   if (chg_param_ptr->dcin_input_current_limit.enable_config == PM_ENABLE_CONFIG)
   {
      uint32 dcin_i_limit_ma = chg_param_ptr->dcin_input_current_limit.dcin_input_current_limit;
      if ((dcin_i_limit_ma >= 300) && (dcin_i_limit_ma <= 3200))
      {
          err_flag |= pm_schg_dc_set_usbin_current_limit_cfg(device_index, dcin_i_limit_ma);
      }
      else
      {
         err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
      }
   }
   
   //Configure Wipower Current Limit level to match power value
   if (wipower_chg_param_ptr->wipower_chg_power_level.enable_config == PM_ENABLE_CONFIG)
   {
      uint32 wipower_chg_power_uw = wipower_chg_param_ptr->wipower_chg_power_level.wipower_chg_power_level;
      if ((wipower_chg_power_uw >= PM_SBL_WP_CHG_POWER_MIN_UW) && (wipower_chg_power_uw <= PM_SBL_WP_CHG_POWER_MAX_UW))
      {
         err_flag |= pm_sbl_config_wipower_chg_ilimit(device_index, wipower_chg_power_uw);
      }
      else
      {
         err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
      }
   }

   return err_flag;
}


pm_err_flag_type 
pm_sbl_schg_info_init(uint32 device_index)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   boolean bIntergrityBit    = FALSE;

   //Read SOC Profile load state at the start of SBL Charging.  schg_info gets loaded to sram when pm_smem_init() called
   err_flag |= PmicFgSram_ReadIntegrityBit(device_index, &bIntergrityBit); //Check if SOC profile loaded: TRUE read SOC otherwise set to -1
   if (bIntergrityBit)
   {
      err_flag |= pm_fg_batt_soc_get_monotonic_soc(device_index, (uint32*)&schg_info.sbl_entry_soc);
   }
   else
   {
      schg_info.sbl_entry_soc = -1;
   }

   pm_log_message("EntryVbat: %d; EntrySOC: %d", schg_info.sbl_entry_mV, schg_info.sbl_entry_soc );  

   return err_flag;
}


pm_err_flag_type pm_sbl_schg_info_to_smem_update()
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint32 buf_size = sizeof(pm_chg_info_type);
  uint32 buf_size_ret = 0;
  pm_chg_info_type *pChargerInfo = NULL;

  pChargerInfo = (pm_chg_info_type *) smem_get_addr(SMEM_CHARGER_BATTERY_INFO, &buf_size_ret);
  if(!pChargerInfo)
  { //Assume Smem is already initilized in XBL Loader 
     pChargerInfo = (pm_chg_info_type *)smem_alloc(SMEM_CHARGER_BATTERY_INFO, buf_size);
  }


  if(!pChargerInfo)
  {
     pm_log_message("SMEM Chgr Info, ERROR Allocating");
     err_flag = PM_ERR_FLAG_INVALID;
  }
  else
  {
     pChargerInfo->sbl_entry_mV   = schg_info.sbl_entry_mV;
     pChargerInfo->sbl_entry_soc  = schg_info.sbl_entry_soc;
     pm_log_message("SMEM Chgr Info Write Success");
  }

  return err_flag; 
}



pm_err_flag_type 
pm_app_chgr_check_batt_temp_limits(uint32 device_index, pm_sbl_schg_batt_therm_type *batt_therm_ptr)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   int32 batt_temp;

   CORE_VERIFY_PTR(batt_therm_ptr);
   
   //Ensure Batt therm coefficents are confiugred for accurate temp reading
   if( batt_therm_coeff_cfg_flag == FALSE )
   {
      return  err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED; 
   }

   //Check if SW based Batt Therm monitoring is enabled
   if (batt_therm_ptr->enable_batt_therm_cfg == PM_ENABLE_CONFIG) 
   {
      err_flag |= pm_fg_batt_info_get_batt_temp(device_index, &batt_temp); //Read Batt Temp
      pm_log_message("BATT TEMP: %d DegC", batt_temp);
       
      //Check for outof bound limit
      if(( batt_temp < batt_therm_ptr->batt_temp_limit.critical_low_limit) ||
         ( batt_temp > batt_therm_ptr->batt_temp_limit.critical_high_limit))
      {
         //Double check that the outofbound temp condition
         err_flag |= pm_fg_batt_info_get_batt_temp(device_index, &batt_temp);
         pm_log_message("BATT TEMP Read2: %d DegC", batt_temp);

         if(( batt_temp < batt_therm_ptr->batt_temp_limit.critical_low_limit) ||
            ( batt_temp > batt_therm_ptr->batt_temp_limit.critical_high_limit))
         {
            pm_log_message("ERROR: Out of Bound Temp Conditon Occured, Performing AFP:  VBATT TEMP : %d DegC.; Min Limit: %d; Max Limit: %d; ",
                         batt_temp, batt_therm_ptr->batt_temp_limit.critical_low_limit, batt_therm_ptr->batt_temp_limit.critical_high_limit);
 
            err_flag |= pm_busy_wait(PM_WEAK_BATTERY_CHARGING_AFP_DELAY); //Needed to give just enough time for UART logging
            err_flag |= pm_schg_misc_en_afp(device_index); //perform AFP.  Control does not return
         }
      }
   }
 
   return err_flag; 
}


