#ifndef __PM_SCHG_CHGR_H__
#define __PM_SCHG_CHGR_H__

/*! \file
*  \n
*  \brief  pm_SMBB.h PMIC-SMBC MODULE RELATED DECLARATION 
*  \details  This header file contains functions and variable declarations 
*  to support Qualcomm PMIC SCHG (Switch-Mode Battery Charger) module. The 
*  Switched-Mode Battery Charger (SCHG) module includes a buck regulated 
*  battery charger with integrated switches. The SCHG module, along with the 
*  Over Voltage Proection (OVP) module will majorly be used by charger 
*  appliation for charging Li-Ion batteries with high current (up to 2A).
*  \n &copy; Copyright 2016-2017 QUALCOMM Technologies Incorporated, All Rights Reserved.
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who    what, where, why
--------   ---    ---------------------------------------------------------- 
12/19/16   sm     Added API to get SCHG Charger PmicIndex
03/01/16   sm     Added pm_schg_chgr_jeita_en_cfg()
02/16/16   va     Adding required API for enabling charger
01/24/16   al     Initial version. 
========================================================================== */

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/*! \enum pm_schg_chgr_irq_bit_field_type
   \brief different types of irq bit fields of by smbb irq module
 */
typedef enum {
  PM_SCHG_CHGR_IRQ_CHGR_ERROR,
  PM_SCHG_CHGR_IRQ_CHARGING_STATE_CHANGE,
  PM_SCHG_CHGR_IRQ_STEP_CHARGING_STATE_CHANGE,
  PM_SCHG_CHGR_IRQ_STEP_CHARGING_SOC_UPDATE,
  PM_SCHG_CHGR_IRQ_STEP_CHARGING_SOC_UPDATE_REQUEST,
  PM_SCHG_CHGR_IRQ_FG_FVCAL_QUALIFIED,
  PM_SCHG_CHGR_IRQ_INVALID
}pm_schg_chgr_irq_type;

typedef enum
{
  PM_SCHG_CHGR_PRE_CHARGE_CURRENT,
  /*pre_charge current*/
  PM_SCHG_CHGR_FAST_CHARGE_CURRENT,
  /*fast charge current*/
  PM_SCHG_CHGR_TERMINATION_CURRENT,
  /*termination current*/
  PM_SCHG_CHGR_CURRENT_TYPE_INVALID,
  /*invalid*/
}pm_schg_chgr_current_type;

typedef enum
{
  PM_SCHG_CHGR_CHGR_CFG_INCREASE_RCHG_TIMEOUT_CFG,
  /*Recharge glitch filter timeout: 0 = 20ms 1 = 5s */
  PM_SCHG_CHGR_CHGR_CFG_LOAD_BAT,
  /*When this bit is high, it will enable pulldown on VSYS*/
  PM_SCHG_CHGR_CHGR_CFG_CHG_EN_SRC,
  /*Charge Enable Source 0 = Command Register 1 = Enable Pin 0x0 */
  PM_SCHG_CHGR_CHGR_CFG_CHG_EN_POLARITY,
  /*Charge Enable Polarity*/
  PM_SCHG_CHGR_CHGR_CFG_PRETOFAST_TRANSITION_CFG,
  /*Pre to Fast Charge Transition 0 = Automatic 1 = Requires command*/
  PM_SCHG_CHGR_CHGR_CFG_BAT_OV_ECC,
  /*Battery OV Ends Charge Cycle 0 = Disabled 1 = Enabled*/
  PM_SCHG_CHGR_CHGR_CFG_I_TERM,
  /*Current Termination 0 = Enabled 1 = Disabled*/
  PM_SCHG_CHGR_CHGR_CFG_AUTO_RECHG,
  /*Automatic Recharge 0 = Enabled 1 = Disabled*/
  PM_SCHG_CHGR_CHGR_CFG_EN_ANALOG_DROP_IN_VBATT,
  /*ANALOG_DROP_IN_VBATT_DIS 0x1 : ANALOG_DROP_IN_VBATT_EN*/
  PM_SCHG_CHGR_CHGR_CFG_CHARGER_INHIBIT,
  /*0x0 : CHG_INHIBIT_DIS 0x1 : CHG_INHIBIT_EN*/
  PM_SCHG_CHGR_CHGR_CFG_CHG_ENB_TIMEOUT_SETTING,
  /*0x0 : CHG_ENB_TMR_30MS 0x1 : CHG_ENB_TMR_300MS*/
  PM_SCHG_CHGR_CHGR_CFG_FORCE_ZERO_CFG,
  /*0x0 : CHG_CUR_RAMPDWN_CFG1 0x1 : CHG_CUR_RAMPDWN_CFG2*/
  PM_SCHG_CHGR_CHGR_CFG_CHG_OPTION_PIN_TRIM,
  /* Fedback to CHG_OPTION_PIN of reg 0x08 of SCHGL_CHGR peripheral.*/
  PM_SCHG_CHGR_CHGR_CFG_BATN_SNS_CFG,
  PM_SCHG_CHGR_CHGR_CFG_CFG_TAPER_DIS_AFVC,
  PM_SCHG_CHGR_CHGR_CFG_BATFET_SHUTDOWN_CFG,
  /*0 = BATFET remains enabled during shutdown 1 = BATFET disabled during shutdown*/
  PM_SCHG_CHGR_CHGR_CFG_VDISCHG_EN_CFG,
  /*VCHG DischargeFunction 0 = Disabled 1 = Enabled*/
  PM_SCHG_CHGR_CHGR_CFG_VCHG_EN_CFG,
  /*VCHG Function 0 = Disabled 1 = Enabled*/
  PM_SCHG_CHGR_CHGR_CFG_INVALID,
}pm_schg_chgr_chgr_cgf_type;

