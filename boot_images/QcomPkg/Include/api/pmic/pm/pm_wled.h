#ifndef __PM_WLED_H__
#define __PM_WLED_H__

/*! \file  pm_uefi_wled.h 
 *  \n
 *  \brief  PMIC-WLED MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support 
 *   the PMIC WLED module.
 *
 *  \n &copy; Copyright (c) 2009-2017 QUALCOMM Technologies Incorporated. All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/19/16   al      Adding APIs for configuring SWIRE default value
03/29/16   al      Updating for newer target
28/05/14   al      Adding APIs for configuring various AMOLED param.
05/20/14   al      Architecture update
05/16/14   al      Add AMOLED, PFM and PSM
03/26/14   al      8994 WLED support
09/17/13   al      Adding WLED IRQ and updating get_status 
01/14/13   al      Replacing enums with type uint32 and removing 'dev_' from api name
12/21/12   al      Updating eumn for SWITCHING FREQUENCY  
10/29/12   al      removed register structs to make it generic and added wled get status api
08/15/12   al	   Changed according to naming conventions
7/27/12    al	   Renamed all Enums,added device index and resource index
03/05/12   sm      Changes parameters for pm_uefi_wled_en_sync
02/08/12   sm      New file
===========================================================================*/

/*===========================================================================
                INCLUDE FILES FOR MODULE
===========================================================================*/
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "com_dtypes.h"

/*===========================================================================
                     GLOBAL TYPE DEFINITIONS
===========================================================================*/

typedef enum
{
  PM_WLED_DISPLAY_TYPE_LCD,
  PM_WLED_DISPLAY_TYPE_AMOLED,
  PM_WLED_DISPLAY_TYPE_INVALID,
}pm_wled_display_type;


typedef enum {
  PM_WLED_NONE = 0,
  PM_WLED_LED1 = 1,
  PM_WLED_LED2 = 2,
  PM_WLED_LED3 = 4,
  PM_WLED_LED4 = 8,
  PM_WLED_ALL = (PM_WLED_LED1 | PM_WLED_LED2 | PM_WLED_LED3 | PM_WLED_LED4),
  PM_WLED_INVALID
} pm_wled_led_type;


typedef enum {
  PM_WLED_WARMUP_DELAY_6P7US,
  PM_WLED_WARMUP_DELAY_13P3US,
  PM_WLED_WARMUP_DELAY_26P7US,
  PM_WLED_WARMUP_DELAY_53P4US,
  PM_WLED_WARMUP_DELAY_INVALID
} pm_wled_warmup_dly_type;

typedef enum {
  PM_WLED_PWM_1,
  PM_WLED_PWM_2,
  PM_WLED_PWM_3,
  PM_WLED_PWM_INVALID
} pm_wled_pwm_type;

typedef enum {
  PM_WLED_MODE_12b_SDM = 0,
  PM_WLED_MODE_9b_PWM,
  PM_WLED_MODE_INVALID
} pm_wled_pwm_mode_type;


typedef enum {
  PM_WLED_BST_MAX_DUTY_SUB_26 = 0, //(1/Fsw - 26ns)*Fsw
  PM_WLED_BST_MAX_DUTY_SUB_52,     //(1/Fsw - 52ns)*Fsw
  PM_WLED_BST_MAX_DUTY_SUB_104,    //(1/Fsw - 104ns)*Fsw
  PM_WLED_BST_MAX_DUTY_SUB_156,    //(1/Fsw - 156ns)*Fsw
  PM_WLED_BST_MAX_DUTY_SUB_INVALID
} pm_wled_bst_max_duty_type;

typedef enum {
  PM_WLED_MODE_CLK_19P2MHz = 0, // 19.2MHz
  PM_WLED_MODE_CLK_4P8MHz,      //  4.8MHz
  PM_WLED_MODE_CLK_2P4MHz,      //  2.4MHz
  PM_WLED_MODE_CLK_1P2MHz,      //  1.2MHz
  PM_WLED_MODE_CLK_INVALID
} pm_wled_mode_clk_type;

