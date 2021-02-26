#ifndef __PM_SCHG_DC_H__
#define __PM_SCHG_DC_H__

/*! \file
*  \n
*  \brief  pm_schg_dc.h PMIC-SMBC MODULE RELATED DECLARATION 
*  \details  This header file contains functions and variable declarations 
*  to support Qualcomm PMIC SCHG (Switch-Mode Battery Charger) module. The 
*  Switched-Mode Battery Charger (SCHG) module includes a buck regulated 
*  battery charger with integrated switches. The SCHG module, along with the 
*  Over Voltage Proection (OVP) module will majorly be used by charger 
*  appliation for charging Li-Ion batteries with high current (up to 2A).
*  \n &copy;
*  Copyright (c) 2016 - 2017  Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/17   ll      Added API for OTG workaround for OTG device with large cap
05/27/16   pxm     Add ZIN configuration function 
01/24/16   al      Initial version. 
========================================================================== */
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/


/*! \enum pm_schg_usb_chgpth_irq_bit_field_type
   \brief different types of irq bit fields of by smbb irq module
 */
typedef enum {
  PM_SCHG_DC_IRQ_DCIN_COLLAPSE,       /*The DCIN voltage has crossed the collapse threshold*/
  PM_SCHG_DC_IRQ_DCIN_VASHDN,         /*The DCIN voltage has crossed the auto-shutdown threshold*/
  PM_SCHG_DC_IRQ_DCIN_UV,             /*The DCIN voltage has crossed the under-voltage threshold*/
  PM_SCHG_DC_IRQ_DCIN_OV,             /*The DCIN voltage has crossed the over-voltage threshold*/
  PM_SCHG_DC_IRQ_DCIN_PLUGIN,         /*DCIN plugged in, goes low on un-plug*/
  PM_SCHG_DC_IRQ_DIV2_EN_DG,          /*DIV2_EN Pin Deglitched*/
  PM_SCHG_DC_IRQ_DCIN_ICL_CHANGE,     /*Change in DCIN input current limit*/
  PM_SCHG_DC_IRQ_WIPWR_VOLTAGE_RANGE, /*WIPWR/DCIN Input Voltage Range Change*/
  PM_SCHG_DC_IRQ_INVALID,             /*Invalid*/
}pm_schg_dc_irq_type; 


typedef enum
{
  PM_SCHG_DC_CMD_IL_TYPE_DCIN_SUSPEND,     /*DCIN Suspend: 0 = normal 1 = suspend*/
  PM_SCHG_DC_CMD_IL_TYPE_SHDN_N_SET_CMD,   /*Set WIPWR Shutdown Latch - not self clearing: 0 = Do Not Set/End Set 1 = Set WIPWR Shutdown Latch (SHDN_N_LATCH = 0)''*/
  PM_SCHG_DC_CMD_IL_TYPE_SHDN_N_CLEAR_CMD, /*Clear WIPWR Shutdown Latch - not self clearing: 0 = Do Not Clear/End Clear 1 = Clear WIPWR Shutdown Latch (SHDN_N_LATCH = 1)*/
  PM_SCHG_DC_CMD_IL_TYPE_WIRELESS_CHG_DIS, /*CHG_OK pin control (in software over-ride control mode): 0 = high-z (allow charging) 1 = low (disable charging)*/
  PM_SCHG_DC_CMD_IL_TYPE_INVALID           /*Invalid*/
}pm_schg_dc_cmd_il_type; 


typedef enum
{
  PM_SCHG_DC_ZIN_ICL_PT,        /*The Wipower charging current limit register when PT mode and voltage under 8V*/ 
  PM_SCHG_DC_ZIN_ICL_LV,        /*The Wipower charging current limit register when DIVBY2 mode and voltage under 5.5V*/
  PM_SCHG_DC_ZIN_ICL_HV,        /*The Wipower charging current limit register when DIVBY2 mode and voltage above 8V*/
  PM_SCHG_DC_ZIN_ICL_PT_HV,     /*The Wipower charging current limit register when PT mode and voltage above 8V*/
  PM_SCHG_DC_ZIN_ICL_MID_LV,    /*The Wipower charging current limit register when DIVBY2 mode and voltage between 5.5 to 6.5V*/
  PM_SCHG_DC_ZIN_ICL_MID_HV,    /*The Wipower charging current limit register when DIVBY2 mode and voltage between 6.5 to 8V*/
  PM_SCHG_DC_ZIN_ICL_INVALID,   /*Invalid*/
}pm_schg_dc_zin_icl_res_type; 


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
 * @param[in] irq                 pm_schg_dc_irq_type
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_dc_irq_enable(uint32 device_index, pm_schg_dc_irq_type irq, boolean enable);


/**
 * @brief This function clears the SCHG irq 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                SCHG irq type. Refer enum pm_schg_dc_irq_type.  
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_dc_irq_clear(uint32  device_index, pm_schg_dc_irq_type irq);


/**
 * @brief This function configures the SCHG irq trigger 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                SCHG irq type. Refer enum pm_schg_dc_irq_type.  
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
pm_err_flag_type pm_schg_dc_irq_set_trigger(uint32 device_index, pm_schg_dc_irq_type irq, pm_irq_trigger_type trigger);


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
pm_err_flag_type pm_schg_dc_irq_status(uint32 device_index, pm_schg_dc_irq_type irq, pm_irq_status_type type, boolean *status);

/**
 * @brief This function writes to DC_CMD_IL 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] cmd_il_type:  
 *                Refer enum pm_schg_dc_cmd_il_type.  
 * @param[in] set_value: values to be set 
 *                *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_schg_dc_set_cmd_il(uint32 device_index, pm_schg_dc_cmd_il_type cmd_il_type, boolean set_value);


/**
* @brief This function sets the DCIN current limit
* 
* @details
*  This function reads the DCIN adapter allowance
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] dcin_current_limit   DCIN current to configure. Limits are 0 to 3000mAmp
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_dc_set_usbin_current_limit_cfg(uint32 device_index, uint32 dcin_current_limit);


/**
* @brief This function sets the wipower ZIN current limit
* 
* @details
*  this function sets the wipower ZIN current limit
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] wipower_icl_res_type   ZIN register type
* @param[in] ilmit_level_ma   ZIN current to configure. Limits are 0 to 3000mAmp
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_dc_set_zin_icl(uint32 device_index, pm_schg_dc_zin_icl_res_type zin_icl_res_type, uint32 ilmit_level_ma);

/**
* @brief This function sets OTG softstart current limit
* 
* @details
*  this function sets ivref charging current and input current limit reference DACOTG
*    (OTG softstart front porch FET current limit), 000=slowest, 111=fastest
*      
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] ivref_fpfet_curLimit:  000=slowest, 111=fastest
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_schg_dc_set_ivref_otg_ss(uint32 device_index, uint8 ivref_fpfet_curLimit);
#endif /* __PM_SCHG_DC_H__ */