typedef enum
{
  PM_SCHG_CHGR_CMD_CHARGING_ENABLE,
  /*0x0 : CHARGING_ENABLE_CMD_IS_INACTIVE 0x1 : CHARGING_ENABLE_CMD_IS_ACTIVE.*/
  PM_SCHG_CHGR_CMD_ALLOW_FAST_CHARGING,
  /*Command bit to allow automatic transition from pre-charge to fast-charge.*/
  PM_SCHG_CHGR_CMD_QNOVO_PT_ENABLE,
  /*Command bit to enable Qnovo algorithm*/
  PM_SCHG_CHGR_CMD_INVALID,
  /*INVALID*/
}pm_schg_chgr_cmd_type;

typedef enum
{
  PM_SCHG_BATT_CHGR_STATUS_TRICKLE,
  PM_SCHG_BATT_CHGR_STATUS_PRECHARGE,
  PM_SCHG_BATT_CHGR_STATUS_FAST,
  PM_SCHG_BATT_CHGR_STATUS_FULLON,
  PM_SCHG_BATT_CHGR_STATUS_TAPER,
  PM_SCHG_BATT_CHGR_STATUS_TERMINATION,
  PM_SCHG_BATT_CHGR_STATUS_INHIBIT,
  PM_SCHG_BATT_CHGR_STATUS_INVALID,
}pm_schg_battery_charger_status;

typedef struct
{
  pm_schg_battery_charger_status battery_charger_status;
  uint8   step_charging_status;
  boolean cc_soft_terminate;
  boolean bvr_initial_ramp;
}pm_schg_chgr_battery_charger_status_1;

typedef struct
{
  boolean bat_temp_status_too_cold;
  boolean bat_temp_status_too_hot;
  boolean bat_temp_status_cold_soft_limit;
  boolean bat_temp_status_hot_soft_limit;
  boolean charger_error_status_bat_term_missing;
  boolean charger_error_status_bat_ov;
  boolean charger_error_status_sft_expire;
  boolean input_current_limited;
}pm_schg_chgr_battery_charger_status_2;

typedef struct
{
  boolean taper_region;
  boolean enable_taper_sensor;
  boolean enable_chg_sensors;
  boolean taper;
  boolean charging_enable;
  boolean force_zero_charge_current;
  boolean disable_charging;
  boolean valid_input_power_source;
}pm_schg_chgr_battery_charger_status_5;

typedef struct
{
  boolean cold_sl_adc;
  boolean hot_sl_adc;
  boolean too_hot_adc;
  boolean too_cold_adc;
  boolean enable_fullon_mode;
  boolean enable_fast_charging;
  boolean enable_pre_charging;
  boolean enable_trickle;
}pm_schg_chgr_battery_charger_status_7;


typedef struct
{
  pm_schg_chgr_battery_charger_status_1 status_1;
  pm_schg_chgr_battery_charger_status_2 status_2;
  uint8                                 status_3;
  uint8                                 status_4;
  pm_schg_chgr_battery_charger_status_5 status_5;
  uint8                                 status_6;
  pm_schg_chgr_battery_charger_status_7 status_7;
  uint8                                 status_8;
}pm_schg_chgr_status_type;