typedef enum {
  PM_WLED_MODE_DIM_MODE_ANALOG = 0,
  PM_WLED_MODE_DIM_MODE_DIGITAL,
  PM_WLED_MODE_DIM_MODE_INVALID
} pm_wled_mode_dim_mode_type;

typedef enum {
  PM_WLED_OVP_THR_31P1V=0,       //31.1V
  PM_WLED_OVP_THR_29P6V,         //29.6V
  PM_WLED_OVP_THR_19P6V,         //19.6V
  PM_WLED_OVP_THR_18P1V,         //18.1V
  PM_WLED_OVP_THR_INVALID
} pm_wled_ovp_threshold_type;


typedef enum {
  PM_WLED_BST_ILIMIT_105mA,     //105mA
  PM_WLED_BST_ILIMIT_280mA,     //280mA
  PM_WLED_BST_ILIMIT_450mA,     //450mA
  PM_WLED_BST_ILIMIT_620mA,     //620mA
  PM_WLED_BST_ILIMIT_970mA,     //970mA
  PM_WLED_BST_ILIMIT_1150mA,    //1150mA
  PM_WLED_BST_ILIMIT_1300mA,    //1300mA
  PM_WLED_BST_ILIMIT_1500mA,    //1500mA
  PM_WLED_BST_ILIMIT_INVALID
} pm_wled_bst_ilimit_type;


typedef enum {
  PM_WLED_SLEW_RATE_20nS = 0,  //20 nS
  PM_WLED_SLEW_RATE_15nS,      //15 nS
  PM_WLED_SLEW_RATE_10nS,      //10 nS
  PM_WLED_SLEW_RATE_5nS,       // 5 nS
  PM_WLED_SLEW_RATE_1nS=0,     // 1.0 nS
  PM_WLED_SLEW_RATE_1p5nS,     // 1.5 nS
  PM_WLED_SLEW_RATE_2nS,       // 2.0 nS
  PM_WLED_SLEW_RATE_2p5nS,     // 2.5 nS
  PM_WLED_SLEW_RATE_INVALID
} pm_wled_slew_rate_type;


/* 
Debounce time for short detection happened during normal operation
*/
typedef enum {
  PM_WLED_DBNC_SHORT_TIME_2_BY_FSW = 0, //(2/Fsw)
  PM_WLED_DBNC_SHORT_TIME_4_BY_FSW,     //(4/Fsw)
  PM_WLED_DBNC_SHORT_TIME_8_BY_FSW,     //(8/Fsw)
  PM_WLED_DBNC_SHORT_TIME_16_BY_FSW,    //(16/Fsw)
  PM_WLED_DBNC_SHORT_TIME_INVALID
} pm_wled_dbnc_short_time_type;

typedef struct 
{
  boolean                       short_ckt_alert_to_pbs;  // TRUE :send an alert to PBS; FALSE: use external PFET when short circuit condition is detected
  pm_wled_dbnc_short_time_type  dbnc_time_short_ckt;     //debounce time for short detection
  boolean                       en_discharge_resistance; //connect/disconnect discharge resistance
  boolean                       en_short_ckt;            //enable/disable short protection
}pm_wled_short_ckt_config_type;


typedef enum
{
  PM_WLED_MASK_ERRAMP_EN_MASK = 0, //error amplifier controlled by module enable signals
  PM_WLED_MASK_ILIM_EN_MASK,       //current limit controlled by module enable signals
  PM_WLED_MASK_VREF_EN_MASK,       //reference controlled by module enable signals
  PM_WLED_MASK_CS_REF_EN_MASK,     //reference controlled by module enable signals
  PM_WLED_MASK_SLOPE_COMP_EN_MASK, // slope compensation controlled by module enable signals
  PM_WLED_MASK_OVP_EN_MASK,        // OVP is controlled by module enable signals
  PM_WLED_MASK_T_ERRAMP_OUT,       // enables monitoring error amp output w/ loop filter disconnected
  PM_WLED_MASK_RDSON_TM,           //NFET is turned completely on. Test mode for measuring Rdson of integrated NFET
  PM_WLED_MASK_INVALID,            //Invalid
}pm_wled_mask_type;


