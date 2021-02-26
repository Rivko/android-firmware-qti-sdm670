#ifndef __PM_SCHG_USB_H__
#define __PM_SCHG_USB_H__

/*! \file
*  \n
*  \brief  pm_schg_usb.h PMIC-SMBC MODULE RELATED DECLARATION 
*  \details  This header file contains functions and variable declarations 
*  to support Qualcomm PMIC SCHG (Switch-Mode Battery Charger) module. The 
*  Switched-Mode Battery Charger (SCHG) module includes a buck regulated 
*  battery charger with integrated switches. The SCHG module, along with the 
*  Over Voltage Proection (OVP) module will majorly be used by charger 
*  appliation for charging Li-Ion batteries with high current (up to 2A).
*  \n &copy; Copyright 2016-2017 QUALCOMM Technologies Inc, All Rights Reserved
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who    what, where, why
--------   ---    ---------------------------------------------------------- 
10/12/17   mr     Updated _get_typec_status API to get USB Mode and uUSB status
02/03/17   sm     Added API to configure try sink mode
01/12/17   sm     Added API to returns USB connector configuration
06/02/16   sm     Added ccout_out_hiz_sts in struct pm_schg_usb_typec_status_type
05/25/16   sm     Added pm_schg_usb_enable_icl_override_after_apsd()
04/04/16   sm     Added pm_schg_usb_aicl_options_cfg()
03/28/16   sm     Added VCONN and type C related API support
03/03/16   al     Adding API to read typeC status 
03/01/16   sm     Added pm_schg_usb_set_cmd_il()
02/16/16   va     Adding required API for enabling charger
01/24/16   al     Initial version. 
========================================================================== */
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/


/*! \enum pm_schg_usb_irq_bit_field_type
   \brief different types of irq bit fields of by smbb irq module
 */
typedef enum
{
  PM_SCHG_USB_IRQ_USBIN_COLLAPSE,                 /*The USBIN voltage has crossed the collapse threshold*/
  PM_SCHG_USB_IRQ_USBIN_VASHDN,                   /*The USBIN voltage has crossed the auto-shutdown threshold*/
  PM_SCHG_USB_IRQ_USBIN_UV,                       /*The USBIN voltage has crossed the under-voltage threshold*/
  PM_SCHG_USB_IRQ_USBIN_OV,                       /*The USBIN voltage has crossed the over-voltage threshold*/
  PM_SCHG_USB_IRQ_USBIN_PLUGIN,                   /*USBIN plugged in, goes low on un-plug*/
  PM_SCHG_USB_IRQ_USBIN_SOURCE_CHANGE,            /*Change to APSD status, HVDCP status, HVDCP authentication status, HVDCP Change Complete (vadp_change_done)*/
  PM_SCHG_USB_IRQ_USBIN_ICL_CHANGE,               /*Change in USBIN input current limit*/
  PM_SCHG_USB_IRQ_TYPE_C_OR_RID_DETECTION_CHANGE, /*Generated when state of CC pin changes (for type-C and RID mode)*/
  PM_SCHG_USB_IRQ_INVALID,                        /*INVALID*/
}pm_schg_usb_irq_type; 


typedef enum 
{
  PM_SCHG_USB_USBIN_ADAPTER_ALLOW_5V,
  PM_SCHG_USB_USBIN_ADAPTER_ALLOW_RESERVED,
  PM_SCHG_USB_USBIN_ADAPTER_ALLOW_9V,
  PM_SCHG_USB_USBIN_ADAPTER_ALLOW_5V_OR_9V,
  PM_SCHG_USB_USBIN_ADAPTER_ALLOW_12V,
  PM_SCHG_USB_USBIN_ADAPTER_ALLOW_5V_OR_12V,
  PM_SCHG_USB_USBIN_ADAPTER_ALLOW_9V_TO_12V,
  PM_SCHG_USB_USBIN_ADAPTER_ALLOW_5V_OR_9V_TO_12V,
  PM_SCHG_USB_USBIN_ADAPTER_ALLOW_5V_TO_9V,
  PM_SCHG_USB_USBIN_ADAPTER_ALLOW_5V_TO_12V,
  PM_SCHG_USB_USBIN_ADAPTER_ALLOW_INVALID,
}pm_schg_usb_usbin_adapter_allowance_type;

