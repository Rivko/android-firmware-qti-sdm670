#ifndef __PM_FG_BCL_H__
#define __PM_FG_BCL_H__


/*! \file pm_fg_bcl.h 
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
05/24/16   sm      Added API to enable goc
03/15/16   aab     Added/updated pm_fg_bcl_get_vbatt() and pm_fg_bcl_get_vbatt_match_cp()
02/16/16   va      Adding required API for enanlbing charger
01/12/16   al      Initial Release 
===========================================================================*/

#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================
                        TYPE DEFINITIONS 
===========================================================================*/

/* Common Interrupt Prototype */
typedef enum
{
  PM_FG_BCL_IBT_HI,      /*iBtHi -> Dx -> iBtHiIntr*/
  PM_FG_BCL_IBT_THI,     /*BtTHi -> Dx -> iBtTHiIntr*/
  PM_FG_BCL_VBT_LO_ADC,  /*vBtLoAdc ->Dx ->vBtLoIntrAdc*/
  PM_FG_BCL_VBT_LO_CMP,  /*vBtLoCmp ->Dx ->vBtLoIntrCmp*/
  PM_FG_BCL_VBT_TLO_CMP, /*vBtTLoCmp ->Dx ->vBtTLoCmpIntr*/
  PM_FG_BCL_TRPN,        /*bcl accumulated data avail for read*/
  PM_FG_BCL_BCL_BUS_ERR, /*bus error in transaction*/
  PM_FG_BCL_IRQ_INVALID, /*invalid FG BCL irq*/
} pm_fg_bcl_irq_type;


typedef enum
{
  PM_FG_BCL_BGOCEN = 0x07, /* Enable Gain Offset correctiong in HW for VBAT */
  PM_FG_BCL_INVALID, /* Invalid RST CTRL*/
}pm_fg_bcl_cfg;

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
* @param[in] pm_fg_bcl_irq_type               irq type
* @param[in] boolean        enable/disable value
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_bcl_irq_enable(uint32 device_index, pm_fg_bcl_irq_type irq, boolean enable);

/**
* @brief This function clears irq
* 
* @details
*  This function clears irq
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_bcl_irq_type               irq type
*
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
  pm_err_flag_type pm_fg_bcl_irq_clear(uint32  device_index, pm_fg_bcl_irq_type irq);
  
/**
* @brief This function sets interrupt triggers
* 
* @details
*  This function sets interrupt triggers
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_bcl_irq_type               irq type
* @param[in] pm_irq_trigger_type               trigger
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_bcl_irq_set_trigger(uint32 device_index, pm_fg_bcl_irq_type irq, pm_irq_trigger_type trigger);

/**
* @brief This function returns irq status
* 
* @details
*  This function returns irq status
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_bcl_irq_type        irq type
* @param[in] pm_irq_status_type        irq status type
* @param[out]boolean                   status TRUE/FALSE status
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_bcl_irq_status(uint32 device_index, pm_fg_bcl_irq_type irq, pm_irq_status_type type, boolean *status);


/**
* @brief This function returns the battery current
* 
* @details
*  This function returns the battery current
* 
* @param[in] pmic_device_index.         Primary: 0 Secondary: 1
* @param[out] current_ma                current in milliAmp
* @param[out] does_current_match_cp     wheather current and its copy register read same value
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_bcl_get_ibat(uint32 device_index, int32 *current_ma, boolean *does_current_match_cp);


/**
* @brief This function returns the battery voltage level in mv
* 
* @details
*  This function returns the battery voltage
*  It tries upto 5 times for FG_BCL_VBATT and FG_BCL_VBATT_CP reading to match 
* 
* @param[in]  device_index.           Primary: 0 Secondary: 1
* @param[out] vbatt_mv                vbatt level in mv
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_bcl_get_vbatt(uint32 pmic_device, int32 *vbatt_mv);


/**
* @brief This function enable/disables Fule Gauge Algorithm BCL 
*        (battery current limiting s/w use)
* 
* @details
*  This function enable/disables Fule Gauge Algorithm BCL
*  (battery current limiting s/w use)
* 
* @param[in]  device_index.           Primary: 0 Secondary: 1
* @param[in]  enable                  TRUE/FALSE
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_bcl_enable_ctl(uint32 pmic_device, boolean enable);


/**
* @brief This function gets Fule Gauge Algorithim BCL (battery 
*        current limiting s/w use) sts
* 
* @details
*  This function gets Fule Gauge Algorithm BCL (battery current limiting s/w use) sts
* 
* @param[in]  device_index.           Primary: 0 Secondary: 1
* @param[out] enable                  TRUE/FALSE
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_get_bcl_enable_ctl(uint32 pmic_device, boolean *enable);

/**
* @brief This function gets Fule Gauge Algorithim BCL Cfg  
* @details
*  This function gets Fule Gauge Algorithm BCL (battery current limiting s/w use) sts
* 
* @param[in]  device_index.           Primary: 0 Secondary: 1
* @param[in]  bcl_cfg .               BCL Cfg Element 
* @param[out] enable                  TRUE/FALSE
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_set_bcl_cfg(uint32 pmic_device, pm_fg_bcl_cfg bcl_cfg, boolean enable);

/**
* @brief This function enables BCL Gain and offset comparator 
* 
* @details
*  This function enables BCL Gain and offset comparator 
* 
* @param[in]  device_index.           Primary: 0 Secondary: 1
* @param[out] enable                  TRUE/FALSE
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_bcl_enable_goc(uint32 pmic_device, boolean enable);




#endif /* __PM_FG_BCL_H__ */