typedef enum
{
  PM_WLED_SH_CONFIG_ENB_IIND_UP =0,       //Early turn on the boost NFET when ilim happens
  PM_WLED_SH_CONFIG_EN_SOFT_START,        //Enable soft start Ilim ramp after WLED module enable
  PM_WLED_SH_CONFIG_EN_SLEEP_CLK_REQUEST, //WLED module request for 19.2MHz clock is disabled when sleep_b = 1'b0
  PM_WLED_SH_CONFIG_DEBOUNCE_BYPASS,      //debouncing is removed for Ilim and OVP circuits
  PM_WLED_SH_CONFIG_EN_CLAMP,             //Enable to clamp the error amp output
  PM_WLED_SH_CONFIG_EN_VREF_UP,           //Enable S/H during soft start
  PM_WLED_SH_CONFIG_DEBOUNCE_BYPASS_ILIM, //Enable S/H Debounce for Ilim circuits
  PM_WLED_SH_CONFIG_EN_SH_FOR_MIN_SEL,    //Enable S/H in the minimum selector path
  PM_WLED_SH_CONFIG_INVALID,              //Invalid
}pm_wled_sh_config_type;


typedef enum {
  PM_WLED_IRQ_ILIM_FAULT ,
  PM_WLED_IRQ_OVP_FAULT,
  PM_WLED_IRQ_SC_FAULT,
  PM_WLED_IRQ_INVALID
} pm_wled_irq_type;

typedef enum
{
  PM_WLED_AMOLED_CTRL_TYPE_SWIRE,
  PM_WLED_AMOLED_CTRL_TYPE_SPMI,
  PM_WLED_AMOLED_CTRL_TYPE_INVALID,
}pm_wled_amoled_ctrl_type;


typedef struct
{
   boolean                     led1_modulator_enable;
   boolean                     led2_modulator_enable;
   boolean                     led3_modulator_enable;
   boolean                     led4_modulator_enable;

   boolean                     led1_current_sink_enable;
   boolean                     led2_current_sink_enable;
   boolean                     led3_current_sink_enable;
   boolean                     led4_current_sink_enable;

   uint32                      slope_trim;

   pm_wled_mode_clk_type      mode_clk; 
   boolean                    en_phase_stagger;
   pm_wled_mode_clk_type      accum_clk;
   boolean                    en_hybrid_dim;
   pm_wled_pwm_mode_type      pwm_mode;
   pm_wled_mode_dim_mode_type dim_mode;
   pm_wled_bst_max_duty_type   bst_max_duty;
   uint32                      set_fsw_ctrl;
   uint32                      ramp_time;
   pm_wled_bst_ilimit_type     bst_ilimit;
   pm_wled_slew_rate_type      slew_rate;
   uint32                      vref_ctrl;
   pm_wled_warmup_dly_type     warmup_dly;
   pm_wled_ovp_threshold_type  ovp_thr;
   uint32                      led1_full_scale_current;
   uint32                      led2_full_scale_current;
   uint32                      led3_full_scale_current;
   uint32                      led4_full_scale_current;
   uint32                      led1_brightness;
   uint32                      led2_brightness;
   uint32                      led3_brightness;
   uint32                      led4_brightness;
}pm_wled_status_type;
/*===========================================================================
                          FUNCTION DEFINITIONS
===========================================================================*/