typedef enum
{
 PM_SCHG_USB_APSD_STATUS_APSD_DTC_STATUS_DONE,
  /*APSD Detection Done*/
 PM_SCHG_USB_APSD_STATUS_QC_CHARGER,
  /*Quick Charge Detection Done*/
 PM_SCHG_USB_APSD_STATUS_QC_AUTH_DONE_STATUS,
  /*QC Authentication Algorithm Done*/
 PM_SCHG_USB_APSD_STATUS_VADP_CHANGE_DONE_AFTER_AUTH,
  /*Adaptive Voltage Change Done after Authentication*/
 PM_SCHG_USB_APSD_STATUS_ENUMERATION_DONE,
  /*SDP Enumeration Done*/
 PM_SCHG_USB_APSD_STATUS_SLOW_PLUGIN_TIMEOUT,
  /*Slow Plugin Timeout Expired*/
 PM_SCHG_USB_APSD_STATUS_INVALID,
  /*invalid*/
}pm_schg_usb_apsd_status_type;


typedef enum
{
  PM_SCHG_USB_APSD_RESULT_STATUS_SDP_CHARGER,
  PM_SCHG_USB_APSD_RESULT_STATUS_OCP_CHARGER,
  PM_SCHG_USB_APSD_RESULT_STATUS_CDP_CHARGER,
  PM_SCHG_USB_APSD_RESULT_STATUS_DCP_CHARGER,
  PM_SCHG_USB_APSD_RESULT_STATUS_FLOAT_CHARGER,
  PM_SCHG_USB_APSD_RESULT_STATUS_QC_2P0,
  PM_SCHG_USB_APSD_RESULT_STATUS_QC_3P0,
  PM_SCHG_USB_APSD_RESULT_STATUS_ICL_OVERRIDE_LATCH,
  PM_SCHG_USB_APSD_RESULT_STATUS_INVALID,
}pm_schg_usb_apsd_result_status_type;

typedef enum
{
  PM_SCHG_USB_ICL_OPTIONS_USBIN_MODE_CHG, /**< 0x0 : USB_100_OR_500_MODE 0x1 : USB_HIGH_CURRENT_MODE >*/
  PM_SCHG_USB_ICL_OPTIONS_USB51_MODE,     /**< 0x0 : USB_100_MODE 0x1 : USB_500_MODE                 >*/
  PM_SCHG_USB_ICL_OPTIONS_CFG_USB3P0_SEL, /**< 0x0 : USB_2P0_SEL  0x1 : USB_3P0_SEL                 >*/
  PM_SCHG_USB_ICL_OPTIONS_INVALID
}pm_schg_usb_icl_options_type;

typedef enum
{
  PM_SCHG_USB_APSD_RERUN,
  PM_SCHG_USB_ICL_OVERRIDE,
  PM_SCHG_USB_CMD_APSD_INVALID,
}pm_schg_usb_cmd_apsd;

typedef enum
{
  PM_SCHG_USB_HVDCP2_SINGLE_INCREMENT,
  PM_SCHG_USB_HVDCP2_SINGLE_DECREMENT,
  PM_SCHG_USB_HVDCP2_IDLE,
  PM_SCHG_USB_HVDCP2_FORCE_5V,
  PM_SCHG_USB_HVDCP2_FORCE_9V,
  PM_SCHG_USB_HVDCP2_FORCE_12V,
  PM_SCHG_USB_HVDCP2_TRIGGER_AICL,
  PM_SCHG_USB_HVDCP2_RESTART_AICL,
  PM_SCHG_USB_HVDCP2_CMD_INVALID,
}pm_schg_usb_cmd_hvdcp2;


typedef enum
{
  PM_SCHG_USB_VADP_TAPER_TIMER_EN,
  PM_SCHG_USB_VADP_INCREMENT_VOLTAGE_LIMIT,
  PM_SCHG_USB_HVDCP_EN,
  PM_SCHG_USB_AUTO_SRC_DETECT,
  PM_SCHG_USB_INPUT_PRIORITY,
  PM_SCHG_USB_HVDCP_AUTONOMOUS_MODE_EN_CFG,
  PM_SCHG_USB_HVDCP_AUTH_ALG_EN_CFG,
  PM_SCHG_USB_CABLE_R_SEL,
  PM_SCHG_USB_USBIN_OPTION1_CFG_INVALID,
}pm_schg_usb_usbin_options_1_cfg;

