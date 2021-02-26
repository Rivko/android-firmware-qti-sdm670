#ifndef __PM_FG_BATT_H__
#define __PM_FG_BATT_H__


/*! \file pm_fg_batt.h 
 *  \n
 *  \brief  PMIC-BMS MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support 
 *   the PMIC BMS Fule Gauge Battery module.
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
12/02/14   va      Correcting Read API
08/27/14   va      Driver cleanup update
06/25/14   va      Driver update, added protected call
04/14/14   va      Initial Release 
===========================================================================*/

#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================
                        TYPE DEFINITIONS 
===========================================================================*/

typedef struct pm_fg_batt_profile_id_in_use{
  /* Battery Profile ID currently in use by the algorithm */
  uint8 pm_fg_batt_prof_id;
} pm_fg_batt_profile_id_in_use;


typedef enum {
  /* Battery Profile that is Forced when SW_BATT_ID_FORCE is asserted
        0x0 : SW_BATT_ID0
        0x1 : SW_BATT_ID1
        0x2 : SW_BATT_ID2
        0x3 : SW_BATT_ID3*/
  PM_FG_BATT_SW_BATT_ID0,
  PM_FG_BATT_SW_BATT_ID1 ,
  PM_FG_BATT_SW_BATT_ID2,
  PM_FG_BATT_SW_BATT_ID3,
  PM_FG_BATT_SW_BATT_PROFILE_ID_INVALID
} pm_fg_batt_profile_id;


/*FG BATT SYSTEM STS */
typedef enum 
{
  PM_FG_BATT_IS_MISSING,
  PM_FG_BATT_CHARRGING_CV,
  PM_FG_BATT_CHARRGING_CC,
  PM_FG_BATT_SYSTEM_FULL,
  PM_FG_BATT_DISCHARGING,
  PM_FG_BATT_AUTO_RECHARGE,
  PM_FG_BATT_SUPPLEMENTAL,
  PM_FG_BATT_EMPTY,
  PM_FG_BATT_INVALID,

} pm_fg_batt_fg_sys_status;

typedef enum 
{
  PM_FG_BATT_PRESENT,
  PM_FG_BATT_REM_LATCHED

} pm_fg_batt_rem_latched_status;


typedef enum 
{
  PM_FG_BATT_VOLTTAGE_NORMAL,
  PM_FG_BATT_VOLTAGE_LOW

} pm_fg_batt_low_rt_status;

typedef enum 
{
  PM_FG_BATT_PROFILE_ORIGINAL,
  PM_FG_BATT_PROFILE_CHANGED

} pm_fg_batt_profile_status;


typedef struct pm_fg_batt_sys_sts{
  /* Refer pm_fg_batt_fg_sys_status 
        FG System status, ISMISSING=0, CHARGING_CV=1, CHARGING_CC=2, SYSTEM_FULL=3, DISCHARGING=4, AUTOMATIC_RECHARGE=5, SUPPLEMENTAL=6, EMPTY=7 */
  uint8   pm_fg_batt_fg_sys_sts;
  /*1: battery is/was removed; Once set can be cleared only through a software write of BATT_REMOVED_LATCHED__CLR
      0x0 : BATT_PRESENT
      0x1 : BATT_REMOVED_LATCHED*/
  BOOLEAN pm_fg_batt_rem_latched_sts;
  /* 1: battery voltage low - comparator output
      0x0 : BATT_VOLTAGE_NORMAL
      0x1 : BATT_VOLTAGE_LOW  */
  BOOLEAN pm_fg_batt_low_rt_sts;
  /* 1: Battery profile data store in RAM does not match OTP data
      0x0 : BATT_PROFILE_ORIGINAL
      0x1 : BATT_PROFILE_CHANGED*/
  BOOLEAN pm_fg_batt_profile_sts;
} pm_fg_batt_sys_sts;


typedef enum 
{
  PM_FG_BATT_BATT_ID_NOT_SHORTED_TO_GND,
  PM_FG_BATT_BATT_ID_SHORTED_TO_GND

} pm_fg_batt_det_sts_gnd;

typedef enum 
{
  PM_FG_BATT_BATT_ID_NOT_FLOAT_TO_GND,
  PM_FG_BATT_BATT_ID_FLOAT_TO_GND

} pm_fg_batt_det_sts_float;


