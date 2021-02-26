#ifndef __PM_FG_SOC_H__
#define __PM_FG_SOC_H__


/*! \file pm_fg_soc.h 
 *  \n
 *  \brief  PMIC FG_SOC MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support 
 *   the PMIC Fuel Gauge SOC module.
 *
 *  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
 *  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/27/14   va      Driver cleanup update
08/13/14   sm      Changed return type for SOC pointer
06/25/14   va      Driver update, added protected call
04/08/14   va      Initial release
===========================================================================*/

#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"


/*===========================================================================
                        TYPE DEFINITIONS 
===========================================================================*/

typedef enum {
  /* 1 indicates FG cycle was stretched due to memory not released by system in time; Cleared by writing */
  PM_FG_SOC_ALG_CYCLE_STRETCHED = 0x01,
  /* 1 indicates a watchdop expiry event occurred since last time it was cleared, see FG_WDOG_EXP__CLR register */
  PM_FG_SOC_ALG_WDOG_EXP,
  /*FG Algorithm status bit to notify that the RELOAD/RESTART sequence is in progress, cleared when the sequence is completed */
  PM_FG_SOC_ALG_RESTART_IN_PROGRESS,
  /* FG Algorithm status bit to notify that the RELOAD/RESTART sequence is done, cleared when RESTART.GO is cleared */
  PM_FG_SOC_ALG_RESTART_DONE,
  /* FG Algorithm status bit to notify that the first iteration after a first SoC estimate is completed */
  PM_FG_SOC_ALG_FIRST_ITER_DONE,
  /* FG Algorithm status bit to notify that the First estimate of SoC is done, cleared on a battery missing event */
  PM_FG_SOC_ALG_BOOT_DONE,
  /* FG Algorithm READY Status bit asserted at the end of each SOC update, cleared on a battery missing event and on a shutdown event */
  PM_FG_SOC_ALG_RDY,
  PM_FG_SOC_ALG_RDY_INVALID
} pm_fg_soc_alg_sts;

typedef enum {

  /*  FG grants shutdown request
          0x0 : FG_DOES_NOT_ALOOW_SHDN
          0x1 : FG_ALLOWS_SHDN */
    PM_FG_SOC_SLEEP_SHUTDOWN_STS_FG_SHUTDOWN_ALLOWED = 0x04,

  /* FG receives shutdown Request 
        0x0 : SLEEP_NOT_REQUEST
        0x1 : SLEEP_REQUESTED */
  PM_FG_SOC_SLEEP_SHUTDOWN_STS_FG_SHUTDOWN_REQ,

  /* FG receives acknowledge that analog peripherals are in low power sleep state
        0x0 : FG_AWAKE
        0x1 : FG_IN_SHDN_OR_SLEEP */
  PM_FG_SOC_SLEEP_SHUTDOWN_STS_SHUTDOWN_OR_SLEEP,

  /* FG requests that analog peripherals enter low power sleep state
        0x0 : SLEEP_NOT_REQUEST
        0x1 : SLEEP_REQUESTED */
  PM_FG_SOC_SLEEP_SHUTDOWN_STS_FG_SLEEP_REQ,
  PM_FG_SOC_SLEEP_SHUTDOWN_STS_INVALID


} pm_fg_soc_sleep_shutdown_sts;

typedef enum {
  PM_FG_SOC_SW_FALLBK_NEW_BATT_RT_STS,
  PM_FG_SOC_SW_FALLBK_OCV_RT_STS,
  PM_FG_SOC_FIRST_EST_DONE_RT_STS,
  PM_FG_SOC_DELTA_SOC_RT_STS,
  PM_FG_SOC_EMPTY_SOC_RT_STS,
  PM_FG_SOC_FULL_SOC_RT_STS,
  PM_FG_SOC_LOW_SOC_RT_STS,
  PM_FG_SOC_HIGH_SOC_RT_STS
} pm_fg_soc_int_rt_sts;

typedef enum {
   PM_FG_SOC_HIGH_SOC_INT,
   PM_FG_SOC_LOW_SOC_INT,
   PM_FG_SOC_FULL_SOC_INT,
   PM_FG_SOC_EMPTY_SOC_INT,
   PM_FG_SOC_DELTA_SOC_INT,
   PM_FG_SOC_FIRST_EST_DONE_INT,
   PM_FG_SOC_SW_FALLBK_OCV_INT,
   PM_FG_SOC_SW_FALLBK_NEW_BATT_INT,
   PM_FG_SOC_IRQ_INVALID
}pm_fg_soc_irq_type;


