/*! \file
 *  
 *  \brief  
 *   pm_config_target.c
 *  \details 
 *   This file contains customizable target specific 
 *   driver settings & PMIC registers. This file is generated from database functional
 *   configuration information that is maintained for each of the targets.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Resource Setting Information Version: VU.Please Provide Valid Label - Not Approved
 *    PMIC code generation Software Register Information Version: VU.Please Provide Valid Label - Not Approved
 *    PMIC code generation Processor Allocation Information Version: VU.Please Provide Valid Label - Not Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2010-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/17   ks      Added more config items on sbl_schg_pbs_temp_monitor_config[]
06/15/17   sv      Updated to support SDM670 taregt.
05/01/17   aab     Added support for pm_rgb_specific_data_type config data
04/28/17   aab     Added pm_device_index_type config data
03/14/17   pxm     Enable ICL and set to 1500 mA.
02/13/17    al     Add support to read PBS from REV_ID
01/11/17   pxm     add support for PBS triggered AFP
12/16/16   pxm     Added support for fake battery id range configuration
10/20/16   aab     Added support for Batt_therm Out of bound temperature check
10/13/16   pxm     Updated charger Wdog default configuration
09/17/16   aab     Updated Wipower charger default power level. 
09/17/16   aab     Added WiPower charging wait time as configurable 
08/25/16   pxm     Add skin temperature threshold configuration parameter  
08/12/16   aab     Added debugboard detection support
07/05/16   aab     Cleanup unused data
05/27/16   pxm     Add sbl_schg_wipower_specific_data[1]
04/29/16   aab     Added sbl_pon_specific_data[]
03/24/16   al      Add keypress support
03/29/16   al      Updating for newer target
03/15/16   aab     Driver update (Support for PMI8998)
02/16/16   aab     Creation
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_lpg_driver.h"
#include "pm_vib_driver.h"
#include "pm_schg_driver.h"

#include "pm_fg_sram.h"
#include "pm_fg_adc_rr.h"
#include "pm_resource_manager.h"
#include "pm_pbs_info.h"
#include "pm_pbs_driver.h"
#include "pm_config_target.h"
#include "pm_sbl_boot.h"

#include "pm_app_chgr.h"
#include "pm_app_chgr_common.h"
#include "pm_schg_batif.h"
#include "pm_schg_misc.h"
#include "pm_app_pon.h"

#include "rpmh_common.h"
#include "device_info.h"


 /************************************************ DRIVER DATA *********************************/
//PM660,NONE,PM8005
uint32 num_of_ldo[]        = {19,0,0}; /* LDO4 absent */
uint32 num_of_smps[]       = {6, 0,4};
uint32 num_of_gpio[]       = {13,0,4};
uint32 num_of_mpp[]        = {0, 0,0};
uint32 num_of_rtc[]        = {1, 0,0};
uint32 num_of_megaxo[]     = {1, 0,0}; /* XO or CLK driver */
uint32 num_of_boost[]      = {0, 0,0};
uint32 num_of_bob[]        = {0, 0,0};      //todo
uint32 num_of_vs[]         = {0, 0,0};
uint32 num_of_lpg_chan[]   = {0, 0,0};
uint32 num_of_pwron[]      = {1, 0,1};
uint32 num_of_misc[]       = {1, 0,1};
uint32 num_of_rgb[]        = {0, 0,0}; /* TRI_LED */
uint32 num_of_wled[]       = {0, 0,0};
uint32 num_of_schg[]       = {1, 0,0};
uint32 num_of_coincell[]   = {1, 0,0};
uint32 num_of_fg[]         = {1, 0,0};
uint32 num_of_ibb[]        = {0, 0,0};
uint32 num_of_lab[]        = {0, 0,0};
uint32 num_of_vib[]        = {0, 0,0};
uint32 num_of_pbs_client[] = {4, 0,2};
uint32 num_of_haptics[]    = {1, 0,0};

pm_lpg_specific_info_type lpg_specific[1] =  
{
    {49}
};

pm_vib_specific_data_type
vib_specific[1] = 
{
  {1200, 2000}
};


pm_sbl_pon_specific_data_type
sbl_pon_specific_data[1] = 
{
   {
      {
         16,                              // s3 timer value  (max value: 128 sec)
         PM_APP_PON_RESET_SOURCE_KPDPWR,  // pm_app_pon_reset_source_type
         PM_DISABLE_CONFIG                // PM_DISABLE_CONFIG / PM_ENABLE_CONFIG
      }, 
   }
};