typedef enum 
{
  PM_FG_BATT_BATT_ID_DETECTION_NOT_SMART,
  PM_FG_BATT_BATT_ID_DETECTION_SMART

} pm_fg_batt_det_sts_smart;


typedef enum 
{
  PM_FG_BATT_BATT_ID_DETECTION_OK,
  PM_FG_BATT_BATT_ID_DETECTION_ERR

} pm_fg_batt_det_sts_err;


typedef enum {
  /* Battery ID is shorted to ground
      0x0 : BATT_ID_NOT_SHORTED_TO_GND
      0x1 : BATT_ID_SHORTED_GND */
  PM_FG_BATT_DET_STS__GND = 0x04,
  /* Battery ID is floating to ground
      0x0 : BATTERY_ID_NOT_FLOAT_TO_GND
      0x1 : BATTERY_ID_FLOAT_TO_GND */
  PM_FG_BATT_DET_STS_FLOAT,
  /* Battery ID is smart
      0x0 : BATT_ID_DETECTION_NOT_SMART
      0x1 : BATT_ID_DETECTION_SMART */
  PM_FG_BATT_DET_STS_SMART,
  /* Battery ID detection error,FALSE=0,TRUE=1
      0x0 : BATT_ID_DETECTION_OK
      0x1 : BATT_ID_DETECTION_ERR  */
  PM_FG_BATT_DET_STS_ERR,
  PM_FG_BATT_DET_STS_INVALID
} pm_fg_batt_det_sts;

typedef enum 
{
  PM_FG_BATT_ESR_NOT_UPDATED_THIS_CYCLE,
  PM_FG_BATT_ESR_UPDATED_THIS_CYCLE

} pm_fg_batt_jeita_st_info_esr;

typedef enum 
{
  PM_FG_BATT_JEITA_BELOW_HARD_HOT_THRESHOLD,
  PM_FG_BATT_JEITA_ABOVE_HARD_HOT_THRESHOLD

} pm_fg_batt_jeita_st_info_hard_hot;

typedef enum 
{
  PM_FG_BATT_JEITA_BELOW_HARD_COLD_THRESHOLD,
  PM_FG_BATT_JEITA_ABOVE_HARD_COLD_THRESHOLD

} pm_fg_batt_jeita_st_info_hard_cold;

typedef enum {
  /* 
      0x0 : JEITA_BELOW_HARD_COLD_THRESHOLD
      0x1 : JEITA_ABOVE_HARD_COLD_THRESHOLD */
  PM_FG_BATT_JEITA_HARD_COLD_RT_STS = 0x05,
  /* 
      0x0 : JEITA_BELOW_HARD_HOT_THRESHOLD
      0x1 : JEITA_ABOVE_HARD_HOT_THRESHOLD */
  PM_FG_BATT_JEITA_HARD_HOT_RT_STS,
  /* 1=Battery Estimated Resistance updated by FG algorithm this cycle (a cycle is 1.4 seconds)
      0x0 : ESR_NOT_UPDATED_THIS_CYCLE
      0x1 : ESR_UPDATED_THIS_CYCLE */
  PM_FG_BATT_ESR_UPDATE,
  PM_FG_BATT_JEITA_RT_INFO_INVALID

} pm_fg_batt_jeita_rt_info;


typedef enum {
  /* If MISC_CFG__TIMEOUT_EN is set and RECOVERY_DATA_WAIT is 1, if software does not (provide recovery data and) set BATT_RECOVERY__RECOVERY_DATA_RDY (timeout expires),
        then fgAlg proceeds without recovery data and sets this bit
      0x0 : RECOVERY_TIMEOUT_FALSE
      0x1 : RECOVERY_TIMEOUT_TRUE */
  PM_FG_BATT_RECOVERY_TIMEOUT_STS = 0x05,
  /* After software provides recovery data, it sets BATT_RECOVERY__RECOVERY_DATA_RDY. This bit then gets set and is cleared by fgAlg when it asserts boot_done
      0x0 : RECOVERY_DATA_NOT_ACK
      0x1 : RECOVERY_DATA_ACK */
  PM_FG_BATT_RECOVERY_DATA_ACK,
  /* If MISC_CFG__RECOVERY_DATA_EN is set, when battery is re-inserted, fgAlg indicates that it is waiting for recovery data from system (software)
      0x0 : RECOVERY_DATA_NOT_WAIT
      0x1 : RECOVERY_DATA_WAIT */
  PM_FG_BATT_RECOVERY_DATA_WAIT,
  PM_FG_BATT_RECOVERY_STS_INVALID

} pm_fg_batt_recovery_sts;


