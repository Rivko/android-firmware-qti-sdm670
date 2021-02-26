#ifndef __PM_IBB_H__
#define __PM_IBB_H__

/*! \file
*  \n
*  \brief  pm_uefi_ibb.h PMIC-IBB MODULE RELATED DECLARATION 
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
07/16/14   al      Add API to configure delay between LAB and IBB during pwrup and pwrdwn
05/20/14   al      Architecture update 
04/18/14   al      Updated copyright 
09/11/12   al      Initial version. 

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
  PM_IBB_PWRUP_DLY_SRC_BIAS_ACTIVE, /*IBB power stage delay source is after IBB's bias active*/
  PM_IBB_PWRUP_DLY_SRC_LAB_VREG_OK, /*IBB power stage delay source is after VREG_OK from LAB and IBB's bias active 0x0 : BIAS_ACTIVE */
  PM_IBB_PWRUP_DLY_SRC_INVALID      /*Invalid*/
}pm_ibb_pwrup_dly_src_type;

typedef enum
{
  PM_IBB_PWRDN_DLY_SRC_IBB_DISABLE,   /*LAB power-down delay source is after IBB disable */
  PM_IBB_PWRDN_DLY_SRC_IBB_DISCHARGE, /*LAB power-down delay is after IBB's discharged*/
  PM_IBB_PWRDN_DLY_SRC_INVALID        /*Invalid*/
}pm_ibb_pwrdn_dly_src_type;

typedef struct
{
  uint32                    pwrdn_dly_ms;           /*delay between IBB and LAB while powering down. Valid values are 1,2,4,8 ms*/
  pm_ibb_pwrdn_dly_src_type pwrdn_dly_src;          /*refer pm_ibb_pwrdn_dly_src_type for more info*/
  uint32                    pwrup_dly_ms;           /*delay between IBB and LAB while powering up. Valid values are 1,2,4,8 ms*/
  boolean                   en_pwrup_dly;           /*enable disable powerup delay*/
  pm_ibb_pwrup_dly_src_type pwrup_dly_src;          /*refer pm_ibb_pwrup_dly_src_type for more info*/
} pm_ibb_pwrup_pwrdn_dly_type; 

typedef enum
{
    PM_IBB_MODE__IBB_OFF = 0x0,
    PM_IBB_MODE__WAIT_EOR,          /* = 0x1 */
    PM_IBB_MODE__IBB_PWRUP1 = 0x4,
    PM_IBB_MODE__IBB_PWRUP2,
    PM_IBB_MODE__IBB_PWRUP3,
    PM_IBB_MODE__IBB_PWRUP4,
    PM_IBB_MODE__IBB_PWRDN1,        /* = 0x8 */
    PM_IBB_MODE__IBB_PWRDN2,
    PM_IBB_MODE__IBB_PWRDN3,
    PM_IBB_MODE__IBB_PWRDN4,
    PM_IBB_MODE__IBB_PWRDN5,
    PM_IBB_MODE__IBB_PWRDN6,
    PM_IBB_MODE__IBB_ON1,           /* = 0xE */
    PM_IBB_MODE__IBB_ON2,
    PM_IBB_MODE__INVALID
} pm_ibb_mode;

typedef struct
{
  boolean                     en_amoled_mode;        /*false is LCD and true is amoled           */
  boolean                     ibb_module_rdy;        /*ready to be enabled                       */
  boolean                     ibb_module_en;         /*module enabled or disabled                */
  boolean                     swire_rdy;             /*swire_rdy                                 */
  uint32                      chgr_resistor_kohms;   /*charger resistor                          */
  uint32                      swire_pulse_mv;        /*swire output voltage pulse in milli volt  */
  boolean                     override_output_volt;  /*override output voltage                   */
  uint32                      output_volt;           /*output voltage set                        */
  pm_ibb_pwrup_pwrdn_dly_type ibb_dly_config;        /*IBB delay config type                     */
  boolean                     ibb_vreg_ok;           /*IBB VREG OK                               */ 
  boolean                     short_circuit;         /*IBB Module short circuit detect           */
  boolean                     ilimit_error;          /*current limit error                       */
    /* For IBB 3.0/+ */
    boolean                     overload;
    pm_ibb_mode                 ibb_mode_status;
    uint32                      swire_edge_count;
}pm_ibb_status_type;


typedef struct
{
    boolean  fault_shutdown;
    boolean  overload;
    boolean  ilimit_error;
} pm_ibb_rt_status_type;

typedef enum
{
    PM_IBB_PD_CTRL_SWIRE_PD_UPDT__DISABLE,
    PM_IBB_PD_CTRL_SWIRE_PD_UPDT__ENABLE,
    PM_IBB_PD_CTRL_SWIRE_PD_UPDT__INVALID
} pm_ibb_pd_ctrl_swire_pd_updt;