pm_sbl_schg_specific_data_type
sbl_schg_specific_data[1] = 
{
   {
      //Configuration Value,                             Enable config
      {PM_SCHG_BATIF_LOW_BATTERY_THRESH_3P2,             PM_DISABLE_CONFIG },  //Vlowbatt Threshold
      {PM_SCHG_BATIF_LOW_BATTERY_THRESH_2P6,             PM_ENABLE_CONFIG  },  //APSD rerun Vlowbatt Threshold
      {70,       									     PM_ENABLE_CONFIG  },  //Fg skin hot threshold: valid range is -30 to 97 degree C
      {80,       									     PM_ENABLE_CONFIG  },  //Fg skin too hot threshold: valid range is -30 to 97 degree C
      {80,                                               PM_ENABLE_CONFIG  },  //Fg charge hot threshold: valid range is -30 to 97 degree C 
      {90,                                               PM_ENABLE_CONFIG  },  //Fg charge too hot threshold: valid range is -30 to 97 degree C 
      {TRUE,                                             PM_DISABLE_CONFIG },  //Use BATID and/or THERM pin for battery missing detection
      {{TRUE, PM_SCHG_MISC_SNARL_WDOG_TMOUT_62P5MS, PM_SCHG_MISC_BARK_WDOG_TMOUT_128S, PM_SCHG_MISC_BITE_WDOG_TMOUT_8S}, PM_ENABLE_CONFIG },  //Enable/Disable and Timeout WDog Config
      {1000,                                             PM_DISABLE_CONFIG },  //FAST Charging Current
      {250,                                              PM_DISABLE_CONFIG },  //PRE Charge Current
      {4200,                                             PM_DISABLE_CONFIG },  //Float Voltage
      {500,                                              PM_ENABLE_CONFIG  },  //USBIN Input Current Limit
      {1000,                                             PM_DISABLE_CONFIG },  //DCIN Input Current Limit
      {6750,8250,                                        PM_DISABLE_CONFIG  },  //Fake battery detection range: Battery will be detected as fake battery if BATT ID in this range. unit: Ohms
      {PM_FG_ADC_RR_BIAS_WAIT_4_MS,                      TRUE              },  //Batt therm ground selection
      {PM_FG_ADC_RR_BIAS_WAIT_4_MS,                      FALSE             },  //Aux therm ground selection       
      {2000,14000},                                                            //Debug Board detect: BATT_ID PD Resistor ADC Min/Max Read Value range; unit: Ohms
      3300,                                                                    //bootup_battery_theshold_mv
      3300,                                                                    //wipowr bootup battery thesholdmv
      2800,                                                                    //apsd_reset_threshold_mv:  APSD reset only applicable if initial Vbatt level is less than this threshold
      2800,                                                                    //apsd_reset_theshold_no_uvlo_mv: Used if last reset reason is NOT UVLO
      3200,                                                                    //apsd_reset_theshold_uvlo_mv: Used if last reset reason is UVLO
      TRUE,                                                                    //Enable/Disable JEITA Hard Temp Limit Check in SBL                                                                                                                                                       
      TRUE,                                                                    //dbc_usb_500_mode
      FALSE,                                                                   //Verbose SBL CHG UART logging
   }
};



pm_sbl_specific_data_type
sbl_specific_data[1] = {
   {
     FALSE,                     // Used to enable/disable verbose UART logging
     FALSE,                     // FALSE: Default for all customer; TRUE: Disable RFCLK1
   }
};

pm_sbl_schg_wipower_specific_data_type
sbl_schg_wipower_specific_data[1] = 
{
   {
      {
         5000000,           // Wipower charger power valid range in uWatt: 5000000 to 20000000
         PM_ENABLE_CONFIG   // PM_DISABLE_CONFIG / PM_ENABLE_CONFIG
      }, 
      {
         3500,              //unit: ms;  default min Val: 3500ms;  NO Max Value.
         PM_ENABLE_CONFIG   //PM_DISABLE_CONFIG: use the wait time;  PM_ENABLE_CONFIG: Wait forever
      }, 
   }
};


