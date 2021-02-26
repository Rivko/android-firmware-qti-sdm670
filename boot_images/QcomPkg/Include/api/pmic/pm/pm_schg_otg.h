#ifndef __PM_SCHG_OTG_H__
#define __PM_SCHG_OTG_H__

/*! \file
*  \n
*  \brief  pm_schg_otg.h PMIC-SMBC MODULE RELATED DECLARATION 
*  \details  This header file contains functions and variable declarations 
*  to support Qualcomm PMIC SCHG OTG (Switch-Mode Battery Charger) module. The 
*  Switched-Mode Battery Charger (SCHG OTG) module includes a buck regulated 
*  battery charger with integrated switches. The SCHG OTG module, along with the 
*  Over Voltage Proection (OVP) module will majorly be used by charger 
*  appliation for charging Li-Ion batteries with high current (up to 2A).
*  \n &copy;
*  Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/08/17   ll      Added API for OTG workaround for OTG device with large cap
12/06/16   sm      Added API to return OTG BOOST SoftStart done status
06/09/16   sm      Redefined pm_schg_otg_status_type struct
                   Created pm_schg_otg_get_settings() to return OTG configuration
01/14/16   al      Initial version. 
========================================================================== */
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
typedef enum
{
  PM_SCHG_OTG_IRQ_OTG_FAIL,               /*OTG was running, but disabled due to some event*/
  PM_SCHG_OTG_IRQ_OTG_OVERCURRENT,        /*The OTG output current reached the current limit setting, and caused the output voltage to drop below 3.6V*/
  PM_SCHG_OTG_IRQ_OTG_OC_DIS_SW_STS,      /*OTG Overcurrent Disabled Switcher*/
  PM_SCHG_OTG_IRQ_TESTMODE_CHANGE_DETECT, /*For use only with test mode.*/
  PM_SCHG_OTG_IRQ_INVALID,                /*INVALID*/
}pm_schg_otg_irq_type;


typedef enum
{
  PM_SCHG_OTG_CFG_CONCURRENT_MODE_CFG,      /*0 = DFP CC Cycle Time 2ms           1 = DFP CC Cycle Time 10ms*/
  PM_SCHG_OTG_CFG_OTG_EN_SRC_CFG,           /*0 = OTG Enable Source Command       1 = OTG Enable Source Type-C/RID*/
  PM_SCHG_OTG_CFG_ENABLE_OTG_IN_DEBUG_MODE, /*0 = Do not enable OTG in debug mode 1 = Enable OTG in debug mode*/
  PM_SCHG_OTG_CFG_INCREASE_DFP_TIME,        /*0 = DFP CC Cycle Time 2ms           1 = DFP CC Cycle Time 10ms*/
  PM_SCHG_OTG_CFG_QUICKSTART_FASTROLESWAP,  /*0 = check for USBIN < 1V before enabling OTG startup sequence  1 = not check*/
  PM_SCHG_OTG_CFG_INVALID,                  /*INVALID*/
}pm_schg_otg_cfg_type;


typedef struct
{
  boolean concurrent_mode_cfg;      /*0 = DFP CC Cycle Time 2ms           1 = DFP CC Cycle Time 10ms*/
  boolean otg_en_src_cfg;           /*0 = OTG Enable Source Command       1 = OTG Enable Source Type-C/RID*/
  boolean enable_otg_in_debug_mode; /*0 = Do not enable OTG in debug mode 1 = Enable OTG in debug mode*/
  boolean increase_dfp_time;        /*0 = DFP CC Cycle Time 2ms           1 = DFP CC Cycle Time 10ms*/
}pm_schg_otg_cfg_setting;


typedef struct
{
  boolean otg_enable;
  pm_schg_otg_cfg_setting cfg_status;
  uint32 milli_volt;
  uint32 otg_limit_ma;
}pm_schg_otg_cfg_setting_type;


/**  OTG status type **/
typedef enum
{
    PM_SCHG_OTG_DISABLED,
    PM_SCHG_OTG_EXECUTING_ENABLE,
    PM_SCHG_OTG_ENABLED,
    PM_SCHG_OTG_EXECUTING_DISABLE,
    PM_SCHG_OTG_ERROR,
    PM_SCHG_OTG_STATUS_INVALID
} pm_schg_otg_status_type;