typedef enum
{
  PM_SCHG_USB_CMD_IL_TYPE_USBIN_SUSPEND,     /*USBIN Suspend: 0 = normal 1 = suspend*/
  PM_SCHG_USB_CMD_IL_TYPE_BAT_2_SYS_FET_DIS, /*Battery-to-System FET Control: 0 = Automatic 1 = FET off (in all modes)*/
  PM_SCHG_USB_CMD_IL_TYPE_INVALID           /*Invalid*/
}pm_schg_usb_cmd_il_type; 

/*
 * For detecting the current limit source can provide.
 */
typedef enum
{
  PM_SCHG_USB_TYPEC_CURR_ADV_RESERVED,
  PM_SCHG_USB_TYPEC_CURR_ADV_3A,
  /*Current advertisement 3.0A*/
  PM_SCHG_USB_TYPEC_CURR_ADV_1P5A,
  /*Current advertisement 1.5A*/
  PM_SCHG_USB_TYPEC_CURR_ADV_STD,
  /*Current advertisement STD TypeC USB Power*/
  PM_SCHG_USB_TYPEC_CURR_ADV_INVALID,
}pm_schg_usb_typec_curr_adv_type;

typedef enum
{
    PM_TYPEC_PLUG_RESERVED,
    PM_TYPEC_PLUG_UNFLIP,      // CC1 is connected
    PM_TYPEC_PLUG_FLIP,        // CC2 is connected
    PM_TYPEC_PLUG_OPEN,
    PM_TYPEC_PLUG_INVALID
} pm_schg_usb_typec_plug_orientation;

typedef enum
{
 PM_SCHG_USB_AICL_CFG_LV_COLLAPSE_RESPONSE,
 PM_SCHG_USB_AICL_CFG_HV_COLLAPSE_RESPONSE,
 PM_SCHG_USB_AICL_CFG_AICL_EN,
 PM_SCHG_USB_AICL_CFG_ADC_EN,
 PM_SCHG_USB_AICL_CFG_RERUN_EN,
 PM_SCHG_USB_AICL_CFG_AICL_START_AT_MAX,
 PM_SCHG_USB_AICL_CFG_HDC_EN,
 PM_SCHG_USB_AICL_CFG_SUSPEND_ON_COLLAPSE_USBIN,
 PM_SCHG_USB_AICL_CFG_INVALID
}pm_schg_usb_aicl_cfg_type;

/* For the types that different source (UFP) attached (we are in DFP).
 */
typedef enum
{
    PM_SCHG_TYPEC_UFP_RESERVED,
    PM_SCHG_TYPEC_UFP_UNATTACHED,                  // OPEN OPEN
    PM_SCHG_TYPEC_UFP_ATTACHED,                    // RD OPEN
    PM_SCHG_TYPEC_UFP_NOT_ATTACHED_POWER_CABLE,    // RA OPEN
    PM_SCHG_TYPEC_UFP_ATTACHED_POWER_CABLE,        // RD RA
    PM_SCHG_TYPEC_UFP_AUDIO_ADAPTER,               // RA RA
    PM_SCHG_TYPEC_UFP_DEBUG_ACCESSORY,             // RD RD
    PM_SCHG_TYPEC_UFP_INVALID
} pm_schg_usb_typec_ufp_type;


/* For uUSB Connection types.
 */
typedef enum
{
    PM_SCHG_USB_MICROUSB_MODE_GROUND_VBUS,      /* Micro USB mode - Ground on CC1 when VBUS is present */
    PM_SCHG_USB_MICROUSB_MODE_FLOAT_NOVBUS,     /* Micro USB mode - Float on CC1 when VBUS is not present */
    PM_SCHG_USB_MICROUSB_MODE_GROUND_NOVBUS,    /* Micro USB mode - Ground on CC1 when VBUS is not present */
    PM_SCHG_USB_MICROUSB_MODE_INVALID
} pm_schg_usb_microusb_mode;