typedef enum
{
    PM_IBB_PD_CTRL_PD_STRENGHT__FULL,
    PM_IBB_PD_CTRL_PD_STRENGHT__HALF,
    PM_IBB_PD_CTRL_PD_STRENGHT__INVALID
} pm_ibb_pd_ctrl_pd_strenght;

typedef struct
{
    boolean  enable_pd;
    pm_ibb_pd_ctrl_swire_pd_updt  swire_pd_updt;
    pm_ibb_pd_ctrl_pd_strenght  pd_strenght;
} pm_ibb_pd_ctrl_type;

typedef enum
{
    PM_IBB_ILIMIT_DEB_PERCENTAGE__1b8_LOW,  /* 1/8 low glitch allowed */
    PM_IBB_ILIMIT_DEB_PERCENTAGE__3b8_LOW,  /* 3/8 low glitch allowed */
    PM_IBB_ILIMIT_DEB_PERCENTAGE__4b8_LOW,  /* 4/8 low glitch allowed */
    PM_IBB_ILIMIT_DEB_PERCENTAGE__5b8_LOW,  /* 5/8 low glitch allowed */
    PM_IBB_ILIMIT_DEB_PERCENTAGE__INVALID
} pm_ibb_ilimit_deb_percentage;

typedef enum
{
    PM_IBB_ILIMIT_CURRENT_STEP_SIZE__50mA,
    PM_IBB_ILIMIT_CURRENT_STEP_SIZE__100mA,
    PM_IBB_ILIMIT_CURRENT_STEP_SIZE__150mA,
    PM_IBB_ILIMIT_CURRENT_STEP_SIZE__200mA,
    PM_IBB_ILIMIT_CURRENT_STEP_SIZE__INVALID
} pm_ibb_ilimit_current_step_size;

typedef enum
{
    PM_IBB_IPLIMIT_STEPPER_CYCLES__8,
    PM_IBB_IPLIMIT_STEPPER_CYCLES__16,
    PM_IBB_IPLIMIT_STEPPER_CYCLES__32,
    PM_IBB_IPLIMIT_STEPPER_CYCLES__64,
    PM_IBB_IPLIMIT_STEPPER_CYCLES__INVALID
} pm_ibb_iplimit_stepper_cycles;    /* no. cycles per step */

typedef struct
{
    boolean  en_iplimit_stepper;
    pm_ibb_ilimit_deb_percentage  deb_percentage;
    uint32  startup_iplimit;
    pm_ibb_ilimit_current_step_size  current_step_size;
    pm_ibb_iplimit_stepper_cycles  iplimit_stepper_cycles;
} pm_ibb_iplimit_startup_ctrl_type;

typedef enum
{
    PM_IBB_IPLIMIT_COUNT__8,
    PM_IBB_IPLIMIT_COUNT__16,
    PM_IBB_IPLIMIT_COUNT__32,
    PM_IBB_IPLIMIT_COUNT__64,
    PM_IBB_IPLIMIT_COUNT__INVALID
} pm_ibb_iplimit_count;

typedef struct
{
    boolean  en_current_limit;
    pm_ibb_iplimit_count  ilimit_count;
    uint32  current_max;
} pm_ibb_current_limit_cfg_type;

typedef enum
{
    PM_IBB_PS_THRESHOLD__0uA,
    PM_IBB_PS_THRESHOLD__0P5uA,
    PM_IBB_PS_THRESHOLD__1uA,
    PM_IBB_PS_THRESHOLD__1P5uA,
    PM_IBB_PS_THRESHOLD__2uA,
    PM_IBB_PS_THRESHOLD__2P5uA,
    PM_IBB_PS_THRESHOLD__3uA,
    PM_IBB_PS_THRESHOLD__3P5uA,
    PM_IBB_PS_THRESHOLD__INVALID
} pm_ibb_ps_threshold;

typedef struct
{
    boolean  en_ps;
    pm_ibb_ps_threshold  ps_threshold;
} pm_ibb_ps_ctrl_type;

typedef enum
{
    PM_IBB_VREG_OK_OVERLOAD_DEB__1mS,
    PM_IBB_VREG_OK_OVERLOAD_DEB__2mS,
    PM_IBB_VREG_OK_OVERLOAD_DEB__4mS,
    PM_IBB_VREG_OK_OVERLOAD_DEB__8mS,
    PM_IBB_VREG_OK_OVERLOAD_DEB__INVALID
} pm_ibb_vreg_ok_overload_deb;

