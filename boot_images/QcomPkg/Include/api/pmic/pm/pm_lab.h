#ifndef __PM_LAB_H__
#define __PM_LAB_H__

/*! \file
*  \n
*  \brief  pm_uefi_lab.h PMIC-IBB MODULE RELATED DECLARATION 
*  \details  This header file contains functions and variable declarations 
*  \n &copy;
*  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

                        EDIT HISTORY

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------    ---     ---------------------------------------------------------
10/03/17   ks      Added support for new IBB Type/Subtype
03/05/15   al      Adding API for setting ISENSE and PS_CTL
07/03/14   al      Updating to read VREG_OK
05/20/14   al      Architecture update 
05/29/14   al      Fixed history entry date for first entry  
05/29/14   al      Adding API to configure Vdsp 
04/18/14   al      Updated copyright 
04/18/14   al      Initial version. 
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"


/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

typedef enum
{
  PM_LAB_ISENSE_0P5,   /*0.5 ibb current sense */
  PM_LAB_ISENSE_0P8,   /*0.8 ibb current sense*/
  PM_LAB_ISENSE_0P9,   /*0.9 ibb current sense*/
  PM_LAB_ISENSE_1P0,   /*1.0 ibb current sense*/
  PM_LAB_ISENSE_1P1,   /*1.1 ibb current sense*/
  PM_LAB_ISENSE_1P5,   /*1.5 ibb current sense*/
  PM_LAB_ISENSE_2P0,   /*2.0 ibb current sense*/
  PM_LAB_ISENSE_INVALID,
}pm_lab_isense_type; 

typedef enum
{
    PM_LAB_ISENSE_TAP_0P8,   /*0.8 ibb current sense*/
    PM_LAB_ISENSE_TAP_0P9,   /*0.9 ibb current sense*/
    PM_LAB_ISENSE_TAP_1P0,   /*1.0 ibb current sense*/
    PM_LAB_ISENSE_TAP_1P1,   /*1.1 ibb current sense*/
    PM_LAB_ISENSE_TAP_INVALID,
} pm_lab_isense_tap;

typedef enum
{
    PM_LAB_ISENSE_GAIN_0P5,   /*0.5 ibb current sense */
    PM_LAB_ISENSE_GAIN_1P0,   /*1.0 ibb current sense*/
    PM_LAB_ISENSE_GAIN_1P5,   /*1.5 ibb current sense*/
    PM_LAB_ISENSE_GAIN_2P0,   /*2.0 ibb current sense*/
    PM_LAB_ISENSE_GAIN_INVALID,
} pm_lab_isense_gain;

typedef enum
{
    PM_LAB_LDO_STATUS__DISABLE,
    PM_LAB_LDO_STATUS__ENABLE,
    PM_LAB_LDO_STATUS__NPM,
    PM_LAB_LDO_STATUS__BYPASS,
    PM_LAB_LDO_STATUS__ACT_BYPASS,
    PM_LAB_LDO_STATUS__INVALID
} pm_lab_ldo_status;

typedef struct
{
   boolean en_amoled_mode;        //false is LCD and true is amoled
   boolean lab_module_rdy;        //ready to be enabled
   boolean lab_module_en;         //module enabled or disabled
   boolean lab_ibb_en_rdy;        //allow IBB module to enable lab
   boolean en_fast_precharge;     //allow fast precharge
   uint32  max_prechg_time_usec;  //maximum pre_charge time
   boolean override_output_volt;  //override output voltage 
   uint32  output_volt;           //output voltage set
   boolean lab_vreg_ok;           //LAB VREG OK  
   boolean short_circuit;         //TRUE: no short circuit fault. FALSE: short circuit 
   boolean lab_status_en;         //TRUE: Current LAB status enabled; FALSE: Current LAB status disable
   pm_lab_isense_tap isense_tap;  //current sense tapping
   pm_lab_isense_gain isense_gain; //current sense gain
   uint32 ps_threshold;            //threshold for when pulse-skip mode is entered
   boolean select_ps_table;        //select PS table
   boolean select_digital_ps;      //select digital PS
   boolean enable_ps;              //enable pulse

    /* For LAB 3.0/+ */
    boolean bst_status;
    pm_lab_ldo_status ldo_status;
    /* Lab LDO Status */
    boolean ldo_softstart_done;
    boolean ldo_over_current_detected;
    boolean ldo_vreg_ok;
    /* Lab BOOST Status */
    uint32  bst_vset;
    /* Lab Fault Status */
    boolean fault_shutdown_sts;
}pm_lab_status_type;

typedef struct
{
    boolean sc_error;
    boolean vreg_not_ok;
} pm_lab_rt_status_type;

typedef enum
{
    PM_LAB_SWIRE_CTL__EN_VOUT,
    PM_LAB_SWIRE_CTL__EN_PD,
    PM_LAB_SWIRE_CTL__INVALID
} pm_lab_swire_pgm_ctl;