typedef enum
{
  PM_SCHG_CHGR_JEITA_COLD_SL_CCC,       /** <Cold SL Charge Current Compensation              >*/
  PM_SCHG_CHGR_JEITA_HOT_SL_CCC,        /** <Hot SL Charge Current Compensation               >*/
  PM_SCHG_CHGR_JEITA_COLD_SL_FVC,       /** <Cold SL Float Voltage Compensation               >*/
  PM_SCHG_CHGR_JEITA_HOT_SL_FVC,        /** <Hot SL Float Voltage Compensation                >*/
  PM_SCHG_CHGR_JEITA_HARD_LIMIT,        /** <Jeita Temperature Hard Limit disable             >*/
  PM_SCHG_CHGR_JEITA_INVALID            /** < Invalid                                         >*/
}pm_schg_chgr_jeita_cfg_type;

typedef enum
{
  PM_SCHG_CHGR_PRE_CHARGE_SAFETY_TIMER_EN,  /* Pre-Charge Safety Timers Enabled */
  PM_SCHG_CHGR_FAST_CHARGE_SAFETY_TIMER_EN, /* Fast-Charge Safety Timer Enabled */
  PM_SCHG_CHGR_CFG_SAFETY_TIMER_INVAID
}pm_schg_chgr_safety_timer_enable_cfg;

typedef enum
{
  PM_SCHG_CHGR_PRE_CHARGE_TIMEOUT_24_MIN,  /* Pre-Charge Safety Time out */
  PM_SCHG_CHGR_PRE_CHARGE_TIMEOUT_48_MIN,  /* Pre-Charge Safety Time out*/
  PM_SCHG_CHGR_PRE_CHARGE_TIMEOUT_96_MIN,  /* Pre-Charge Safety Time out*/
  PM_SCHG_CHGR_PRE_CHARGE_TIMEOUT_192_MIN, /* Pre-Charge Safety Time out*/
  PM_SCHG_CHGR_PRE_CHARGE_SAFETY_TIMEOUT_INVAID
}pm_schg_chgr_pre_charge_safety_timeout;

typedef enum
{
  PM_SCHG_CHGR_P2F_ANALOG_2P4V,  /* Pre-Charge Safety Time out */
  PM_SCHG_CHGR_P2F_ANALOG_2P6V, /* Pre-Charge Safety Time out*/
  PM_SCHG_CHGR_P2F_ANALOG_2P8V,  /* Pre-Charge Safety Time out*/
  PM_SCHG_CHGR_P2F_ANALOG_3P0V, /* Pre-Charge Safety Time out*/
  PM_SCHG_CHGR_P2F_INVALID,
}pm_schg_chgr_p2f_thrd_cfg;

typedef enum
{
  PM_SCHG_CHGR_FAST_CHARGE_TIMEOUT_192_MIN,  /* Fast-Charge Safety Time out */
  PM_SCHG_CHGR_FAST_CHARGE_TIMEOUT_384_MIN,   /* Fast-Charge Safety Time out*/
  PM_SCHG_CHGR_FAST_CHARGE_TIMEOUT_768_MIN,   /* Fast-Charge Safety Time out*/
  PM_SCHG_CHGR_FAST_CHARGE_TIMEOUT_1536_MIN,  /* Fast-Charge Safety Time out*/
  PM_SCHG_CHGR_FAST_CHARGE_SAFETY_TIMEOUT_INVAID
} pm_schg_chgr_fast_charge_safety_timeout;

/*===========================================================================

                        SCHG DRIVER FUNCTION PROTOTYPES

===========================================================================*/