/**
 * @brief This function enables WLED module
 * 
 * @param[in] enable
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_enable_module
(
  uint32 pmic_device_index, uint32 resource_index, boolean enable 
);

/**
 * @brief This function enables digital modulator generator and modulator input
 *        for each current sink.
 * 
 * @param[in] pmic_device_index
 * 
 * @param[in] led_type
 *
 * @param[in] enable
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_enable_modulator
(
  uint32 pmic_device_index, pm_wled_led_type led_type, boolean enable
);

/**
 * @brief This function enables control for LED current sink outputs
 * 
 * @param[in] led_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_enable_current_sink
(
  uint32 pmic_device_index, pm_wled_led_type led_type
);


/**
 * @brief This function LED current sink number 
 * 
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic
 * @param[in]  led_type:
 *				    refer enum pm_wled_led_type
 * @param[in]  current_milliamp:
 *                 current value in milliamp. Valid range is 0 to 25.
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_set_current_sink
(
  uint32 pmic_device_index, 
  pm_wled_led_type led_type, 
  uint32 current_milliamp
);

/**
 * @brief This function enables use of pwm_in_ext as pwm dimming control
 * 
 * @param[in] enable
 * @param[in] led_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_en_ext_pwm
(
 uint32 pmic_device_index, pm_wled_led_type led_type, boolean enable
);

/**
 * @brief This function sets the current sense ratio
 *
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic
 * @param[in]  display:
 *				 display type. Refer #pm_wled_display_type for more info 
 * @param[in] resistance_kohm:
 *                  ISENSE value. Valid range is 3 to 11 for LCD and 20 to 55 for AMOLED
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_set_current_sense_ratio(uint32 pmic_chip, pm_wled_display_type display, uint32 resistance_kohm);

/**
 * @brief This function sets amoled vout
 *
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic
 * @param[in]  vout_ctrl:
 *				 S-wire or SPMI control. Refer #pm_wled_amoled_ctrl_type for more info 
 * @param[in]  avdd_milli_volt:
 *                  slope trim value. Valid range is 5650 to 7900 milli volt
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_set_amoled_vout(uint32 pmic_chip, pm_wled_amoled_ctrl_type  vout_ctrl, uint32 avdd_milli_volt);

/**
 * @brief This function sets 12b duty cycle control for indicated LED digital 
 *        modulator control.
 * 
 * @param[in] led_type
 * @param[in] data
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_set_led_duty_cycle
(
  uint32 pmic_device_index, pm_wled_led_type led_type, uint16 data
);


/**
 * @brief This function enables content adaptive backlight control
 * 
 * @param[in] led_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_en_cabc
(
  uint32 pmic_device_index, pm_wled_led_type led_type
);

/**
 * @brief This function enables LED sync
 * 
 * @param[in] led_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_en_sync
(
 uint32 pmic_device_index,  
 pm_wled_led_type led_type,
  boolean          enable
);


/**
 * @brief This function configures different sink modulation 
 *        properties
 * 
 * @param[in] pmic_device_index 
 * @param[in] mode_clk 
 * @param[in] en_phase_stagger 
 * @param[in] accum_clk 
 * @param[in] en_hybrid_dim 
 * @param[in] pwm_mode 
 * @param[in] dim_mode 
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_config_sink_modulation
(
  uint32 pmic_device_index,  
  pm_wled_mode_clk_type mode_clk, 
  boolean en_phase_stagger, 
  pm_wled_mode_clk_type accum_clk, 
  boolean en_hybrid_dim,
  pm_wled_pwm_mode_type pwm_mode, 
  pm_wled_mode_dim_mode_type dim_mode
);



/**
 * @brief This function selects boost switching frequency control
 * 
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic
 * @param[in]  resource_index:
 *				    valid value 0 
 * @param[in] fsw_khz:
 *                  boost switching frequency in KHz.
 *                  Acceptable values are: 9600, 4800, 3200, 2400, 1920, 1600, 1370, 1200, 1060,  960,  872,  800, 738, 685, 640, 600
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_set_fsw_ctrl
(
  uint32 pmic_device_index, 
  uint32 resource_index,
  uint32   fsw_khz
);


/**
 * @brief This function selects OVP threshold
 * 
 * @param[in] ovp_threshold_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_sel_ovp_thr
(
  uint32 pmic_device_index, 
  uint32 resource_index,
  pm_wled_ovp_threshold_type ovp_threshold_type
);


/**
 * @brief This function selects boost current limit
 * 
 * @param[in] bst_ilimit_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_sel_bst_ilimit
(
  uint32 pmic_device_index, 
  uint32 resource_index,
  pm_wled_bst_ilimit_type bst_ilimit_type
);


/**
 * @brief This function selects slew rate
 * 
 * @param[in] slew_rate_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_sel_slew_rate
(
  uint32 pmic_device_index, 
  pm_wled_slew_rate_type slew_rate_type
);


/**
 * @brief This function sets compensator resistance
 * 
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic
 * @param[in] display_type
 *                  Display type for which this setting is needed. Refer #pm_wled_display_type for more info
 * @param[in] vloop_comp_zero
 *                  control to select the feedback resisto.
 *                  LCD :    Ra = 140k + code * 20k
 *                  AMOLED : Ra = 20k + code * 20k
 * @return  pm_err_flag_type PM_ERR_FLAG_FEATURE_NOT_SUPPORTED 
 *          = Feature not available on this version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_sel_comp_ra
(
  uint32               pmic_chip, 
  pm_wled_display_type display_type, 
  uint32               vloop_comp_zero
);


/**
 * @brief This function enables/disables amoled
 * 
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic; 1 for secondary
 * @param[in] display_type
 *                  AMOLED or LCD display type. Refer #pm_wled_display_type
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_en_amoled
(
   uint32               pmic_device_index,
   pm_wled_display_type display_type
);

/**
 * @brief This function gets the current status for wled:

 * 
 * @param[in] pmic_device_index : Pmic Chip device index.
 *                                For Primary :0
 *      
 *             
 * @param[out] wled_status       :refer pm_wled_status_type for more info  
 *                                pm_wled_led_type set for enabling modulator;
 *                                pm_wled_led_type set for enabling current sink;
 *                                value set for slope trim;
 *                                pm_wled_pwm_mode_type set for pwm mode;
 *                                pm_wled_mod_clk_type set for mod clk;
 *                                pm_wled_mod_dim_mode_type set for mod dim mode;
 *                                pm_wled_bst_max_duty_type set for bst max duty;
 *                                value set for fsw ctrl;
 *                                value set for ramp time;
 *                                pm_wled_bst_ilimit_type set for bst ilimit;
 *                                pm_wled_slew_rate_type set for slew rate;
 *                                pm_wled_vref_ctrl_type set for vref ctrl;
 *                                pm_wled_warmup_dly_type set for warmup dly;
 *                                pm_wled_ovp_threshold_type set for ovp thr; 
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_get_status
(
  uint32 pmic_device_index, 
  pm_wled_status_type *wled_status
);

/**
 * @brief This function enables or disables WLED irq
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                WLED irq type. Refer enum pm_wled_irq_type.
 *  
 * @param[in] enable:  
 *                TRUE: Enable corresponding WLED interrupt 
 *                FALSE:Disable corresponding WLED interrupt
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_irq_enable(uint32 device_index, pm_wled_irq_type irq, boolean enable);


/**
 * @brief This function clears the WLED irq 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                WLED irq type. Refer enum pm_wled_irq_type.  
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_irq_clear(uint32  device_index, pm_wled_irq_type irq);


/**
 * @brief This function configures the WLED irq trigger 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                WLED irq type. Refer enum pm_wled_irq_type.  
 * @param[in] trigger:  
 *                One of different irq triggers. Refer enum pm_irq_trigger_type 
 *                from pm_uefi_irq.h for more details
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_irq_set_trigger(uint32 device_index, pm_wled_irq_type irq, pm_irq_trigger_type trigger);

/**
 * @brief This function reads the WLED irq 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                WLED irq type. Refer enum pm_wled_irq_type. 
 * @param[in] type:  
 *                Type of IRQ status to read. Refer enum pm_irq_status_type 
 *                from pm_uefi_irq.h for more details
 * @param[out] status:  
 *                IRQ status.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_irq_status(uint32 device_index, pm_wled_irq_type irq, pm_irq_status_type type, boolean *status);


 /**
 * @brief This function masks/unmasks various WLED features 
 * 
 * @param[in] device_index : PMIC device index. 0 for primary and 1 for secondary
 * @param[in] mask_type    : Different type of WLED featues mask. Refer #pm_wled_mask_type for more info
 * @param[in] enable       :  TRUE to enable the mask and FALSE to disable
 * 
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 */
// pm_err_flag_type pm_wled_config_wled_mask(uint32 device_index, pm_wled_mask_type mask_type, boolean enable);


 /**
 * @brief This function sets various dtest for WLED
 * 
 * @param[in] device_index   : PMIC device index. 0 for primary and 1 for secondary
 * @param[in] dtest_setting  : Various dtest mapping for WLED. Following are options:
 *                            0 : DTEST1, DTEST2 output disabled
 *                            1 : DTEST1 = Gate drive output, DTEST2 disabled
 *                            2 : DTEST1 = OVP comparator for trim, DTEST2 disabled
 *                            3 : DTEST1 = Ilim comparator (ilim_out), DTEST2 = ilim_no_blnk (no blank of ilim)
 *                            4 : DTEST1 = state[0], DTEST2 = state[1]
 *                            5 : DTEST1 = state[2], DTEST2 = state[3]
 *                            6 : DTEST1 = EN_BST, DTEST2 = EN_DSCHGR
 *                            7 : DTEST1 = errAmp_out for trim, DTEST2 disabled
 *                            8 : DTEST1 = psm_skipb(PSM comparator output), DTEST2 = PFM_ilim_rstb
 *                            9 : DTEST1 = Output short detection (DET_SC), DTEST2 = EXT_FET
 *                            10 : DTEST1 = fsw_clk_in; DTEST2 = Max_duty;
 *                            11 : N/A
 *                            12 : N/A
 *                            13 : DTEST1 = sstart[0], DTEST2 = sstart[1]
 *                            14 : DTEST1 = pwr_stg_en, DTEST2 = SS_DONE
 *                            15 : DTEST1 = cabc_sync, DTEST2 output disabled
 *                            16 : DTEST1 = pwm_out_dig<1>, DTEST2 = pwm_out_ana<1>
 *                            17 : DTEST1 = pwm_out_dig<2>, DTEST2 = pwm_out_ana<2>
 *                            18 : DTEST1 = pwm_out_dig<3>, DTEST2 = pwm_out_ana<3>
 *                            19 : DTEST1 = pwm_out_dig<4>, DTEST2 = pwm_out_ana<4>
 *                            20 : DTEST1 = CS_gatedrv<1>, DTEST2 = CS_gatedrv<2>
 *                            21: DTEST1 = CS_gatedrv<3>, DTEST2 = CS_gatedrv<4>
 *                            22 : DTEST1 = otaout_dft<1>, DTEST2 = otaout_dft<2>
 *                            23: DTEST1 = otaout_dft<3>, DTEST2 = otaout_dft<4>
 *                            24 : DTEST1 = pwm_out_dig<1>, DTEST2 = pwm_hv_gate_pull<1>
 *                            25 : DTEST1 = pwm_out_dig<2>, DTEST2 = pwm_hv_gate_pull<2>
 *                            26 : DTEST1 = pwm_out_dig<3>, DTEST2 = pwm_hv_gate_pull<3>
 *                            27 : DTEST1 = pwm_out_dig<4>, DTEST2 = pwm_hv_gate_pull<4>
 *                            28 : DTEST1: cs_hybrid_dim_cmp_outb <1>, DTEST2: cs_hybrid_dim_cmp_outb <2>
 *                            29 : DTEST1: cs_hybrid_dim_cmp_outb <3>, DTEST2: cs_hybrid_dim_cmp_outb <4>
 * 
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 */
pm_err_flag_type pm_wled_set_wled_dtest(uint32 device_index, uint32 dtest_setting);


/**
 * @brief This function sets swire avdd dormant default volt
 *
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic; 1 for secondary pmic
 * @param[in]  avdd_milli_volt:
 *                  slope trim value. Valid range is 5650 to 7900 milli volt
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_set_dormant_swire_avdd_volt(uint32 pmic_chip, uint32 avdd_milli_volt);

#endif /* __PM_WLED_H__ */

