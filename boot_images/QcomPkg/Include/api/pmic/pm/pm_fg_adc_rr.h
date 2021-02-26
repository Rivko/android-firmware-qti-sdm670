#ifndef __PM_FG_ADC_RR_H__
#define __PM_FG_ADC_RR_H__


/*! \file pm_adc_rr.h 
 *  \n
 *  \brief  PMIC-FG MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support 
 *   the PMIC FG Fule Gauge memory interface module.
 *
 *  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
 *  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/15/17   cs      fixed BATT_ID_CTL_TYPE bit order
04/01/16   aab     Added more ADC RR APIs 
01/12/16   al      Initial Release 
===========================================================================*/

#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================
                        MACRO DEFINITIONS 
===========================================================================*/
#define PM_FG_ADC_RR_FG_CHARGER_SKIN_TEMP_MIN    -30
#define PM_FG_ADC_RR_FG_CHARGER_SKIN_TEMP_MAX    97
#define PM_FG_ADC_RR_FG_FAKE_BATT_MAX    14000
#define PM_FG_ADC_RR_FG_FAKE_BATT_CI     150
#define PM_FG_ADC_RR_FG_FAKE_BATT_FS     2500000

/*===========================================================================
                        TYPE DEFINITIONS 
===========================================================================*/
/* Common Interrupt Prototype */
typedef enum
{
  PM_FG_ADC_RR_IRQ_BT_ID,         /*battery ID conversion data available*/
  PM_FG_ADC_RR_IRQ_RR,            /*fresh round robin data available*/
  PM_FG_ADC_RR_IRQ_RR_BUS_ERR,    /*bus error in transaction*/
  PM_FG_ADC_RR_IRQ_RR_SPARE,      /*spare interrupt*/
  PM_FG_ADC_RR_IRQ_INVALID        /*Invalid adc_rr irq*/
} pm_fg_adc_rr_irq_type;


typedef enum
{
  PM_FG_ADC_RR_REF_VREG_FG_LEVEL_2P70_V,
  PM_FG_ADC_RR_REF_VREG_FG_LEVEL_2P57_V,
  PM_FG_ADC_RR_REF_VREG_FG_LEVEL_2P45_V,
  PM_FG_ADC_RR_REF_VREG_FG_LEVEL_2P35_V,
  PM_FG_ADC_RR_REF_VREG_FG_LEVEL_2P84_V,
  PM_FG_ADC_RR_REF_VREG_FG_LEVEL_3P00_V,
  PM_FG_ADC_RR_REF_VREG_FG_LEVEL_3P18_V,
  PM_FG_ADC_RR_REF_VREG_FG_LEVEL_3P38_V,
  PM_FG_ADC_RR_REF_VREG_FG_LEVEL_INVALID
}pm_fg_adc_rr_ref_vreg_fg_level_type;


typedef enum
{
  PM_FG_ADC_RR_BIAS_WAIT_0_MS,
  PM_FG_ADC_RR_BIAS_WAIT_1_MS,
  PM_FG_ADC_RR_BIAS_WAIT_4_MS,
  PM_FG_ADC_RR_BIAS_WAIT_12_MS,
  PM_FG_ADC_RR_BIAS_WAIT_20_MS,
  PM_FG_ADC_RR_BIAS_WAIT_40_MS,
  PM_FG_ADC_RR_BIAS_WAIT_60_MS,
  PM_FG_ADC_RR_BIAS_WAIT_80_MS,
  PM_FG_ADC_RR_BIAS_WAIT_INVALID,
}pm_fg_adc_rr_bias_wait_type;


typedef enum
{
  PM_FG_ADC_RR_AUX_THERM_CFG_NO_HYST,
  PM_FG_ADC_RR_AUX_THERM_CFG_TMP_HYST_1_C,
  PM_FG_ADC_RR_AUX_THERM_CFG_TMP_HYST_2_C,
  PM_FG_ADC_RR_AUX_THERM_CFG_TMP_HYST_3_C,
  PM_FG_ADC_RR_AUX_THERM_CFG_TMP_HYST_INVALID
}pm_fg_adc_rr_aux_therm_cfg_tmp_hyst_type;


typedef enum
{
  PM_FG_ADC_RR_AUX_THERM_COEF_C1,
  PM_FG_ADC_RR_AUX_THERM_COEF_C2,
  PM_FG_ADC_RR_AUX_THERM_COEF_C3,
  FG_FG_ADC_RR_AUX_THERM_HALF_RANGE,
  PM_FG_ADC_RR_AUX_THERM_COEF_INVALID
}pm_fg_adc_rr_aux_therm_coefficient_type;


typedef enum
{
  PM_FG_ADC_RR_FG_CHARGER_TEMP_HOT,
  PM_FG_ADC_RR_FG_CHARGER_TEMP_TOO_HOT,
  PM_FG_ADC_RR_FG_CHARGER_TEMP_INVALID
}pm_fg_adc_rr_charger_temp_type;