typedef enum
{
    PM_LAB_SW_HIGH_PSRR_CTL__ENABLE_OVERRIDE,   /* Enable bit, once enabled SW triggers HIGH_PSRR entry/exit request. */
    PM_LAB_SW_HIGH_PSRR_CTL__ENABLE_FORCE,      /* Works only if SW Override Enabled */
    PM_LAB_SW_HIGH_PSRR_CTL__INVALID
} pm_lab_sw_high_psrr_ctl;

typedef enum
{
    PM_LAB_SW_LDO_DBG_MODE__VBIAS_EN,
    PM_LAB_SW_LDO_DBG_MODE__OCP_EN,
    PM_LAB_SW_LDO_DBG_MODE__NPM_EN,
    PM_LAB_SW_LDO_DBG_MODE__BYP_EN,
    PM_LAB_SW_LDO_DBG_MODE__INVALID
} pm_lab_sw_ldo_dbg_mode;

typedef enum
{
    PM_LAB_VPH_VREF_HIGH_PSRR_SEL__350MV,
    PM_LAB_VPH_VREF_HIGH_PSRR_SEL__400MV,
    PM_LAB_VPH_VREF_HIGH_PSRR_SEL__450MV,
    PM_LAB_VPH_VREF_HIGH_PSRR_SEL__500MV,
    PM_LAB_VPH_VREF_HIGH_PSRR_SEL__INVALID
} pm_lab_vph_vref_high_psrr_sel;

typedef enum
{
    PM_LAB_VPH_HW_SRC_SEL__VPH_HIGH__HIGH_PSRR_EN,
    PM_LAB_VPH_HW_SRC_SEL__VPH_HIGH,
    PM_LAB_VPH_HW_SRC_SEL__HIGH_PSRR_EN,
    PM_LAB_VPH_HW_SRC_SEL__INVALID
} pm_lab_vph_hw_high_psrr_src_sel;


/*===========================================================================

                        FUNCTION DELCARATIONS

===========================================================================*/

  /**
 * @brief This function configures module for AMOLED or LCD
 * 
 * @details
 *  This API configures module for AMOLED or LCD
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] en_amoled_mode      TRUE enables for AMOLED and FALSE for LCD 
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lab_lcd_amoled_sel(uint32 device_index, boolean en_amoled_mode);


 /**
 * @brief This function configures module to be enabled or not enabled
 * 
 * @details
 *  This API configures module to be enabled or not enabled
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] lab_rdy             TRUE if ready to be enabled else FALSE
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */

