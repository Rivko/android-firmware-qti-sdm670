#ifndef __PM_SCHG_MISC_H__
#define __PM_SCHG_MISC_H__

/*! \file
*  \n
*  \brief  pm_schg_misc.h PMIC-SMBC MODULE RELATED DECLARATION 
*  \details  This header file contains functions and variable declarations 
*  to support Qualcomm PMIC SCHG OTG (Switch-Mode Battery Charger) module. The 
*  Switched-Mode Battery Charger (SCHG OTG) module includes a buck regulated 
*  battery charger with integrated switches. The SCHG OTG module, along with the 
*  Over Voltage Proection (OVP) module will majorly be used by charger 
*  appliation for charging Li-Ion batteries with high current (up to 2A).
*  \n &copy;
*  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/17/17   pxm     Added API to get sysok reason 
01/10/17   cs      add API to set thermistor source configuration
11/14/16   sm      Added pm_schg_misc_set_buckboost_refresh_pulse()
03/09/16   sm      Added pm_schg_misc_get_shutdown_status()
02/16/16   va      Adding required API for enabling charger
01/14/15   al      Initial version. 
========================================================================== */
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

typedef enum {
  PM_SCHG_MISC_IRQ_WDOG_SNARL,             /* Generated when SNARL watchdog timer expires*/
  PM_SCHG_MISC_IRQ_WDOG_BARK,              /*Generated when BARK watchdog timer expires*/ 
  PM_SCHG_MISC_IRQ_AICL_FAIL,              /*AICL Fail*/
  PM_SCHG_MISC_IRQ_AICL_DONE,              /*AICL Done*/
  PM_SCHG_MISC_IRQ_HIGH_DUTY_CYCLE,        /*High duty cycle*/
  PM_SCHG_MISC_IRQ_INPUT_CURRENT_LIMITING, /*Input current limiting*/
  PM_SCHG_MISC_IRQ_TEMPERATURE_CHANGE,     /*Change in temperature die or skin*/
  PM_SCHG_MISC_IRQ_SWITCHER_POWER_OK,      /*Indicates that the switcher is running and able to deliver power to the load*/
  PM_SCHG_MISC_IRQ_INVALID,                /*Invalid*/
}pm_schg_misc_irq_type; 


typedef enum 
{
   PM_SCHG_MISC_POWER_PATH__NONE,
   PM_SCHG_MISC_POWER_PATH__BATTERY,
   PM_SCHG_MISC_POWER_PATH__USBIN,
   PM_SCHG_MISC_POWER_PATH__DCIN,
   PM_SCHG_MISC_POWER_PATH__INVALID
}pm_schg_misc_pwr_pth_type;

typedef enum 
{
   PM_SCHG_MISC_SYSOK_REASON__NONE,
   PM_SCHG_MISC_SYSOK_REASON__USBIN,
   PM_SCHG_MISC_SYSOK_REASON__DCIN,
   PM_SCHG_MISC_SYSOK_REASON__INVALID
}pm_schg_misc_sysok_reason_type;

typedef struct {
  boolean thermreg_disabled;         /* all 3 temperature monitoring sources (skin, die, junction) are disabled */
  boolean temp_below_range;          /* when high, indicates skin, die or junction temperature below temp_1 level*/
  boolean temp_within_range;         /* when high, indicates skin, die or junction temperature between temp_1 level and temp_2 level*/
  boolean temp_above_range;          /* when high, indicates skin, die or junction temperature between temp_2 level and temp_3 level*/
  boolean alert_level;               /* when high, indicates skin, die or junction temperature above temp_3 level*/
  boolean tlim;                      /* when high, indicates junction temp above 140c*/
  boolean therm_range_active;        /* 0 = no thermal regulation, 1 = thermal regulation of icl and/or input voltage (qc)*/
  boolean therm_range_status_7;      /* reserved */
}pm_schg_misc_temp_range_status; 


typedef struct {
  boolean aicl_done;                 /* Aicl Done*/
  boolean aicl_fail;                 /* Aicl Fail*/
  boolean icl_imin;                  /* ICL has collapsed down to ICLmin*/
  boolean dcin_ch_collapse;          /* DC Input Collapse (dg)*/
  boolean usbin_ch_collapse;         /* USB Input Collapse (dg)*/
  boolean highest_dc;                /* Switcher in High Duty Cycle*/
  boolean soft_limit;                /* Switcher Hard/Soft Limiting*/
  boolean aicl_status_7;             /* tbd-reserved */
}pm_schg_misc_aicl_status; 