/** USB Connector type **/
typedef enum
{
    PM_SCHG_USB_CONNECTOR_TYPE_TYPEC,
    PM_SCHG_USB_CONNECTOR_TYPE_uUSB,
    PM_SCHG_USB_CONNECTOR_TYPE_INVALID
} pm_schg_usb_connector_type;

/**  TYPEC Port status on attach of USB cable **/
typedef struct
{
    pm_schg_usb_typec_plug_orientation     cc_out_sts;
    pm_schg_usb_typec_curr_adv_type        dfp_curr_adv;
    pm_schg_usb_typec_ufp_type             ufp_conn_type;
    boolean                                vbus_sts;
    boolean                                vbus_err_sts;
    boolean                                debounce_done_sts;
    boolean                                vconn_oc_sts;
    boolean                                ccout_out_hiz_sts;
    pm_schg_usb_connector_type             usb_connector;
    pm_schg_usb_microusb_mode              u_usb_mode;
} pm_schg_usb_typec_status_type;

typedef enum
{
    PM_SCHG_USB_TYPEC_ROLE_NONE,
    PM_SCHG_USB_TYPEC_ROLE_DFP,
    PM_SCHG_USB_TYPEC_ROLE_UFP,
    PM_SCHG_USB_TYPEC_ROLE_INVALID
} pm_schg_usb_typec_port_role_type;

typedef enum
{
    PM_SCHG_USB_TYPEC_VCONN_CONTROL_BY_HW,
    PM_SCHG_USB_TYPEC_VCONN_DISABLE_BY_SW = 0x10,
    PM_SCHG_USB_TYPEC_VCONN_ENABLE_BY_SW,
    PM_SCHG_USB_TYPEC_VCONN_INVALID
} pm_schg_usb_typec_vconn_enable_type;

/**  TYPEC VConn Current Limit **/
typedef enum
{
    PM_TYPEC_VCONN_ILIMIT_300MA,
    PM_TYPEC_VCONN_ILIMIT_500MA,
    PM_TYPEC_VCONN_ILIMIT_INVALID
} pm_schg_usb_typec_vconn_ilimit;

/**  TYPEC VConn Soft Start Pull Down Current mA - SS_PD_IMA **/
typedef enum
{
    PM_TYPEC_VCONN_SS_PD_35MA,
    PM_TYPEC_VCONN_SS_PD_88MA,
    PM_TYPEC_VCONN_SS_PD_160MA,
    PM_TYPEC_VCONN_SS_PD_248MA,
    PM_TYPEC_VCONN_SS_PD_INVALID
} pm_schg_usb_typec_vconn_ss_pd_ima;



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
* @param[in] irq                 pm_schg_usb_irq_type
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_irq_enable(uint32 device_index, pm_schg_usb_irq_type irq, boolean enable);