pm_err_flag_type pm_lab_lab_module_rdy(uint32 device_index, boolean lab_rdy);


 /**
 * @brief This function enables or disabled LAB module
 * 
 * @details
 *  This API enables or disabled LAB module
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] en_module           TRUE allows IBB module to enable LAB and FALSE won't
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lab_en_lab_module(uint32 device_index, boolean en_module);


 /**
 * @brief This function sets LAB to be enabled by IBB or not
 * 
 * @details
 *  This API allows LAB to be enabled by IBB or not
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] ibb_en_rdyTRUE      TRUE allows IBB module to enable LAB and FALSE won't 
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lab_ibb_rdy_en(uint32 device_index, boolean ibb_rdy_en);


 /**
 * @brief This function configures precharge mode and sets max precharge time
 * 
 * @details
 *  This API configures precharge mode and sets max precharge time
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] fast_precharge     TRUE configures the fast_precharge and FLASE slow_precharge 
 * @param[in] max_time_usec      Max pre-charge time. Valid range is 200 to 500usec
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lab_config_precharge_ctrl(uint32 device_index, boolean fast_precharge, uint32 max_time_usec);


 /**
 * @brief This reads the LAB module setting status
 * 
 * @details
 *  This API reads the LAB module setting status
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] lab_status          Refer struct #pm_lab_status_type for more info
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lab_get_lab_status(uint32 device_index, pm_lab_status_type* lab_status);

 /**
 * @brief This function allows to select default voltage or set 
 *        output voltage for Vdisp
 *  
 * @details
 *  This API allows to select default voltage or set output 
 *  voltage for +Vdisp
 * 
 * @param[in] pmic_device_index.     Primary: 0 Secondary: 1
 * @param[in] override_output_volt   TRUE sets Output voltage given by SET_OUTPUT_VOLTAGE field otherwise default
 * @param[in] volt_millivolt         Output voltage. Range is 4600 to 6100 milli volt.
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lab_config_output_volt(uint32 device_index, boolean override_output_volt, uint32 volt_millivolt);

/**
 * @brief This configures LAB current sense 
 * 
 * @details
 *  This API configures current sense tapping point and current
 *  sense gain
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] isense_tap          : Current sense tapping point. Refer #pm_lab_isense_type for more info
 * @param[in] isense_gain         : Current sense gain. Refer #pm_lab_isense_type for more info
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lab_config_current_sense(uint32 device_index, pm_lab_isense_type isense_tap, pm_lab_isense_type isense_gain);


 /**
 * @brief This configures LAB pulse skipping parameters  
 * 
 * @details
 *  This API configures pulse skipping parameters
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] ps_threshold      : Threshold when pulse skipping mode is entered. Range is 20 to 50 mAmps
 * @param[in] select_ps_table   : TRUE to slected PS table, otherwise FALSE
 * @param[in] select_digital_ps : TRUE to slected DIGITAL PS, otherwise FALSE for ANALOG
 * @param[in] enable_ps         : TRUE to enable PS, otherwise FALSE to disable it
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lab_config_pulse_skip_ctrl
(uint32 device_index, uint32 ps_threshold, boolean select_ps_table, boolean select_digital_ps, boolean enable_ps);


/* For LAB 3.0/+ Revision */
/**
* @brief This reads the LAB module IRQ RT status
*
* @details
*  This reads the LAB module IRQ RT status
*
* @param[in]  pmic_index        Primary: 0 Secondary: 1
* @param[out] rt_status         Refer struct #pm_lab_rt_status_type for more info
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_lab_get_rt_status(uint8 pmic_index,
    pm_lab_rt_status_type *rt_status
    );

/**
* @brief This API configures LAB S-wire Signal controls.
*
* @details
*  This API configures LAB S-wire Signal controls.
*
* @param[in] pmic_index         Primary: 0 Secondary: 1
* @param[in] swire_ctl          Refer struct #pm_lab_swire_pgm_ctl for more info
* @param[in] enable             TRUE/FALSE
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_lab_swire_ctrl(uint8 pmic_index,
    pm_lab_swire_pgm_ctl swire_ctl,
    boolean enable
    );

/**
* @brief This API configures LAB S-wire Signal controls.
*
* @details
*  This API configures LAB DEFAULT/RUNTIME Output Voltage.
*
* @param[in] pmic_index         Primary: 0 Secondary: 1
* @param[in] op_voltage_mv      4600mV to 6100mV
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_lab_set_default_voltage(uint8 pmic_index,
    uint32 op_voltage_mv
    );

/**
* @brief This API configures LAB Module Current Limit settings.
*
* @details
*  This API configures LAB Module Current Limit settings.
*
* @param[in] pmic_index         Primary: 0 Secondary: 1
* @param[in] en_current_limit   Enable Current Limit config
* @param[in] current_max        Max value for Current Limit
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_lab_current_limit_cfg(uint8 pmic_index,
    boolean en_current_limit,
    uint32 current_max
    );

/**
* @brief This API configures LAB Module's PSRR settings.
*
* @details
*  This API configures LAB Module's PSRR settings.
*
* @param[in] pmic_index         Primary: 0 Secondary: 1
* @param[in] sw_high_psrr_ctl   Refer struct #pm_lab_sw_high_psrr_ctl for more info
* @param[in] enable             TRUE/FALSE
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_lab_sw_high_psrr_ctrl(uint8 pmic_index,
    pm_lab_sw_high_psrr_ctl sw_high_psrr_ctl,
    boolean enable
    );

/**
* @brief This API configures LAB Module debug mode.
*
* @details
*  This API configures LAB Module debug mode.
*
* @param[in] pmic_index         Primary: 0 Secondary: 1
* @param[in] en_sw_ldo_dbg_mode TRUE/FALSE
* @param[in] sw_ldo_dbg_mode    Refer struct #pm_lab_sw_ldo_dbg_mode for more info
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_lab_ldo_sw_dbg_ctrl(uint8 pmic_index,
    boolean en_sw_ldo_dbg_mode,
    pm_lab_sw_ldo_dbg_mode sw_ldo_dbg_mode
    );

/**
* @brief This API configures LAB PullDown enable.
*
* @details
*  This API configures LAB PullDown enable.
*
* @param[in] pmic_index         Primary: 0 Secondary: 1
* @param[in] enable             TRUE/FALSE
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_lab_pd_ctrl(uint8 pmic_index,
    boolean enable
    );

/**
* @brief This API configures LAB Module's supply mode, VPH/HIGH_PSRR.
*
* @details
*  This API configures LAB Module's supply mode, VPH/HIGH_PSRR.
*
* @param[in] pmic_index             Primary: 0 Secondary: 1
* @param[in] vref_high_psrr_sel     Refer struct #pm_lab_vph_vref_high_psrr_sel for more info
* @param[in] hw_high_psrr_src_sel   Refer struct #pm_lab_vph_hw_high_psrr_src_sel for more info
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_lab_vph_envelop_ctrl(uint8 pmic_index,
    pm_lab_vph_vref_high_psrr_sel vref_high_psrr_sel,
    pm_lab_vph_hw_high_psrr_src_sel hw_high_psrr_src_sel
    );


#endif /* __PM_LAB_H__*/