typedef enum {
  PM_SCHG_MISC_BARK,
  PM_SCHG_MISC_BITE,
  PM_SCHG_MISC_BARK_BITE_INVALID, /*Invalid*/
}pm_schg_misc_wdog_status;

typedef struct {
  boolean wdog_timer_en;              /* Reserved (Note in PMI8998 2.0, this bit will be used as global watchdog timer enable/disable) */
  boolean wdog_option;                /* Reserved*/
  boolean wdog_irq_sft;               /* Reserved*/
  uint8   sft_after_wdog_irq;         /* reserved */
  boolean bite_wdog_int_en;           /* 0 = Disable BITE watchdog timer expire interrupt, 1 = Enable BITE watchdog timer expire interrupt*/
  boolean bark_wdog_int_en;           /* 0 = Disable BARK watchdog timer expire interrupt, 1 = Enable BARK watchdog timer expire interrupt
                                      0x0 : BARK_WDOG_INTR_DIS
                                      0x1 : BARK_WDOG_INTR_EN*/
  boolean watchdog_trigger_afp_en;    /* Watchdog Starts TFT (trigger AFP mode on bite watchdog timer expire)
                                      0 = Do not enable AFP mode on bite watchdog timer expire
                                      1 = Enable AFP mode on bite watchdog timer expire */
}pm_schg_misc_wdog_cfg_type;

typedef enum {
  PM_SCHG_MISC_SNARL_WDOG_TMOUT_62P5MS,
  PM_SCHG_MISC_SNARL_WDOG_TMOUT_125MS,
  PM_SCHG_MISC_SNARL_WDOG_TMOUT_250MS,
  PM_SCHG_MISC_SNARL_WDOG_TMOUT_500MS,
  PM_SCHG_MISC_SNARL_WDOG_TMOUT_1S,
  PM_SCHG_MISC_SNARL_WDOG_TMOUT_2S,
  PM_SCHG_MISC_SNARL_WDOG_TMOUT_4S,
  PM_SCHG_MISC_SNARL_WDOG_TMOUT_8S,
  PM_SCHG_MISC_SNARL_WDOG_TMOUT_INVALID, /*Invalid*/
}pm_schg_misc_snarl_wdog_timeout; 

typedef enum {
  PM_SCHG_MISC_BARK_WDOG_TMOUT_16S,
  PM_SCHG_MISC_BARK_WDOG_TMOUT_32S,
  PM_SCHG_MISC_BARK_WDOG_TMOUT_64S,
  PM_SCHG_MISC_BARK_WDOG_TMOUT_128S,
  PM_SCHG_MISC_BARK_WDOG_TMOUT_INVALID, /*Invalid*/
}pm_schg_misc_bark_wdog_timeout;

typedef enum {
  PM_SCHG_MISC_BITE_WDOG_TMOUT_1S,
  PM_SCHG_MISC_BITE_WDOG_TMOUT_2S,
  PM_SCHG_MISC_BITE_WDOG_TMOUT_4S,
  PM_SCHG_MISC_BITE_WDOG_TMOUT_8S,
  PM_SCHG_MISC_BITE_WDOG_TMOUT_INVALID, /*Invalid*/
}pm_schg_misc_bite_wdog_timeout;

typedef struct {
  boolean bite_wdog_disable_charging_cfg;  /* 0 =Allow charging when bite watchdog timer expires, 1 = Stop charging when bite watchdog timer expires*/
  pm_schg_misc_snarl_wdog_timeout snarl_wdog_timeout;              /* Snarl watchdog Timeout*/
  pm_schg_misc_bark_wdog_timeout  bark_wdog_timeout;               /* Bark watchdog Timeout*/
  pm_schg_misc_bite_wdog_timeout  bite_wdog_timeout;               /* Bite watchdog Timeout */
}pm_schg_misc_sbb_wd_cfg;