/*===========================================================================

                 SCHG OTG DRIVER FUNCTION PROTOTYPES

===========================================================================*/
/**
 * @brief This function returns OTG configuration.
 * 
 * @details
 *  This function returns OTG configuration.
 * 
 * @param[in] pmic_device_index. Secondary: 1, Primary: 0
 * @param[out] otg_cfg:    Refer #pm_schg_otg_cfg_type for 
 *       more info
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_otg_enable(uint32 pmic_device,boolean enable);

/**
 * @brief This function returns OTG configuration.
 * 
 * @details
 *  This function returns OTG configuration.
 * 
 * @param[in] pmic_device_index. Secondary: 1, Primary: 0
 * @param[out] otg_cfg:    Refer #pm_schg_otg_cfg_type for 
 *       more info
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_otg_config_otg(uint32 pmic_device, pm_schg_otg_cfg_type otg_cfg_type, boolean set_value);

/**
 * @brief This function returns OTG configuration.
 * 
 * @details
 *  This function returns OTG configuration.
 * 
 * @param[in] pmic_device_index. Secondary: 1, Primary: 0
 * @param[out] otg_cfg:    Refer #pm_schg_otg_cfg_type for 
 *       more info
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_otg_set_otg_batt_uvlo(uint32 pmic_device, uint32 milli_volt);

/**
 * @brief This function returns OTG configuration.
 * 
 * @details
 *  This function returns OTG configuration.
 * 
 * @param[in] pmic_device_index. Secondary: 1, Primary: 0
 * @param[in] otg_limit_ma:    Refer #pm_schg_otg_cfg_type for 
 *       more info
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_otg_set_otg_i_limit(uint32 pmic_device, uint32 otg_limit_ma);

/**
 * @brief This function returns OTG configuration.
 * 
 * @details
 *  This function returns OTG configuration.
 * 
 * @param[in] pmic_device_index. Secondary: 1, Primary: 0
 * @param[out] otg_cfg:    Refer #pm_schg_otg_cfg_type for 
 *       more info
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_otg_get_status(uint32 pmic_device, pm_schg_otg_status_type* otg_status);


 /**
 * @brief This function enables the SCHG irq 
 * 
 * @details
 *  This function enables the SCHG irq 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] irq                 refer pm_schg_otg_irq_type
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_otg_irq_enable(uint32 pmic_device, pm_schg_otg_irq_type irq, boolean enable);

 /**
 * @brief This function clears the SCHG irq 
 * 
 * @details
 *  This function clears the SCHG irq 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] irq                 refer pm_schg_otg_irq_type
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_otg_irq_clear(uint32  pmic_device, pm_schg_otg_irq_type irq);

 /**
 * @brief This function configures the SCHG irq trigger 
 * 
 * @details
 *  This function configures the SCHG irq trigger 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] irq                 refer pm_schg_otg_irq_type
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_otg_irq_set_trigger(uint32 pmic_device, pm_schg_otg_irq_type irq, pm_irq_trigger_type trigger);

 /**
 * @brief This function configures the SCHG for irq 
 * 
 * @details
 *  This function configures the SCHG for irq 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] irq                 refer pm_schg_otg_irq_type
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_otg_irq_status(uint32 pmic_device, pm_schg_otg_irq_type irq, pm_irq_status_type type, boolean *status);

/**
 * @brief This function returns OTG configuration.
 * 
 * @details
 *  This function returns OTG configuration.
 * 
 * @param[in] pmic_device_index. Secondary: 1, Primary: 0
 * @param[out] otg_settings:    Refer #pm_schg_otg_cfg_setting_type for 
 *       more info
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_otg_get_settings(uint32 pmic_device, pm_schg_otg_cfg_setting_type* otg_settings);



/**
 * @brief This function returns OTG boost soft start done status.
 * 
 * @details
 *  This function returns OTG boost soft start done status.
 * 
 * @param[in] pmic_device_index. Secondary: 1, Primary: 0
 * @param[out] softstart_done:   TRUE: Indicates BOOST softstart done
 *                               FALSE: Indicates BOOST softstart not done
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_otg_get_boost_softstart_done_status(uint32 pmic_device, boolean * softstart_done);

/**
 * @brief This function configures 1_in_8 mode in OTG mode.
 * 
 * @details
 *  This function sets halt 1_in_8 mode in OTG mode.
 *
 * @param[in] pmic_device_index. Secondary: 1, Primary: 0
 * @param[in] enable:  FALSE(default): 1_in_8 mode enabled
 *                     TRUE: 1_in_8 mode disabled
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_otg_halt_1_in_8_mode(uint32 pmic_device, boolean enable);

/**
 * @brief This function returns if boost output is regulating at 5V, 1110[2:0] = 000.
 * 
 * @details
 *  This function returns if boost output is regulating at 5V, 1110[2:0] = 000.
 *       1110[2] OTG_OC_DIS_SW_STS_RT_STS: OTG overcurrent disabled switcher
 *       1110[1] OTG_OVERCURRENT_RT_STS  : OTG output current reached limit setting, 
 *                                         caused the output voltage to drop below 3.6V
 *       1110[0] OTG_FAIL_RT_STS         : OTG was running, but disabled due to some event
 *
 *
 * @param[in] pmic_device_index. Secondary: 1, Primary: 0
 * @param[out] pbOtgOK;    TRUE: boost output is regulating at 5V (no issue on OTG)
 *                        FALSE: there's issue with OTG causing it not regulating at 5V
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_otg_get_rt_sts_ok(uint32 pmic_device, boolean *pbOtgOK);
#endif /* __PM_SCHG_OTG_H__*/