typedef enum{
  PM_FG_BATT_JEITA_SOFT_COLD_INT_SET_TYPE =0,
  PM_FG_BATT_JEITA_SOFT_HOT_INT_SET_TYPE ,
  PM_FG_BATT_VBATT_LOW_INT_SET_TYPE,
  PM_FG_BATT_BATT_IDENTIFIED_INT_SET_TYPE,
  PM_FG_BATT_BATT_ID_REQ_INT_SET_TYPE,
  PM_FG_BATT_BATT_UNKNOWN_INT_SET_TYPE,
  PM_FG_BATT_BATT_MISSING_INT_SET_TYPE,
  PM_FG_BATT_BATT_MATCH_INT_SET_TYPE,
  PM_FG_BATT_IRQ_INVALID
} pm_fg_batt_irq_type;


typedef enum 
{
  PM_FG_BATT_SW_BATT_ID_FORCE_BATID_FALSE,
  PM_FG_BATT_SW_BATT_ID_FORCE_BATID_TRUE,
  PM_FG_BATT_SW_BATT_ID_FORCE_INVALID

} pm_fg_batt_sw_batt_id_force_sts;


typedef enum 
{  /*
      0x0 : BATT_REMOVED_LATCH_CLEAR_FALSE
      0x1 : BATT_REMOVED_LATCH_CLEAR_TRUE */
  PM_FG_BATT_REMOVED_LATCH_CLEAR_FALSE,
  PM_FG_BATT_REMOVED_LATCH_CLEAR_TRUE,
  PM_FG_BATT_REMOVED_LATCH_CLEAR_INVALID

} pm_fg_batt_latch;

typedef enum 
{  /*
      0x0 : RECOVERY_DATA_RDY_FALSE
      0x1 : RECOVERY_DATA_RDY_TRUE*/
  PM_FG_BATT_RECOVERY_DATA_RDY_FALSE,
  PM_FG_BATT_RECOVERY_DATA_RDY_TRUE,
  PM_FG_BATT_RECOVERY_DATA_RDY_INVALID

} pm_fg_batt_recovery_rdy;

typedef enum 
{  /*Hitting ESR Lower saturation threshold
      0x0 : ESR_ABOVE_LO_SATURATE_THRESH
      0x1 : ESR_BELOW_LO_SATURATE_THRESH*/
  PM_FG_BATT_ESR_BOUND_LOWER_HIT = 0x05,
  /* Hitting ESR Higher saturation threshold
      0x0 : ESR_BELOW_HI_SATURATE_THRESH
      0x1 : ESR_ABOVE_HI_SATURATE_THRESH*/
  PM_FG_BATT_ESR_BOUND_UPPER_HIT,
/* Low temperature region, using extended threshold for ESR
      0x0 : ESR_LOW_TEMP_RANGE_FALSE
      0x1 : ESR_LOW_TEMP_RANGE_TRUE*/
  PM_FG_BATT_ESR_BOUND_LOW_TEMP_RANGE,

  PM_FG_BATT_ESR_BOUND_CLEAR_INVALID

} pm_fg_batt_esr_change_bound_sts;

typedef struct pm_fg_batt_peek_mux{
  uint8 pm_fg_batt_peek_mux_dtest_sel;
}pm_fg_batt_peek_mux;


typedef enum 
 {  /* 0=GND (use when thermistor on PCB), 1=VBATTN (use when thermistor in battery pack)
      0x0 : THERMISTOR_ON_PCB
      0x1 : THERMISTOR_ON_BATT */
  PM_FG_BATT_MISC_CFG_THERMISTOR_CFG,
  /* If RECOVERY_DATA_EN is set and fgAlg begins waiting(FG_BATT_SYS_BATT__RECOVERY_DATA_WAIT=1), a timeout(~1 second) can be enabled for system to provide recovery data. After expiration, fgAlg proceeds without recovery information
      0x0 : RECOVERY_TIMEOUT_DISABLE
      0x1 : RECOVERY_TIMEOUT_ENABLE */
  PM_FG_BATT_MISC_CFG_RECOVERY_TIMEOUT_EN,
/* System is expected to provide info (alias: restore enable)
      0x0 : RECOVERY_DATA_DISABLE
      0x1 : RECOVERY_DATA_ENABLE */
  PM_FG_BATT_MISC_CFG_RECOVERY_DATA_EN,

  PM_FG_BATT_MISC_CFG_INVALID

} pm_fg_batt_mis_cfg;