typedef enum {
  PM_SCHG_MISC_AICL_CFG_TREG_ALLOW_DECREASE,  /* allow decrease during thermal regulation even if increase was due to high duty cycle*/
  PM_SCHG_MISC_AICL_CFG_AICL_HIGH_DC_INC,     /*allow aicl to increment input current limit even during high duty cycle*/
  PM_SCHG_MISC_AICL_CFG_INVALID
}pm_schg_misc_aicl_cfg;

typedef enum
{
  PM_SCHG_MISC_AICL_RERUN_DIS,
  PM_SCHG_MISC_AICL_RERUN_EN,
  PM_SCHG_MISC_AICL_RERUN_INVALID,
}pm_schg_misc_aicl_rerun_type;

typedef enum {
  PM_SCHG_MISC_AICL_RERUN_TIME_0P2S,
  PM_SCHG_MISC_AICL_RERUN_TIME_0P4S,
  PM_SCHG_MISC_AICL_RERUN_TIME_0P8S,
  PM_SCHG_MISC_AICL_RERUN_TIME_1P6S,
  PM_SCHG_MISC_AICL_RERUN_TIME_CFG_INVALID, /*Invalid*/
}pm_schg_misc_aicl_rerun_time_cfg;

typedef enum
{
  PM_SCHG_MISC_BUCKBOOST_REFRESH_PULSE_1_IN_8,
  PM_SCHG_MISC_BUCKBOOST_REFRESH_PULSE_1_IN_4,
  PM_SCHG_MISC_BUCKBOOST_REFRESH_PULSE_INVALID,
}pm_schg_misc_buckboost_refresh_pulse_type;


typedef enum
{
  PM_SCHG_MISC_DIE_CMP_SRC_EN,
  PM_SCHG_MISC_DIE_ADC_SRC_EN,
  PM_SCHG_MISC_SKIN_ADC_SRC_EN,
  PM_SCHG_MISC_SKIN_ADC_CFG,  //1 TO USE AUX THERM PIN FOR BATTERY TEMPERATURE DETECTION (jeita), 0 FOR SKIN TEMP DETECTION
  PM_SCHG_MISC_ALLOW_ALL_DIE_CMP_SRC, //0: reset threshold only, 1: Lower Bound, upper bound and reset thresholds.
  PM_SCHG_MISC_THERM_SRC_CFG_INVALID,
}pm_schg_misc_therm_src_cfg_type;