typedef struct
{
   pm_fg_adc_rr_bias_wait_type               aux_therm_cfg_bias_wait;
   pm_fg_adc_rr_aux_therm_cfg_tmp_hyst_type  tmp_hyst;
   boolean                                   batt_therm_sync;
   boolean                                   aux_therm_mode;
   boolean                                   gnd_sel;
}pm_fg_adc_rr_aux_therm_cfg_type;


typedef enum
{
  PM_FG_ADC_RR_BATT_ID_CTRL_CHANNEL_CONV,
  PM_FG_ADC_RR_BATT_ID_CTRL_CHANNEL_IRQ,
  PM_FG_ADC_RR_BATT_ID_CTRL_INVALID
}pm_fg_adc_rr_batt_id_ctrl_type;


/*===========================================================================
                        FUNCTION DEFINITIONS
===========================================================================*/
/**
* @brief This function enables irq
* 
* @details
* This function enables irq
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_adc_rr_irq_type               irq type
* @param[in] boolean        enable/disable value
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_rr_irq_enable(uint32 device_index, pm_fg_adc_rr_irq_type irq, boolean enable);

/**
* @brief This function clears irq
* 
* @details
*  This function clears irq
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_adc_rr_irq_type               irq type
*
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
  pm_err_flag_type pm_fg_adc_rr_irq_clear(uint32  device_index, pm_fg_adc_rr_irq_type irq);
  
/**
* @brief This function sets interrupt triggers
* 
* @details
*  This function sets interrupt triggers
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_adc_rr_irq_type               irq type
* @param[in] pm_irq_trigger_type               trigger
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_rr_irq_set_trigger(uint32 device_index, pm_fg_adc_rr_irq_type irq, pm_irq_trigger_type trigger);

/**
* @brief This function returns irq status
* 
* @details
*  This function returns irq status
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_adc_rr_irq_type        irq type
* @param[in] pm_irq_status_type        irq status type
* @param[in] boolean        irq TRUE/FALSE status
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_rr_irq_status(uint32 device_index, pm_fg_adc_rr_irq_type irq, pm_irq_status_type type, boolean *status);




/**
* @brief This function sets Aux thermister half range 
*        value
* 
* @details
*  This function sets Aux thermister half range value
* 
* @param[in] pmic_device.  Primary: 0 Secondary: 1
* @param[in] temp_value    uint8
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_rr_set_aux_therm_half_range(uint32 pmic_device, uint8 temp_value);



/**
* @brief This function configures FG ADC RR reference level
* 
* @details
*  This function configures FG ADC RR reference level
* 
* @param[in] pmic_device.   : Primary: 0 Secondary: 1
* @param[in] vreg_fg_level  : pm_fg_adc_rr_ref_vreg_fg_level_type
* @param[in] vreg_fg_bypass : boolean
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not
*          available on this ersion of the PMIC.
*          PM_ERR_FLAG__PAR_OUT_OF_RANGE = parameter out of
*          range
*          PM_ERR_FLAG_SUCCESS = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_rr_ref_cfg(uint32 pmic_device, pm_fg_adc_rr_ref_vreg_fg_level_type vreg_fg_level, boolean vreg_fg_bypass);



/**
* @brief This function selects FG ADC RR rbiase cnls buffer
* 
* @details
*  This function selects FG ADC RR rbiase cnls buffer
* 
* @param[in] pmic_device.   : Primary: 0 Secondary: 1
* @param[in] rbias_cnls_buf_sel : boolean
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not
*          available on this ersion of the PMIC.
*          PM_ERR_FLAG__PAR_OUT_OF_RANGE = parameter out of
*          range
*          PM_ERR_FLAG_SUCCESS = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_rr_rbiase_cnls_buf_sel(uint32 pmic_device, boolean rbias_cnls_buf_sel);



/**
* @brief This function configure FG ADC RR battery id
* 
* @details
*  This function configure FG ADC RR battery id
* 
* @param[in] pmic_device   : Primary: 0 Secondary: 1
* @param[in] batt_id_cfg_bias_wait: pm_fg_adc_rr_bias_wait_type

*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not
*          available on this ersion of the PMIC.
*          PM_ERR_FLAG__PAR_OUT_OF_RANGE = parameter out of
*          range
*          PM_ERR_FLAG_SUCCESS = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_rr_batt_id_cfg(uint32 pmic_device, pm_fg_adc_rr_bias_wait_type batt_id_cfg_bias_wait);



/**
* @brief This function configure FG ADC RR battery therm
* 
* @details
*  This function configure FG ADC RR battery therm
* 
* @param[in] pmic_device   : Primary: 0 Secondary: 1
* @param[in] batt_therm_cfg_bias_wait: 
*       pm_fg_adc_rr_bias_wait_type
* @param[in] gnd_sel     : boolean 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not
*          available on this ersion of the PMIC.
*          PM_ERR_FLAG__PAR_OUT_OF_RANGE = parameter out of
*          range
*          PM_ERR_FLAG_SUCCESS = SUCCESS.
*
*/
pm_err_flag_type pm_fg_set_adc_rr_batt_therm_cfg(uint32 pmic_device, pm_fg_adc_rr_bias_wait_type batt_therm_cfg_bias_wait, boolean gnd_sel);