typedef enum {

  PM_FG_SOC_PREVENT_OTP_PROFILE_RELOAD = 0x06,
  PM_FG_SOC_BOOT_MOD_INVALID,
  //PM_FG_SOC_PREVENT_OTP_SYS_RELOAD /* Not Available */

} pm_fg_soc_boot_mod;


typedef enum {

  PM_FG_SOC_FG_SOC_RESTART_GO, //4
  PM_FG_SOC_FG_SOC_RESTART_RELOAD_SYS,
  PM_FG_SOC_FG_SOC_RESTART_RELOAD_PROFILE,
  PM_FG_SOC_FG_SOC_RESTART_RERUN_FIRST_EST,
  PM_FG_SOC_FG_SOC_RESTART_REDO_BATID_DURING_FIRST_EST,
  PM_FG_SOC_FG_SOC_RESTART_INVALID

} pm_fg_soc_fg_soc_restart;

typedef enum {
  /* Any Write to this register clears WDOG_EXP bit in FG_ALG_STS register */
  PM_FG_SOC_WDONG_EXP = 0x07

} pm_fg_soc_wdog_exp;

typedef enum {
  /* Writing a 1 to this bit clears FG_ALG_STS__CYCLE_STRECHED status */
  PM_FG_SOC_CYCLE_STRETCH_CLR = 0x07

} pm_fg_soc_sts_clr;


/* Unlock the FG_SOC Secure Registers by writing 0xA5 to this register. Lock is rearmed after the next write to the module.*/
#define PM_FG_SOC_SEC_ACCESS_VALUE 0xA5

typedef struct {
  //uint8 pm_fg_soc_alg_sts_unused: 4;
  uint8 pm_fg_soc_alg_st_tracker;
}pm_fg_soc_alg_sts_cmp;


#define FG_SOC_DBG_CFG //define only API


typedef enum {

  PM_FG_SOC_FG_PB_TRIG_INV = 0x06,
  PM_FG_SOC_FG_PB_TRIG_DTEST_SEL_EN

}pm_fg_soc_fg_pbs_trig;

typedef struct {
  //uint8 pm_fg_soc_fg_pbs_trig_mux_unused: 1;
  uint8 pm_fg_soc_fg_pbs_trig_mux_sel;
}pm_fg_soc_fg_pbs_trig_mux;


typedef enum {
  /* 1=resets FG so mem can be accessed for test
        0x0 : FG_NOT_IN_RESET
        0x1 : FG_IN_RESET */
  PM_FG_SOC_RESET_CMD = 0x06,
 /*0x0 : HYBRID_DVDD_RB_RAW
     0x1 :  HYBRID_DVDD_RB */
  PM_FG_SOC_RESET_TYPE_SEL,
  PM_FG_SOC_RESET_INVLIAD

}pm_fg_soc_reset;



typedef struct {
  /* 1=resets FG so mem can be accessed for test
        0x0 : FG_NOT_IN_RESET
        0x1 : FG_IN_RESET */
  boolean pm_fg_soc_reset_cmd;
 /*0x0 : HYBRID_DVDD_RB_RAW
     0x1 :  HYBRID_DVDD_RB */
  boolean pm_fg_soc_reset_type_sel;

}pm_fg_soc_reset_type;



typedef enum {
  /*0x0 : I_ADC_DISCH_POS
       0x1 : I_ADC_DISCH_NEG */
  PM_FG_SOC_FG_CYA_CFG_I_ADC_CMP_POL_TRIM,
  /* 0x0 : V_ADC_POS_IS_POS
         0x1 : V_ADC_POS_IS_NEG  */
  PM_FG_SOC_FG_CYA_CFG_V_ADC_CMP_POL_TRIM, 
  /* Allows BCL to be 7 bits + sign or 8-bit
        0x0 : I_BCL_ADC_7_BIT_LONG
        0x1 : I_BCL_ADC_8_BIT_LONG */
  PM_FG_SOC_FG_CYA_CFG_I_BCL_ADC_BIT_SEL, 
  /* Allows BCL to be 7 bits + sign or 8-bit
        0x0 : I_BCL_ADC_7_BIT_LONG
        0x1 : I_BCL_ADC_8_BIT_LONG */
  PM_FG_SOC_FG_CYA_CFG_V_BCL_ADC_BIT_SEL, 
  /* 0x0 : I_ADC_FROM_BATFET
        0x1 : I_ADC_FROM_ISENSE */
  PM_FG_SOC_FG_CYA_CFG_I_ADC_SRC_SEL, 
  /* 0x0 : I_ADC_SRC_FROM_FG
        0x1 : I_ADC_SRC_FROM_FG_CYA */
  PM_FG_SOC_FG_CYA_CFG_I_ADC_SRC_MUX_SEL, 
  /*0x0 : DO_NOT_USE_ADC_LOW_CURRENT_MODE
        0x1 : USE_ADC_LOW_CURRENT_MODE */
  PM_FG_SOC_FG_CYA_CFG_LCSMB_TRIM,
  /* Set to a 1 to enable fg wdog timer
        0x0 : FG_WDOG_DISABLED
        0x1 : FG_WDOG_ENABLED*/
  PM_FG_SOC_FG_CYA_CFG_FG_WDOG_EN,
  PM_FG_SOC_FG_CYA_CFG_INVALID

}pm_fg_soc_fg_cya_cfg;