/*===========================================================================

                 SCHG OTG DRIVER FUNCTION PROTOTYPES

===========================================================================*/

 /**
 * @brief This function enables the SCHG irq 
 * 
 * @details
 *  This function enables the SCHG irq 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] irq                 refer pm_schg_misc_irq_type
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_misc_irq_enable(uint32 pmic_device, pm_schg_misc_irq_type irq, boolean enable);

 /**
 * @brief This function clears the SCHG irq 
 * 
 * @details
 *  This function clears the SCHG irq 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] irq                 refer pm_schg_misc_irq_type
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_misc_irq_clear(uint32  pmic_device, pm_schg_misc_irq_type irq);

 /**
 * @brief This function configures the SCHG irq trigger 
 * 
 * @details
 *  This function configures the SCHG irq trigger 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] irq                 refer pm_schg_misc_irq_type
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_misc_irq_set_trigger(uint32 pmic_device, pm_schg_misc_irq_type irq, pm_irq_trigger_type trigger);

 /**
 * @brief This function configures the SCHG for irq 
 * 
 * @details
 *  This function configures the SCHG for irq 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] irq                 refer pm_schg_misc_irq_type
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_misc_irq_status(uint32 pmic_device, pm_schg_misc_irq_type irq, pm_irq_status_type type, boolean *status);


/**
* @brief This function returns sysok reason for device
* 
* @details
*  This function returns sysok reason for device
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] sysok_reason           Refer enum sysok_reasonh_type 
*       for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_get_sysok_reason(uint32 device_index, pm_schg_misc_sysok_reason_type *sysok_reason);


/**
* @brief This function returns power path for device
* 
* @details
*  This function returns power path for device
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] power_path           Refer enum pm_pwr_pth_type 
*       for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_get_power_path(uint32 device_index, pm_schg_misc_pwr_pth_type *power_path);


/**
* @brief This function reads the misc temp range status 
* 
* @details
*  This function reads the misc temp range status
* 
* @param[in] pmic_device_index.   Primary: 0 Secondary: 1
* @param[out] misc_temp_range_sts refer pm_schg_misc_temp_range_status
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_get_temp_range_sts(uint32 device_index, pm_schg_misc_temp_range_status *misc_temp_range_sts);


/**
* @brief This function returns Input Current Limit (post aicl and therm adjustment in milliamp )
* 
* @details
*  This function returns Input Current Limit (post aicl and therm adjustment in milliamp
* 
* @param[in] pmic_device_index.   Primary: 0 Secondary: 1
* @param[out] misc_icl_current_ma in mA
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_get_icl_sts(uint32 device_index, uint8 *misc_icl_current_ma);


/**
* @brief This function returns Input Current Limit when adapter increments from 5V to 9V in QC2.0 in milliamp
* 
* @details
*  This function returns Input Current Limit when adapter increments from 5V to 9V in QC2.0 in milliamp
* 
* @param[in] pmic_device_index.   Primary: 0 Secondary: 1
* @param[out] misc_5v_icl_current_ma in mA
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_get_adapter_5v_icl_status_sts(uint32 device_index, uint8 *misc_5v_icl_current_ma);


/**
* @brief This function returns Input Current Limit when adapter increments from 9V to 12V in QC2.0 in milliamp
* 
* @details
*  This function returns Input Current Limit when adapter increments from 9V to 12V in QC2.0 in milliamp
* 
* @param[in] pmic_device_index.   Primary: 0 Secondary: 1
* @param[out] misc_9v_icl_current_ma in mA
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_get_adapter_9v_icl_status_sts(uint32 device_index, uint8 *misc_9v_icl_current_ma);


/**
* @brief This function reads the misc aicl status
* 
* @details
*  This function returns reads the misc aicl status
* 
* @param[in] pmic_device_index.   Primary: 0 Secondary: 1
* @param[out] misc_aicl_sts refer pm_schg_misc_aicl_status
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_get_aicl_sts(uint32 device_index, pm_schg_misc_aicl_status *misc_aicl_sts);


/**
* @brief This function returns When bark/bite watchdog interrupt trips, this status bit can be used to determine which of the two wdog timer expired
* 
* @details
*  This function returns returns When bark/bite watchdog interrupt trips, this status bit can be used to determine which of the two wdog timer expired
* 
* @param[in] pmic_device_index.   Primary: 0 Secondary: 1
* @param[out] misc_wdog_sts       refer pm_schg_misc_wdog_status
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_get_misc_wdog_status(uint32 device_index, pm_schg_misc_wdog_status *misc_wdog_sts);


/**
* @brief This function enables automatic fault protection 
* 
* @details
*  This function enables automatic fault protection 
* 
* @param[in] pmic_device_index.   Primary: 0 Secondary: 1
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_en_afp(uint32 pmic_device);


/**
* @brief Writing to this register will pet (reset) BARK and BITE watchdog timers 
* 
* @details
*  Writing to this register will pet (reset) BARK and BITE watchdog timers 
* 
* @param[in] pmic_device_index.   Primary: 0 Secondary: 1
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_bark_bite_wdog_pet(uint32 pmic_device);


/**
* @brief This function configures the charger watchdog
* 
* @details
*  This function configures the charger watchdog
* 
* @param[in] pmic_device_index.   Primary: 0 Secondary: 1
* @param[in] misc_wd_cfg       refer pm_schg_misc_wdog_cfg_type
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_config_wdog(uint32 pmic_device, pm_schg_misc_wdog_cfg_type misc_wd_cfg);

/**
* @brief This function gets configures the charger watchdog 
* 
* @details
*  This function gets configures the charger watchdog 
* 
* @param[in] pmic_device_index.   Primary: 0 Secondary: 1
* @param[out] misc_wd_cfg       refer pm_schg_misc_wdog_cfg_type
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_get_config_wdog(uint32 pmic_device, pm_schg_misc_wdog_cfg_type *misc_wd_cfg);


/**
* @brief This function configures the charger watchdog
* 
* @details
*  This function configures the charger watchdog
* 
* @param[in] pmic_device_index.   Primary: 0 Secondary: 1
* @param[in] misc_wd_sbb_cfg      refer pm_schg_misc_sbb_wd_cfg
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_set_snarl_bark_bite_wd_cfg(uint32 pmic_device, pm_schg_misc_sbb_wd_cfg misc_wd_sbb_cfg);


/**
* @brief This function gets the charger snarl bark bite watchdog cfg 
* 
* @details
*  This function gets the charger snarl bark bite watchdog cfg
* 
* @param[in] pmic_device_index.     Primary: 0 Secondary: 1
* @param[out] misc_wd_sbb_cfg       refer pm_schg_misc_sbb_wd_cfg
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_get_snarl_bark_bite_wd_cfg(uint32 pmic_device, pm_schg_misc_sbb_wd_cfg *misc_wd_sbb_cfg);


/**
* @brief This function configures misc aicl cfg 
* 
* @details
*  This function configures misc aicl cfg 
* 
* @param[in] pmic_device_index.   Primary: 0 Secondary: 1
* @param[in] misc_aicl_cfg        fer pm_schg_misc_aicl_cfg
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_set_aicl_cfg(uint32 pmic_device, pm_schg_misc_aicl_cfg misc_aicl_cfg, pm_schg_misc_aicl_rerun_type aicl_rerun_type);


/**
* @brief This functiongets misc aicl cfg 
* 
* @details
*  This function gets gets misc aicl cfg
* 
* @param[in] pmic_device_index.   Primary: 0 Secondary: 1
* @param[out] misc_aicl_cfg       refer pm_schg_misc_aicl_cfg
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_get_aicl_cfg(uint32 pmic_device, pm_schg_misc_aicl_cfg misc_aicl_cfg, pm_schg_misc_aicl_rerun_type *aicl_rerun_type);


/**
* @brief This function configures misc aicl rerun time cfg 
* 
* @details
*  This function configures misc aicl re run time cfg 
* 
* @param[in] pmic_device_index.   Primary: 0 Secondary: 1
* @param[in] misc_aicl_cfg        refer pm_schg_misc_aicl_rerun_time_cfg
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type  pm_schg_misc_set_aicl_rerun_time_cfg(uint32 pmic_device, pm_schg_misc_aicl_rerun_time_cfg aicl_rerun_time);


/**
* @brief This function gets misc aicl cfg 
* 
* @details
*  This function gets gets misc aicl cfg
* 
* @param[in] pmic_device_index.   Primary: 0 Secondary: 1
* @param[out] misc_aicl_cfg       refer pm_schg_misc_aicl_rerun_time_cfg
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_get_aicl_rerun_cfg(uint32 pmic_device, pm_schg_misc_aicl_rerun_time_cfg *misc_aicl_cfg);

/**
* @brief This function gets shutdown status
* 
* @details
*  This function gets gets shutdown status
* 
* @param[in]  pmic_device_index.   Primary: 0 Secondary: 1
* @param[out] shdn_sts            FALSE = SHUTDOWN OFF, TRUE = SHUTDOWN ON
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/

pm_err_flag_type pm_schg_misc_get_shutdown_status(uint32 pmic_device, boolean *shdn_sts);

/**
* @brief This API sets buckboost refreshing pulse rate
* 
* @details
*  This API sets buckboost refreshing pulse rate
* 
* @param[in]  pmic_device_index.   Primary: 0 Secondary: 1
* @param[out] refresh_pulse_type   see pm_schg_misc_buckboost_refresh_pulse_type for details
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_set_buckboost_refresh_pulse(uint32 device_index, pm_schg_misc_buckboost_refresh_pulse_type refresh_pulse_type);

/**
* @brief This API sets thermistor source configuration for charging thermal mitigation (inov)
*
* @details
*  This API sets thermistor source configuration for charging thermal mitigation (inov)
*
* @param[in]  pmic_device_index.   Primary: 0 Secondary: 1
* @param[in] src_cfg_type   see pm_schg_misc_therm_src_cfg_type for details
* @param[in] enable   true for enable, false for disable the source.
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_misc_set_thermreg_src_cfg(uint32 device_index, pm_schg_misc_therm_src_cfg_type src_cfg_type, boolean enable);

#endif /* __PM_SCHG_MISC_H__*/