typedef struct pm_fg_batt_id_current{
  uint8 pm_fg_batt_id_current;
}pm_fg_batt_id_current;

typedef enum 
 {
  /* ESR Measurement Current: 00=60mA, 01=120mA, 10=180mA, 11=240mA */
  PM_FG_BATT_ESR_MEAS_CUR_60MA,
  PM_FG_BATT_ESR_MEAS_CUR_120MA,
  PM_FG_BATT_ESR_MEAS_CUR_180MA,
  PM_FG_BATT_ESR_MEAS_CUR_240MA,
  PM_FG_BATT_ESR_CUR_INVALID

} pm_fg_batt_esr_cur;



typedef enum 
 {
  PM_FG_BATT_ESR_MEAS_ENABLE,
  PM_FG_BATT_ESR_MEAS_DISBALE,
  PM_FG_BATT_ESR_CUR_CFG_INVALID

} pm_fg_batt_esr_cur_cfg;

typedef enum 
 {/* Reference for RBIAS Ratiometric Conversions: 0=unbuffered, 1= buffered */
  PM_FG_BATT_RBIAS_REF_UNBUF,
  PM_FG_BATT_RBIAS_REF_BUF,
  PM_FG_BATT_RBIAS_DIV_INVALID

} pm_fg_batt_rbias_div;


/*===========================================================================
                        EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/**
* @brief This function returns BMS Fule Gauge Algorithm profile id currently in use* 
* @details
*  This function returns BMS Fule Gauge Algorithm profile id currently in use
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]pm_fg_batt_profile_id_in_use  profile number in use
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_get_batt_profile_id_in_use(uint32 device_index, pm_fg_batt_profile_id_in_use* profile_id_in_use);


/**
* @brief This function returns BMS Fule Gauge Batt system status * 
* @details
*  This function returns BMS Fule Gauge Batt system status 
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]pm_fg_batt_sys_sts system profile status
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_get_sys_batt_sts(uint32 device_index, pm_fg_batt_sys_sts *batt_sys_profile);


/**
 * @brief This function returns BMS Fule Gauge Batt detection status * 
 * @details
 *  This function returns BMS Fule Gauge Batt detection status 
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[out]pm_fg_batt_det_sts battery detection status
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_batt_detect_batt(uint32 device_index, pm_fg_batt_det_sts batt_det_sts, boolean *enable);



/**
 * @brief This function returns BMS Fule Gauge Batt detection status * 
 * @details
 *  This function returns BMS Fule Gauge Batt detection status 
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[out]pm_fg_batt_det_sts battery detection status
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_batt_jeita_rt_info_sts(uint32 device_index, pm_fg_batt_jeita_rt_info batt_jeita_info, boolean *enable);


/**
 * @brief This function returns BMS Fule Gauge Batt recovery status * 
 * @details
 *  This function returns BMS Fule Gauge Batt recovery status 
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[out]pm_fg_batt_recovery_sts Battery Recovery status
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_batt_get_batt_recovery_sts(uint32 device_index, pm_fg_batt_recovery_sts batt_recovery_sts, boolean *enable);

/* common interrupt prototype */
/**
* @brief This function enables irq
* 
* @details
* This function enables irq
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_batt_irq_type               irq type
* @param[in] boolean        enable/disable value
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_irq_enable(uint32 device_index, pm_fg_batt_irq_type irq, boolean enable);


/**
* @brief This function clears irq
* 
* @details
*  This function clears irq
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_batt_irq_type               irq type
*
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_irq_clear(uint32  device_index, pm_fg_batt_irq_type irq);

/**
* @brief This function sets interrupt triggers
* 
* @details
*  This function sets interrupt triggers
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_batt_irq_type               irq type
* @param[in] pm_irq_trigger_type               trigger
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_irq_set_trigger(uint32 device_index, pm_fg_batt_irq_type irq, pm_irq_trigger_type trigger);


/**
* @brief This function returns irq status
* 
* @details
*  This function returns irq status
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_batt_irq_type        irq type
* @param[in] pm_irq_status_type        irq status type
* @param[in] boolean        irq TRUE/FALSE status
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_irq_status(uint32 device_index, pm_fg_batt_irq_type irq, pm_irq_status_type type, boolean *status);


/**
* @brief This function returns BMS fuel Gauge Boot Mode mofication setting *
* @details
*  This function returns BMS fuel Gauge Boot Mode mofication setting  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]pm_fg_batt_boot_mod. returns status in define enum 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/

/* End of common interrupt prototype */