//To enable configuration, set EnableConfig = PM_ENABLE_CONFIG
pm_sbl_schg_jetta_threshold_config_type
sbl_schg_jetta_threshold[1] = 
{
   {
      {0x3C,     PM_ENABLE_CONFIG },   //JEITA Hard Cold Threshold:  default = 0x3C
      {0x50,     PM_ENABLE_CONFIG },   //JEITA Soft Cold Threshold:  default = 0x50
      {0x96,     PM_ENABLE_CONFIG },   //JEITA Soft Hot  Threshold:  default = 0x96
      {0xAA,     PM_ENABLE_CONFIG }    //JEITA hard Hot  Threshold:  default = 0xAA
   }
};


pm_sbl_schg_batt_therm_type
sbl_schg_batt_therm[1] = 
{
   {
      PM_ENABLE_CONFIG,   //Enable/Disable Batt Therm config
      {
        -20,              //Battery Critical low temp limit -20 Deg C.
         80               //Battery Critical high temp limit  80 Deg C. 
      },                       
      {  
        0xA1,             //Thremistor_c1_coeff:  default = 0xA1
        0x50,             //Thremistor_c2_coeff:  default = 0x50
        0xFF              //Thremistor_c3_coeff:  default = 0xFF
      },                                                                              
      {                                                                               
        0xBF,             //Thremistor_c1_coeff:  default = 0xBF
        0x36,             //Thremistor_c2_coeff:  default = 0x36
        0xFF              //Thremistor_c3_coeff:  default = 0xFF
      }
   }
};

pm_app_chg_temp_limit_config_type
pm_app_chg_temp_limit_config[1] = 
{
   {
      0xAC,     //Hot threshold:  71 Deg. c. =  (71 + 273)/2 
      0x00,     //Cold threshold: -273 Deg.C =  (-273 + 273)/2 
   }
};

uint8 pm_pbs_afp_en_afp_val  = 0x11; //PBS SW Trigger argument value to enable AFP
uint8 pm_pbs_afp_dis_afp_val = 0x01; //PBS SW Trigger argument value to disable AFP

pm_app_chg_temp_monitor_config_type
pm_app_chg_temp_monitor_config[1] = 
{
   {
      0,        //PMIC index
      0,        //Hot/Cold Limit PMIC Reg Address SID Index
      0x0953,   //Hot Limit Reg Address  (misc_spare_4)
      0x0952,   //Cold Limit reg Address (misc_spare_3)
      PM_PBS_CLIENT_3,                    //PBS Client
      PM_PBS_CLIENT_SW_TRIGGER,           //Client Trigger Num
      PM_PBS_CLIENT_TRIGGER_EDGE_INVALID, //Client HW Trigger Edge
      &pm_pbs_afp_en_afp_val,             //PBS SW Trigger argument value to enable AFP
      &pm_pbs_afp_dis_afp_val,            //PBS SW Trigger argument value to disable AFP  
      TRUE                                //PBS AFP Enable/Disable 
   }
};


pm_device_index_type pm_device_index =
{
   0,  //Primary PMIC Device Index   (PM660)
   1,  //Interface PMIC Device Index (PM660L)
   2,  //Secondary PMIC Device Index (PM8005)
};

pm_rgb_pmic_index_type pm_rgb_pmic_index = //todo
{
   PMIC_IS_INVALID,   // Charger RGB LED PMIC index
   //0   // Notification RGB LED PMIC index - future purpose when multiple RGB exists on different PMIC's
};

pm_rgb_specific_data_type
rgb_specific_data =  //todo
{
   0x00,           //PM_RGB_DIM_LEVEL_MIN 
   0x1FF,          //PM_RGB_DIM_LEVEL_MAX
   PM_LPG_CHAN_3,  //LPG Channel for Red LED
   PM_LPG_CHAN_2,  //LPG Channel for Green LED
   PM_LPG_CHAN_1,  //LPG Channel for Blue LED
};


pm_pbs_info_data_type pm_pbs_info_data_a =
{
    0x0,                /* PBS OTP MEMORY Start Address */
    512,                /* PBS OTP MEMORY Size */
    0x800,              /* PBS RAM MEMORY Start Address */
    512,                /* PBS RAM MEMORY Size */
    PM_PBS_INFO_IN_OTP, /* Place where PBS Info stored */
    0x0100,                /* REV_ID Periph BASE_ADDRESS */
    0xF0,                  /* trim_num  */
    0xF1,                  /* tp_rev    */
    0xF2,                  /* fab_id    */
    0xF3,                  /* wafer_id  */
    0xF4,                  /* x_co_ordi */
    0xF5,                  /* y_co_ordi */
    0xF6,                  /* lot_id_11_10 */
    0xFF,                  /* mfg_id    */
};