/**
* @brief This function enables irq
*
* @details
*  This function enables irq
*
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] irq                 pm_schg_chgr_irq_type
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_irq_enable(uint32 device_index, pm_schg_chgr_irq_type irq, boolean enable);


/**
* @brief This function clears the SCHG irq
*
* @details
*  This function clears the SCHG irq
*
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] irq                 pm_schg_chgr_irq_type
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_irq_clear(uint32  device_index, pm_schg_chgr_irq_type irq);


/**
* @brief This function configures the SCHG irq trigger
*
* @details
*  This function configures the SCHG irq trigger
*
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] irq                 pm_schg_chgr_irq_type
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_irq_set_trigger(uint32 device_index, pm_schg_chgr_irq_type irq, pm_irq_trigger_type trigger);


/**
* @brief This function configures the SCHG for irq
*
* @details
*  This function configures the SCHG for irq
*
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] irq                 pm_schg_chgr_irq_type
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_irq_status(uint32 device_index, pm_schg_chgr_irq_type irq, pm_irq_status_type type, boolean *status);

/**
 * @brief This function enables source as command or pin
 *        controlled.
 *
 * @details
 *  This function enables source as command or pin controlled.
 *
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] chg_en_src          TRUE: Enables Pin
 *                                FALSE: Command Register
 *
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_chgr_enable_src(uint32 device_index, boolean chg_en_src);

/**
 * @brief This function sets the command/pin polarity
 *
 * @details
 *  This function sets the command/pin polarity .
 *
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] chg_pol_low         TRUE: sets polarity low
 *                                FASLE: sets polarity high
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_chgr_set_chg_polarity_low(uint32 device_index, boolean chg_pol_low);

 /**
 * @brief This function sets the various type of current
 *
 * @details
 *  This function sets the various type of current
 *
 * @param[in] pmic_device_index.   Primary: 0 Secondary: 1
 * @param[in] charge_current_type  Refer #pm_schg_chgr_current_type
 * @param[in] current              current value
 *
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_chgr_set_charge_current(uint32 device_index, pm_schg_chgr_current_type charge_current_type, uint32 current);

/**
 * @brief This function gets the various type of current
 *
 * @details
 *  This function sets the various type of current
 *
 * @param[in] pmic_device_index.   Primary: 0 Secondary: 1
 * @param[in] charge_current_type  refer #pm_schg_chgr_current_type
 * @param[out] current             current value
 *
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_chgr_get_charge_current(uint32 device_index, pm_schg_chgr_current_type charge_current_type, uint32 *current);


 /**
 * @brief This function sets the various type of charge configuration
 *
 * @details
 *  This function sets the various type of current
 *
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] chgr_cfg            refer #pm_schg_chgr_chgr_cgf_type
 * @param[in] set_value           value to be set
 *
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_chgr_set_chgr_cfg(uint32 device_index, pm_schg_chgr_chgr_cgf_type chgr_cfg, boolean set_value);

/**
 * @brief This function sets the various type of charge cmd registers
 *
 * @details
 *  This function sets the various type of charge cmd registers
 *
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] chgr_cmd            refer #pm_schg_chgr_cmd_type
 * @param[in] set_value           value to be set
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_chgr_set_chgr_cmd_type(uint32 device_index, pm_schg_chgr_cmd_type chgr_cmd, boolean set_value);

/**
 * @brief This function returns the various charger status type
 * 
 * @details
 *  This function returns the various charger status type
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[out] charger_status      refer #pm_schg_chgr_status_type
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_chgr_get_chgr_status(uint32 device_index, pm_schg_chgr_status_type *charger_status);

/**
* @brief This function This API configures the floating voltage
* 
* @details
* This API configures the floating voltage. Valid range is 3600mV to 4500 mv
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] float_volt_mv       Valid range is 3600mV to 4500 mv
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_set_float_volt(uint32 device_index, uint32 float_volt_mv);

/**
* @brief This function returns configured floating voltage
* 
* @details
*  This function returns set floating voltage
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] float_volt_mv      set float voltage 
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_get_float_volt_cfg(uint32 device_index, uint32 *float_volt_mv);

/**
* @brief This function sets the pre-charge to full charger 
*        threshold
* 
* @details
*  This API sets the pre-charge to full charger threshold. Valid range is 2400mV to 3000mV
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] milli_volt          Valid range is 2400mV to 3000mV
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_set_p2f_threshold(uint32 device_index, uint32 milli_volt);

/**
 * @brief This function returns the pre-charge to full charger 
 *        threshold
 * 
 * @details
 *  This API returns the pre-charge to full charger threshold.
 *  Valid range is 2400mV to 3000mV
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[out] milli_volt          Valid range is 2400mV to 
 *       3000mV
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_chgr_get_p2f_threshold(uint32 device_index, uint32 *milli_volt);

/**
* @brief This function sets Jeita floating voltage conpmensation cfg value
* 
* @details
*  This api sets floating voltage conpmensation code. Valid
*  value is from 0 to 63
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] float_volt_cmpn     Valid value is from 0 to 63 
*                         
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_set_jeita_fvcomp_cfg(uint32 device_index, uint32 fv_cmpn);

/**
* @brief This function reads Jeita floating voltage conpmensation cfg
* 
* @details
*  This api reads floating voltage conpmensation code. Valid
*  value is from 0 to 6300mV
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] float_volt_cmpn     Valid value is from 0 to 6300 mV 
*                         
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_get_jeita_fvcomp_cfg(uint32 device_index, uint32 *fv_comp);


/**
* @brief This function configures the fast-charger current compensation value in milliamp
* 
* @details
*  This API configures the fast-charger current compensation value in milliamp.
*  Valid values are 300 to 1200 mAmp
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] fast_chg_i_cmp_ma   fast-charger current compensation value in milliamp. 
*                                Valid values are 300 to 1200 mAmp
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_set_jeita_ccomp(uint32 device_index, uint32 fccomp_ma);

/**
* @brief This function gets the fast-charger current value in milliamp. Valid values are 300 to 3000 mAmp
* 
* @details
*  This API gets the fast-charger current value in milliamp. Valid values are 300 to 3000 mAmp
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] *fast_chg_i_ma      fast-charger current value in milliamp. Valid values are 300 to 3000 mAmp 
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_get_jeita_ccomp_cfg(uint32 device_index, uint32 *fccomp_ma);

/**
* @brief This function configures pre charge safety timer
* 
* @details
*  This function configures safety timer
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] chg_sfty_timeout    Total pre-charge safety timer timeout. Valid values is 24 to 192 minutes
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_config_pre_charge_safety_timer(uint32 device_index, pm_schg_chgr_pre_charge_safety_timeout chg_sfty_timeout);

/**
* @brief This function configures fast charge safety timer
* 
* @details
*  This function configures safety timer
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] chg_sfty_timeout    Total pre-charge safety timer timeout. Valid values is 192 to 1536 minutes
*
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_config_fast_charge_safety_timer(uint32 device_index, pm_schg_chgr_fast_charge_safety_timeout chg_sfty_timeout);

/**
* @brief This function returns configured pre charge safety timer
* 
* @details
*  This function returns configured safety timer
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] chg_sfty_timeout   Total pre-charge safety timer timeout. Valid values is 24 to 192 minutes
*
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_get_safety_timer_pre_charge_config(uint32 device_index, pm_schg_chgr_pre_charge_safety_timeout *chg_sfty_timeout);

/**
* @brief This function returns configured pre charge safety timer
* 
* @details
*  This function returns configured safety timer
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] chg_sfty_timeout   Total charge safety timer timeout. Valid values is 192 to  1536 minutes
*
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_get_safety_timer_fast_charge_config(uint32 device_index, pm_schg_chgr_fast_charge_safety_timeout *chg_sfty_timeout);

/**
* @brief This function helps enable HW JEITA related parameters. 
* 
* @details
*  This function helps enable HW JEITA related parameters. 
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] jeita_cfg_type      JEITA config type see pm_schg_chgr_jeita_cfg_type for details
* @param[in] enable              Enables the config; 0: FALSE, 1: TRUE 
*
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_jeita_en_cfg(uint32 device_index, pm_schg_chgr_jeita_cfg_type jeita_cfg_type, boolean enable);

/**
* @brief This function gets  HW JEITA related parameters.status
* 
* @details
*  This function gets HW JEITA related parameters status
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] jeita_cfg_type      JEITA config type see pm_schg_chgr_jeita_cfg_type for details
* @param[out]enable              Enables the config; 0: FALSE, 1: TRUE 
*
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_chgr_get_jeita_cfg(uint32 device_index, pm_schg_chgr_jeita_cfg_type jeita_cfg, boolean *enable);

/**
* @brief This function provides SCHG charger PMIC info.
*
* @details
*  This function provides SCHG charger PMIC info.
*
* @param[out] pmic_index         On what PMIC Index, SCHG charger resides.
* @param[out] slave_id           On what SlaveID for given PMIC, SCHG charger resides.
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available.
*          PM_ERR_FLAG_INVALID_PARAMETER     = Invalid parameters.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_schg_chgr_get_schg_pmic_info(uint8 *pmic_index, uint8 *slave_id);

#endif /* __PM_SCHG_CHGR_H__ */