typedef enum {
  /* 0=FG clk at 1MHz throughout; 1=starts at 1MHz; switched to lower frequency ADC clock (200KHz) after first SoC estimate, obviating 1MHz oscillator for further FG needs
        0x0 : LO_FRQ_CLKSWITCH_DISABLED
        0x1 : LO_FRQ_CLKSWITCH_ENABLED */
  PM_FG_SOC_LOW_PWR_CFG_LQ_FRQ_CLKSWITCH_EN = 0x06,
  /* 1=enables sleep, dropping power consumption to a lower level
        0x0 : SLEEP_IS_DISABLED
        0x1 : SLEEP_IS_ENABLED */
  PM_FG_SOC_LOW_PWR_CFG_SLEEP_EN,
  PM_FG_SOC_LOW_PWR_CFG_INVALID


}pm_fg_soc_low_pwr_cfg;

typedef struct {
  /* 0=FG clk at 1MHz throughout; 1=starts at 1MHz; switched to lower frequency ADC clock (200KHz) after first SoC estimate, obviating 1MHz oscillator for further FG needs
        0x0 : LO_FRQ_CLKSWITCH_DISABLED
        0x1 : LO_FRQ_CLKSWITCH_ENABLED */
  boolean pm_fg_soc_low_pwr_cfg_lq_frq_clkswitch_en;
  /* 1=enables sleep, dropping power consumption to a lower level
        0x0 : SLEEP_IS_DISABLED
        0x1 : SLEEP_IS_ENABLED */
  boolean pm_fg_soc_low_pwr_cfg_sleep_en;

}pm_fg_soc_low_pwr_cfg_type;



/*===========================================================================
                        EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/**
* @brief This function returns  Fuel Gauge Algorithm status * 
* @details
*  This function returns  Fuel Gauge Algorithm status
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] pm_fg_soc_alg_sts. alg status
* @param[out] Enable. true/false status
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC
*          PM_ERR_FLAG_SUCCESS               = SUCCESS
*
*/
pm_err_flag_type pm_fg_soc_get_fg_alg_sts(uint32 device_index, pm_fg_soc_alg_sts alg_sts, boolean *enable);