typedef enum
{
    PM_IBB_VREG_OK_VOK_DEB__4uS,
    PM_IBB_VREG_OK_VOK_DEB__8uS,
    PM_IBB_VREG_OK_VOK_DEB__16uS,
    PM_IBB_VREG_OK_VOK_DEB__32uS,
    PM_IBB_VREG_OK_VOK_DEB__INVALID
} pm_ibb_vreg_ok_vok_deb;

typedef struct
{
    boolean  en_overload_blank;
    pm_ibb_vreg_ok_overload_deb  overload_deb;
    pm_ibb_vreg_ok_vok_deb  vok_deb;
} pm_ibb_vreg_ok_ctrl_type;

typedef enum
{
    PM_IBB_SLEW_CTRL_SLEW_RATE_SPEED__SLOW,
    PM_IBB_SLEW_CTRL_SLEW_RATE_SPEED__FAST,
    PM_IBB_SLEW_CTRL_SLEW_RATE_SPEED__INVALID
} pm_ibb_slew_ctrl_slew_rate_speed;

typedef enum
{
    PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__0p1mS,
    PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__0p2mS,
    PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__0p5mS,
    PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__1mS,
    PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__2mS,
    PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__10mS,
    PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__12mS,
    PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__15mS,
    PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__INVALID
} pm_ibb_slew_ctrl_trans_time_fast;

typedef enum
{
    PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__0p1mS,
    PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__0p2mS,
    PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__0p5mS,
    PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__1mS,
    PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__2mS,
    PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__10mS,
    PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__12mS,
    PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__15mS,
    PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__INVALID
} pm_ibb_slew_ctrl_trans_time_slow;

typedef struct
{
    boolean  en_digital_slew_ctl;
    pm_ibb_slew_ctrl_slew_rate_speed  slew_rate_speed;
    pm_ibb_slew_ctrl_trans_time_fast  trans_time_fast;
    pm_ibb_slew_ctrl_trans_time_slow  trans_time_slow;
} pm_ibb_slew_ctrl_type;