/**
* @brief This function configure FG ADC RR aux therm
* 
* @details
*  This function configure FG ADC RR aux therm
* 
* @param[in] pmic_device            : Primary: 0 Secondary: 1
* @param[in] aux_therm_cfg_bias_wait:pm_fg_adc_rr_bias_wait_type
* @param[in] batt_therm_sync        : boolean 
* @param[in] aux_therm_mode         : boolean 
* @param[in] gnd_sel                : boolean  
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not
*          available on this ersion of the PMIC.
*          PM_ERR_FLAG__PAR_OUT_OF_RANGE = parameter out of
*          range
*          PM_ERR_FLAG_SUCCESS = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_rr_aux_therm_cfg( uint32 pmic_device, pm_fg_adc_rr_aux_therm_cfg_type* aux_therm_cfg);



/**
* @brief 
* This function configure FG ADC RR aux therm coefficent
* 
* @details
*  This function configure FG ADC RR aux therm coefficent
* 
* @param[in] pmic_device   : Primary: 0 Secondary: 1
* @param[in] aux_therm_coef: 
*       pm_fg_adc_rr_aux_therm_coefficient_type
* @param[in] coef_value : uint8 
*                  coef_value is in HALF encoding format 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not
*          available on this ersion of the PMIC.
*          PM_ERR_FLAG__PAR_OUT_OF_RANGE = parameter out of
*          range
*          PM_ERR_FLAG_SUCCESS = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_rr_set_aux_therm_coefficient(uint32 pmic_device, pm_fg_adc_rr_aux_therm_coefficient_type aux_therm_coef, uint8 coef_value);



/**
* @brief 
* This function configure FG ADC RR skin temperature threshold
* 
* @details
*  This function configure FG ADC RR skin temperature threshold
* 
* @param[in] pmic_device   : Primary: 0 Secondary: 1
* @param[in] skin_temp_type: 
*       pm_fg_adc_rr_charger_temp_type
* @param[in] temp : int 
*                  skin temperature threshold  
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not
*          available on this ersion of the PMIC.
*          PM_ERR_FLAG__PAR_OUT_OF_RANGE = parameter out of
*          range
*          PM_ERR_FLAG_SUCCESS = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_rr_set_skin_temp_threshold(uint32 pmic_device, pm_fg_adc_rr_charger_temp_type skin_temp_type, int temp);



/**
* @brief 
* This function do sw sequence initiate
* 
* @details
* Initiate one sequence of the round robin ADC on demand
* 
* @param[in] pmic_device   : Primary: 0 Secondary: 1
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not
*          available on this ersion of the PMIC.
*          PM_ERR_FLAG__PAR_OUT_OF_RANGE = parameter out of
*          range
*          PM_ERR_FLAG_SUCCESS = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_rr_make_sw_req(uint32 pmic_device);



/**
* @brief 
* This function trigger FG ADC RR control register
* 
* @details
*  Trigger FG ADC RR control register after them been modified
* 
* @param[in] pmic_device   : Primary: 0 Secondary: 1
* @param[in] enable : boolean  
*       TRUE: enable   FALSE: disable
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not
*          available on this ersion of the PMIC.
*          PM_ERR_FLAG__PAR_OUT_OF_RANGE = parameter out of
*          range
*          PM_ERR_FLAG_SUCCESS = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_rr_batt_id_trig(uint32 pmic_device, boolean enable);



/**
* @brief 
* This function configure FG ADC RR fake battery id range
* 
* @details
*  This function configure FG ADC RR fake battery id range
* 
* @param[in] pmic_device   : Primary: 0 Secondary: 1
* @param[in] fake_id_min_ohms : uint32  
*       fake battery id lower limit
* @param[in] fake_id_max_ohms : uint32 
*                 fake battery id uper limit
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not
*          available on this ersion of the PMIC.
*          PM_ERR_FLAG__PAR_OUT_OF_RANGE = parameter out of
*          range
*          PM_ERR_FLAG_SUCCESS = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_rr_set_fake_batt_id(uint32 pmic_device, uint32 fake_id_min_ohms, uint32 fake_id_max_ohms);


/**
* @brief 
* This function is used to contorl batt id conversion and irq
* 
* @details
*  Enable/disable interrupt Upon conversion ready
   Enable/disable batt id conversion
* 
* @param[in] pmic_device   : Primary: 0 Secondary: 1
* @param[in] batt_id_ctrl_type: 
*       pm_fg_adc_rr_batt_id_ctrl_type

* @param[in] enable: enable or disable 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not
*          available on this ersion of the PMIC.
*          PM_ERR_FLAG__PAR_OUT_OF_RANGE = parameter out of
*          range
*          PM_ERR_FLAG_SUCCESS = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_rr_batt_id_ctrl(uint32 pmic_device, pm_fg_adc_rr_batt_id_ctrl_type batt_id_ctrl_type, boolean enable);



#endif /* __PM_FG_ADC_RR_H__ */