/**
 * @brief  This function returns status of 'Force the Battery ID during a First SoC redetection' *
 * @details
 *  This function returns status of 'Force the Battery ID during a First SoC redetection'
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[out]pm_fg_batt_sw_batt_id_force_sts software batter id force status
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_batt_read_sw_batt_id(uint32 device_index, pm_fg_batt_sw_batt_id_force_sts *sw_batt_id_sts);


/**
 * @brief  This function forces status of 'Force the Battery ID during a First SoC redetection' *
 * @details
 *  This function forces status of 'Force the Battery ID during a First SoC redetection'
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[out]pm_fg_batt_sw_batt_id_force_sts software batter id force status
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_batt_force_sw_batt_id(uint32 device_index, pm_fg_batt_sw_batt_id_force_sts sw_batt_id_sts);

/**
 * @brief This function returns BMS Fule Gauge Algorithm current profile id currently in use * 
 * @details
 *  This function returns BMS Fule Gauge Algorithm current profile id currently in use 
 *   Battery Profile that is Forced when SW_BATT_ID_FORCE is asserted
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[out]pm_fg_batt_profile_id Profile number currently set
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_batt_get_batt_profile_id(uint32 device_index, pm_fg_batt_profile_id *profile_id);

/**
 * @brief This function sets BMS Fule Gauge Algorithm current profile id currently in use * 
 * @details
 *  This function sets BMS Fule Gauge Algorithm current profile id currently in use 
 *   Battery Profile that is Forced when SW_BATT_ID_FORCE is asserted
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[out]pm_fg_batt_profile_id Profile number currently set
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_batt_set_batt_profile_id(uint32 device_index, pm_fg_batt_profile_id profile_id);

/**
 * @brief This function clear BMS Fule Gauge battery latched status * 
 * @details
 *  This function clear BMS Fule Gauge battery latched status
 *  Writing a 1, will clear FG_BATT_SYS_BATT.BATT_REMOVED_LATCH; If battery remains missing, the latch will get set again 
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[out]pm_fg_batt_profile_id Profile number currently set
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_batt_clr_batt_latched(uint32 device_index, pm_fg_batt_latch profile_id);

/**
 * @brief This function sets recovery data ready after BMS Fule Gauge gives recovery data information * 
 * @details
 *  This function sets recovery data ready after BMS Fule Gauge gives recovery data information 
 *  Set by software after it provides recovery information
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[out]pm_fg_batt_recovery_rdy recovery data value
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_batt_set_recovery_rdy(uint32 device_index, pm_fg_batt_recovery_rdy recovery_rd);

/**
* @brief This function returns BMS Fule Gauge algo status* 
* @details
*  This function returns BMS Fule Gauge Batt algo status 
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]pm_fg_batt_peek_mux Battery Algo informaiton
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_get_batt_fg_algo_info(uint32 device_index, pm_fg_batt_peek_mux *batt_fg_algo_sts);


/**
* @brief This function sets BMS Fule Gauge algo status* 
* @details
*  This function sets BMS Fule Gauge Batt algo status 
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]pm_fg_batt_peek_mux Battery Algo information 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_set_batt_fg_algo_info(uint32 device_index, pm_fg_batt_peek_mux batt_fg_algo_sts);


/**
* @brief This function returns BMS Fule Gauge misc cfg* 
* @details
*  This function returns BMS Fule Gauge Batt misc cfg 
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]pm_fg_batt_mis_cfg Battery misc configuration 
* @param[out]enable cfg status/value
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_get_batt_misc_cfg(uint32 device_index, pm_fg_batt_mis_cfg batt_fg_misc_cfg, boolean *enable);


/**
* @brief This function sets BMS Fule Gauge misc cfg* 
* @details
*  This function sets BMS Fule Gauge Batt misc cfg 
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]pm_fg_batt_mis_cfg Battery misc configuration 
* @param[in]enable TRUE/FALSE value to set 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_set_batt_misc_cfg(uint32 device_index, pm_fg_batt_mis_cfg batt_fg_misc_cfg, boolean enable);

#endif /* __PM_FG_BATT_H__ */