/*===========================================================================

                        FUNCTION DECLARATION

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
pm_err_flag_type pm_ibb_lcd_amoled_sel(uint32 device_index, boolean en_amoled_mode);


 /**
 * @brief This function prepares module to be enabled or not
 * 
 * @details
 *  This API prepares module to be enabled or not.
 *  When this is set to low, the IBB module cannot be enabled 
 *  (powered-up) through S-Wire or IBB_ENABLE register
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] ibb_rdy             TRUE if ready to be enabled else FALSE
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_ibb_ibb_module_rdy(uint32 device_index, boolean ibb_rdy);


 /**
 * @brief This function enables/disables module and makes S-wire ready for listening
 * 
 * @details
 *  This API enables/disables module and makes S-wire ready for listening
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] en_module      TRUE enables module and false disables module
 * @param[in] swire_rdy      TRUE ready to liste to to S-wire and FALSE not ready to listen to S-Wire
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_ibb_config_ibb_ctrl(uint32 device_index, boolean en_module, boolean swire_rdy);



 /**
 * @brief This function sets the charging resistance for soft start
 * 
 * @details
 *  This API sets the charging resistance for soft start.  
 *  Time constant is ref_cap*charging resistor.
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] resistor_kohms     Charging resistor value. Values are 16 to 300 KOhms
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_ibb_set_soft_strt_chgr_resistor(uint32 device_index, uint32 resistor_kohms);



 /**
 * @brief This function sets startup voltage when there is only
 *        one s-wire voltage programming pulse
 * 
 * @details
 *  This API sets startup voltage when there is only one s-wire 
 *        voltage programming pulse
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] volt_millivolt      Valid value range is 1400 to 7700 milli volt
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_ibb_set_swire_output_pulse(uint32 device_index, uint32 volt_millivolt);


 /**
 * @brief This function allows to select default voltage and set 
 *        output voltage
 *  
 * @details
 *  This API allows to select default voltage and set output 
 *  voltage
 * 
 * @param[in] pmic_device_index.     Primary: 0 Secondary: 1
 * @param[in] override_output_volt   TRUE sets Output voltage given by SET_OUTPUT_VOLTAGE field otherwise default
 * @param[in] volt_millivolt         Output voltage. Range is 1400 to 7700 milli volt.
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_ibb_config_output_volt(uint32 device_index, boolean override_output_volt, uint32 volt_millivolt);


 /**
 * @brief This configures the delay for IBB module 
 * 
 * @details
 *  This API configures delay required between IBB and LAB vregs and also delay source
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] ibb_status          Refer struct #pm_ibb_pwrup_pwrdn_dly_type for more info
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_ibb_config_pwrup_pwrdn_dly(uint32 device_index, pm_ibb_pwrup_pwrdn_dly_type *delay_config);

 /**
 * @brief This reads the IBB module setting status
 * 
 * @details
 *  This API reads the IBB module setting status
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] ibb_status          Refer struct #pm_ibb_status_type for more info
 *       
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
 pm_err_flag_type pm_ibb_get_ibb_status(uint32 device_index, pm_ibb_status_type* ibb_status);


/* For IBB 3.0/+ Revision */
/**
* @brief This reads the IBB module real-time status
*
* @details
*  This API reads the IBB module real-time status
*
* @param[in] pmic_index         Primary: 0 Secondary: 1
* @param[out] rt_status         Refer struct #pm_ibb_rt_status_type for more info
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_ibb_get_rt_status(uint8 pmic_index,
    pm_ibb_rt_status_type *rt_status
    );

/**
* @brief This configures the IBB Output voltage
*
* @details
*  This API configures the IBB Output (-ve) voltage. Caller just needs to pass
*  amplitude voltage (in mV).
*
* @param[in] pmic_index         Primary: 0 Secondary: 1
* @param[in] op_voltage_mv      Output voltage - 400mV to 6300mV
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_ibb_set_default_voltage(uint8 pmic_index,
    uint32 op_voltage_mv
    );

/**
* @brief This configures IBB module Pulldown controls
*
* @details
*  This API configures IBB module Pulldown controls
*
* @param[in] pmic_index         Primary: 0 Secondary: 1
* @param[in] pd_ctrl            Refer struct #pm_ibb_pd_ctrl_type for more info
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_ibb_pd_ctrl(uint8 pmic_index,
    pm_ibb_pd_ctrl_type *pd_ctrl
    );

/**
* @brief This configures IBB module Peak Current startup controls
*
* @details
*  This API configures IBB module Peak Current startup controls
*
* @param[in] pmic_index         Primary: 0 Secondary: 1
* @param[in] iplimit_ctrl       Refer struct #pm_ibb_iplimit_startup_ctrl_type for more info
*                               ->startup_iplimit = 175mA to 925mA
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_ibb_iplimit_startup_ctrl(uint8 pmic_index,
    pm_ibb_iplimit_startup_ctrl_type *iplimit_ctrl
    );

/**
* @brief This configures IBB module status current limit
*
* @details
*  This API configures IBB module status current limit
*
* @param[in] pmic_index         Primary: 0 Secondary: 1
* @param[in] current_limit_cfg  Refer struct #pm_ibb_current_limit_cfg_type for more info
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_ibb_current_limit_cfg(uint8 pmic_index,
    pm_ibb_current_limit_cfg_type *current_limit_cfg
    );

/**
* @brief This configures IBB module Pulse skip controls
*
* @details
*  This API configures IBB module Pulse skip controls
*
* @param[in] pmic_index         Primary: 0 Secondary: 1
* @param[in] en_ps              TRUE/FALSE
* @param[in] ps_threshold       Refer struct #pm_ibb_ps_threshold for more info
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_ibb_ps_ctrl(uint8 pmic_index,
    boolean en_ps,
    pm_ibb_ps_threshold ps_threshold
    );

/**
* @brief This configures IBB module Vreg_OK controls
*
* @details
*  This API configures IBB module Vreg_OK controls
*
* @param[in] pmic_index         Primary: 0 Secondary: 1
* @param[in] vreg_ok_ctrl       Refer struct #pm_ibb_vreg_ok_ctrl_type for more info
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_ibb_vreg_ok_ctrl(uint8 pmic_index,
    pm_ibb_vreg_ok_ctrl_type *vreg_ok_ctrl
    );

/**
* @brief This configures IBB module S-wire controls
*
* @details
*  This API configures IBB module S-wire controls
*
* @param[in] pmic_index         Primary: 0 Secondary: 1
* @param[in] en_swire_vout_updt TRUE/FALSE
* @param[in] op_voltage_at_one_edge  Voltage level: 400mV to 6300mV
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_ibb_swire_ctrl(uint8 pmic_index,
    boolean en_swire_vout_updt,
    uint32 op_voltage_at_one_edge
    );

/**
* @brief This configures the IBB module Slew control setting
*
* @details
*  This configures the IBB module Slew control setting
*
* @param[in] pmic_index.        Primary: 0 Secondary: 1
* @param[in] status             Refer struct #pm_ibb_status_type for more info
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_ibb_slew_ctrl(uint8 pmic_index,
    pm_ibb_slew_ctrl_type *slew_ctrl
    );


#endif /* __PM_IBB_H__*/