pm_pbs_info_data_type pm_pbs_info_data_b =   //todo
{
    0,                   /* PBS OTP MEMORY Start Address */
    0,                   /* PBS OTP MEMORY Size */
    0,                 /* PBS RAM MEMORY Start Address */
    0,                   /* PBS RAM MEMORY Size */
    PM_PBS_INFO_INVALID, /* Place where PBS Info stored */
    0,                /* REV_ID Periph BASE_ADDRESS */
    0,                  /* trim_num  */
    0,                  /* tp_rev    */
    0,                  /* fab_id    */
    0,                  /* wafer_id  */
    0,                  /* x_co_ordi */
    0,                  /* y_co_ordi */
    0,                  /* lot_id_11_10 */
    0,                  /* mfg_id    */
};

pm_pbs_info_data_type pm_pbs_info_data_c =
{
    0x0,                /* PBS OTP MEMORY Start Address */
    128,                /* PBS OTP MEMORY Size */
    0x200,              /* PBS RAM MEMORY Start Address */
    128,                /* PBS RAM MEMORY Size */
    PM_PBS_INFO_IN_OTP, /* Place where PBS Info stored */
    0x00,               /* Periph BASE_ADDRESS */
    0x00,               /* trim_num  */
    0x00,               /* tp_rev    */
    0x00,               /* fab_id    */
    0x00,               /* wafer_id  */
    0x00,               /* x_co_ordi */
    0x00,               /* y_co_ordi */
    0x00,               /* lot_id_01_00 */
    0x00,               /* mfg_id    */
};

pm_pbs_info_data_type *pm_pbs_info_data[] = {&pm_pbs_info_data_a, &pm_pbs_info_data_b, &pm_pbs_info_data_c};

/* max_num_seqs, ram_start_addr, poff_trig_id, poff_otp_addr, warmreset_trig_id, warmreset_otp_addr */
pm_pbs_custom_seq_data_type pbs_custom_seq_data_a = {2, 0x07DC, 3, 0x000C, 2, 0x0008};

/* max_num_seqs, ram_start_addr, poff_trig_id, poff_otp_addr, warmreset_trig_id, warmreset_otp_addr */
pm_pbs_custom_seq_data_type pbs_custom_seq_data_b = {33, 0x0554, 2, 0x0008, 7, 0x001C};
 
pm_pbs_custom_seq_data_type* pbs_custom_seq_data[] = 
{
   NULL,
   NULL,
   NULL,
   NULL,
};


/* PPID, channel mapping and ownership.  */
const uint8 pm_periph_bitmap[][PM_MAX_NUM_SLAVE_IDS_PER_PMIC][PM_MAX_BITMAP_ENTRIES] =
{
  /* pm660 */
  {
    {
      0x72, 0x0b, 0xff, 0x1b,
      0x10, 0x11, 0xb6, 0x13,
      0x7f, 0x00, 0x3f, 0x0e,
      0x07, 0x00, 0x1f, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0xff, 0x1f, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,

    },
    {
      0x02, 0x00, 0xf1, 0xff,
      0x3f, 0x00, 0x00, 0x00,
      0xf7, 0xff, 0x07, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x01, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,

    },
  },
  {
    {    },
	{    },
  },
/* pm8005 */
  {
    {
      0x72, 0x03, 0x00, 0x00,
      0x10, 0x10, 0x00, 0x10,
      0x00, 0x00, 0x00, 0x02,
      0x00, 0x00, 0x07, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x0f, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
 
    },
    {
      0x02, 0x00, 0xf1, 0xff,
      0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
 
    },
  },
};

/*key press in milli seconds for valid boot*/
pm_pwrkey_dbnc_chk_type pm_pon_pwrkey_dbnc_chk[] = {{PM_PON_PWRKEY_DBNC_CHK_AT_CORE, 500}} ; //check power key press at, time in milli sec


uint32 rpmh_drv_id [] = {(uint32)RSC_DRV_HLOS};