/**
 * @brief This function clears the SCHG irq 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                SCHG irq type. Refer enum pm_schg_usb_irq_type.  
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_usb_irq_clear(uint32  device_index, pm_schg_usb_irq_type irq);



/**
 * @brief This function configures the SCHG irq trigger 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                SCHG irq type. Refer enum pm_schg_usb_irq_type.  
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
pm_err_flag_type pm_schg_usb_irq_set_trigger(uint32 device_index, pm_schg_usb_irq_type irq, pm_irq_trigger_type trigger);


/**
 * @brief This function configures the SCHG for irq 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                SCHG irq type. Refer enum pm_Smbb_irq_type. 
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
pm_err_flag_type pm_schg_usb_irq_status(uint32 device_index, pm_schg_usb_irq_type irq, pm_irq_status_type type, boolean *status);

/**
* @brief This function sets the USBIN adapter allowance
* 
* @details
*  This function sets the USBIN adapter allowance
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] adptr_allowance     Refer 
*       pm_schg_usb_usbin_adapter_allowance_type for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_set_usbin_adptr_allowance(uint32 device_index, pm_schg_usb_usbin_adapter_allowance_type adptr_allowance);

/**
* @brief This function reads the USBIN adapter allowance
* 
* @details
*  This function reads the USBIN adapter allowance
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] adptr_allowance     Refer 
*       pm_schg_usb_usbin_adapter_allowance_type for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_get_usbin_adptr_allowance(uint32 device_index, pm_schg_usb_usbin_adapter_allowance_type *adptr_allowance);

/**
* @brief This function returns the APSD status
* 
* @details
*  This function reads the USBIN adapter allowance
* 
* @param[in]  device_index.  Primary: 0 Secondary: 1
* @param[in]  apsd_status          Refer #pm_schg_usb_apsd_status_type
* @param[out] result          returns low or high
*                                
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_get_apsd_status(uint32 device_index, pm_schg_usb_apsd_status_type apsd_status, boolean *result);

/**
* @brief This function returns the USB charger port types
* 
* @details
*  This function reads the USBIN adapter allowance
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] apsd_result        Refer #pm_schg_usb_apsd_result_status_type. These value could be OR-ed
*             0x1  = PM_SCHG_USB_APSD_RESULT_STATUS_SDP_CHARGER      
*             0x2  = PM_SCHG_USB_APSD_RESULT_STATUS_OCP_CHARGER     
*             0x4  = PM_SCHG_USB_APSD_RESULT_STATUS_CDP_CHARGER      
*             0x8  = PM_SCHG_USB_APSD_RESULT_STATUS_DCP_CHARGER        
*             0x10 = PM_SCHG_USB_APSD_RESULT_STATUS_FLOAT_CHARGER     
*             0x20 = PM_SCHG_USB_APSD_RESULT_STATUS_QC_2P0        
*             0x40 = PM_SCHG_USB_APSD_RESULT_STATUS_QC_3P0  
*             0x80 = PM_SCHG_USB_APSD_RESULT_STATUS_ICL_OVERRIDE_LATCH 
*                                
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_get_apsd_result_status(uint32 device_index, pm_schg_usb_apsd_result_status_type *apsd_result);


/**
* @brief This function configures various pm_schg_usb_icl_options_type values
* 
* @details
*  This function reads the USBIN adapter allowance
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] icl_options         Refer pm_schg_usb_icl_options_type
* @param[in] set_value           set_value
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_configure_usbin_icl_options(uint32 device_index, pm_schg_usb_icl_options_type icl_options, boolean set_value);


/**
* @brief This function sets the USB current limit
* 
* @details
*  This function reads the USBIN adapter allowance
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] usbin_current_limit   USB current to configure. Limits are 0 to 4000mAmp
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_set_usbin_current_limit_cfg(uint32 device_index, uint32 usbin_current_limit);

/**
* @brief This function configures APSD cfg
* 
* @param[in] device_index. Primary: 0. Secondary: 1
* @param[in] apsd_cmd:     APSD Cmd cfg
* @param[in] boolean:      TRUE/FALSE
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_set_cmd_apsd(uint32 device_index, pm_schg_usb_cmd_apsd apsd_cmd, boolean enable);

/**
* @brief This function configures hvdcp2 cfg
* 
* @param[in] device_index. Primary: 0. Secondary: 1
* @param[in] hvdcp2_cmd:   hvdcp2_cmd cfg.
* @param[in] boolean:      TRUE/FALSE
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_set_cmd_hvdcp2(uint32 device_index, pm_schg_usb_cmd_hvdcp2 hvdcp2_cmd, boolean enable);

/**
* @brief This function configures usbin cfg
* 
* @param[in] device_index. Primary: 0. Secondary: 1
* @param[in] usbin_cfg:    usbin cfg
* @param[in] enable:       enable/disable given parameter
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_set_usbin_option1_cfg(uint32 pmic_device, pm_schg_usb_usbin_options_1_cfg usbin_cfg, boolean enable);

/**
* @brief This function gets configuration of  usbin cfg
* 
* @param[in] device_index. Primary: 0. Secondary: 1
* @param[out] usbin_cfg:   usbin cfg
* @param[out] enable:      enable/disable value for retrived param
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_get_usbin_option1_cfg(uint32 pmic_device, pm_schg_usb_usbin_options_1_cfg usbin_cfg, boolean *enable);

/**
 * @brief This function writes to USB_CMD_IL 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] cmd_il_type:  
 *                Refer enum pm_schg_usb_cmd_il_type.  
 * @param[in] set_value: values to be set 
 *                *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_usb_set_cmd_il(uint32 device_index, pm_schg_usb_cmd_il_type cmd_il_type, boolean set_value);

/**
* @brief This function returns the USB type-C status
* 
* @param[in]   device_index. Primary: 0. Secondary: 1
* @param[out]  typec_status:  USB type c status type. Refer #pm_schg_usb_typec_status_type for more info
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_get_typec_status(uint32 pmic_device, pm_schg_usb_typec_status_type *typec_status);

/**
* @brief This function configures current limit and softstart rate for vconn
* 
* @param[in]  device_index. Primary: 0. Secondary: 1
* @param[in]  vconn_ilimit: VCONN current limit. Refer #pm_schg_usb_typec_vconn_ilimit for more info
* @param[in]  ss_pd_ima:    VCONN softstart rate Refer #pm_schg_usb_typec_vconn_ss_pd_ima for more info
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_typec_configure_vconn(uint32 pmic_device, pm_schg_usb_typec_vconn_ilimit vconn_ilimit, pm_schg_usb_typec_vconn_ss_pd_ima ss_pd_ima);

/**
* @brief This function enables vconn based on the input vconn source type
* 
* @param[in]  device_index. Primary: 0. Secondary: 1
* @param[in]  status_type:  VCONN source type. Refer #pm_schg_usb_typec_vconn_enable_type for more info
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_typec_vconn_enable(uint32 pmic_device, pm_schg_usb_typec_vconn_enable_type enable_type);

/**
* @brief This function gets the TypeC port role as DRP/DFP/UFP
* 
* @param[in]   device_index. Primary: 0. Secondary: 1
* @param[out]  port_role:  USB type c port role type. Refer #pm_schg_usb_typec_port_role_type for more info
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_typec_get_port_role(uint32 pmic_device, pm_schg_usb_typec_port_role_type *port_role);

/**
* @brief This function sets the TypeC port role as DRP/DFP/UFP
* 
* @param[in]  device_index. Primary: 0. Secondary: 1
* @param[in]  port_role:  USB type c port role type. Refer #pm_schg_usb_typec_port_role_type for more info
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_typec_set_port_role(uint32 pmic_device, pm_schg_usb_typec_port_role_type port_role);

/**
* @brief This function disables the TYPE-C block
* 
* @param[in]  device_index. Primary: 0. Secondary: 1
* @param[in]  disable: FALSE = Do not disable TYPEC block,
*                      TRUE = Put TYPEC block in disable state 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_typec_disable(uint32 pmic_device, boolean disable);

/* @brief This function sets AICL options configuration
* 
* @param[in]  device_index.  Primary: 0. Secondary: 1
* @param[in]  aicl_cfg_type:  AICL option config type. Refer #pm_schg_usb_aicl_cfg_type for more info
* @param[out] enable:         TRUE: Enable, FALSE: Disable
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_aicl_options_cfg(uint32 device_index, pm_schg_usb_aicl_cfg_type aicl_cfg_type, boolean enable);


/**
* @brief This function enabled ICL Override after APSD.
* 
* @param[in]  device_index. Primary: 0. Secondary: 1
* @param[in]  enable: TRUE  = Use SW to control Input Current Limit after APSD is completed
*                     FALSE = Use APSD results to control Input Current Limit after APSD is completed
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_enable_icl_override_after_apsd(uint32 device_index, boolean enable);


/**
* @brief This function returns USB connector configuration
* 
* @param[in]   device_index. Primary: 0. Secondary: 1
* @param[out]  usb_connector: see pm_schg_usb_connector_type for details
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_get_usb_connector_type(uint32 device_index, pm_schg_usb_connector_type *usb_connector);

/**
* @brief This function configures try sink mode
* 
* @param[in]   device_index. Primary: 0. Secondary: 1
* @param[in]  enable: enable/disable try sink mode
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_usb_typec_enable_try_sink_mode(uint32 pmic_device, boolean enable);

#endif /* __PM_SCHG_USB_H__ */