/**
* @brief This function returns battery state of charge  * 
* @details
*  This function returns battery state of charge, 0xFF indicates 100%
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]mono_soc. % of state of charge (SOC) 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_soc_get_monotonic_soc(uint32 device_index, uint32 *mono_soc);


/**
 * @brief This function returns battery state of charge copy * 
 * @details
 *  This function returns battery state of charge, 0xFF indicates 100%
 * 
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[out]mono_soc. % of state of charge (SOC) 
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
  pm_err_flag_type pm_fg_soc_get_monotonic_soc_cp(uint32 device_index, uint8 *mono_soc_cp);


/**
* @brief This function returns  Fuel Gauge sleep shut down status * 
* @details
*  This function returns  Fuel Gauge sleep  shut down status
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]pm_fg_soc_sleep_shutdown_sts. sleep shutdown status in define enum 
* @param[out]enable. true/false 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_soc_get_sleep_shutdown_sts(uint32 device_index, pm_fg_soc_sleep_shutdown_sts fg_soc_shutdown_sts, boolean *enable);


/**
* @brief This function enables irq
* 
* @details
* This function enables irq
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_soc_irq_type               irq type
* @param[in] boolean        enable/disable value
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_soc_irq_enable(uint32 device_index, pm_fg_soc_irq_type irq, boolean enable);


/**
* @brief This function clears irq
* 
* @details
*  This function clears irq
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_soc_irq_type               irq type
*
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_soc_irq_clear(uint32  device_index, pm_fg_soc_irq_type irq);

/**
* @brief This function sets interrupt triggers
* 
* @details
*  This function sets interrupt triggers
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_soc_irq_type               irq type
* @param[in] pm_irq_trigger_type               trigger
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_soc_irq_set_trigger(uint32 device_index, pm_fg_soc_irq_type irq, pm_irq_trigger_type trigger);


/**
* @brief This function returns irq status
* 
* @details
*  This function returns irq status
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_soc_irq_type        irq type
* @param[in] pm_irq_status_type        irq status type
* @param[in] boolean        irq TRUE/FALSE status
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_soc_irq_status(uint32 device_index, pm_fg_soc_irq_type irq, pm_irq_status_type type, boolean *status);


/**
* @brief This function returns  fuel Gauge Boot Mode mofication setting *
* @details
*  This function returns  fuel Gauge Boot Mode mofication setting  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] pm_fg_soc_boot_mod. status in define enum 
* @param[out] enable. true/false value 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_soc_get_fg_soc_boot_mod(uint32 device_index, pm_fg_soc_boot_mod fg_soc_boot_mod, boolean *enable);


/**
* @brief This function sets  fuel Gauge Boot Mode *
* @details
*  This function sets  fuel Gauge Boot Mode 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]pm_fg_soc_boot_mod. sets defined mode in enum 
* @param[out]enable. true/false 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_soc_set_fg_soc_boot_mod(uint32 device_index, pm_fg_soc_boot_mod fg_soc_boot_mod, boolean enable);


/**
* @brief This function returns  Fuel Gauge restart status *
* @details
*  This function returns  fuel Gauge restart status  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] pm_fg_soc_fg_soc_restart. status in defined enum 
* @param[out enable. true/false 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_soc_get_fg_soc_restart(uint32 device_index, pm_fg_soc_fg_soc_restart fg_soc_restart, boolean *enable);


/**
* @brief This function sets  Fule Gauge restart configurations *
* @details
*  This function sets  Fule Gauge restart configurations 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in ]pm_fg_soc_fg_soc_restart. defined mode in enum 
* @param[out] enable true/false configuration to enable
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_soc_set_fg_soc_restart(uint32 device_index, pm_fg_soc_fg_soc_restart fg_soc_restart, boolean enable);

/**
* @brief This function clears WDOG_EXP bit in FG_ALG_STS register *
* @details
* This function clears WDOG_EXP bit in FG_ALG_STS register
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] fg_wdog_exp. Any value in range 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_soc_set_fg_soc_wdog_exp(uint32 device_index, uint8 fg_wdog_exp);

/**
* @brief This function clears FG_ALG_STS__CYCLE_STRETCHED Status register *
* @details
* This function clears FG_ALG_STS__CYCLE_STRETCHED Status register
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_soc_set_fg_soc_sts_clr(uint32 device_index);

/**
* @brief This function returns  Fuel Gauge reset Configuration *
* @details
* This function returns  Fuel Gauge reset Configuration 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] pm_fg_soc_reset_type. Returns current reset configuration 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_soc_get_reset_sts(uint32 device_index, pm_fg_soc_reset_type *fg_soc_reset_type);


/**
* @brief This function resets   Fuel Gauge *
* @details
* This function resets   Fuel Gauge 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] pm_fg_soc_reset. as defined in enum
* @param[in] enable. TRUE/FASLE value
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_soc_set_fg_reset(uint32 device_index, pm_fg_soc_reset fg_soc_reset, boolean enable);


/**
* @brief This function returns  Fuel Gauge low power configuration *
* @details
* This function returns  Fuel Gauge low power configuration 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out] pm_fg_soc_low_pwr_cfg_type. returns current low pwr cfg 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_soc_get_low_pwr_cfg(uint32 device_index, pm_fg_soc_low_pwr_cfg_type *fg_soc_low_pwr_cfg_type);

/**
* @brief This function sets  Fuel Gauge low power configuration *
* @details
* This function sets  Fuel Gauge low power configuration 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] pm_fg_soc_low_pwr_cfg_type. low pwr cfg 
* @param[out] enable. value to be set for low pwr cfg passed enum
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_soc_set_low_pwr_cfg(uint32 device_index, pm_fg_soc_low_pwr_cfg fg_soc_low_pwr_cfg, boolean enable);


#endif /* __PM_FG_SOC_H__ */


