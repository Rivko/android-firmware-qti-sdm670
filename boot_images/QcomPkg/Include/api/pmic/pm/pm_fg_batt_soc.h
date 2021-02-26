#ifndef __PM_FG_BATT_SOC_H__
#define __PM_FG_BATT_SOC_H__


/*! \file pm_fg_batt_soc.h 
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
05/23/17   sv      Added pm_fg_get_batt_soc_enable_ctl API
04/21/16   va     Adding FG Rst APIs
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
  PM_FG_BATT_SOC_SOC_UPDT,   /*every fg cyc when soc/info updated*/
  PM_FG_BATT_SOC_SOC_READY,  /*FG completes first boot or re-estimate*/
  PM_FG_BATT_SOC_BSOC_DELTA, /*battery SoC change exceeds delta*/
  PM_FG_BATT_SOC_MSOC_DELTA, /*monotonic SoC change exceeds delta*/
  PM_FG_BATT_SOC_MSOC_LOW,   /*monotonic SoC <= low threshold*/
  PM_FG_BATT_SOC_MSOC_EMPTY, /*monotonic SoC == 0% or vBt < vEmpty; whichever occurs first*/
  PM_FG_BATT_SOC_MSOC_HIGH,  /*monotonic SoC >= high threshold*/
  PM_FG_BATT_SOC_MSOC_FULL,  /*monotonic SoC == 100%*/
  PM_FG_BATT_SOC_IRQ_INVALID,/*invalid irq*/
} pm_fg_batt_soc_irq_type;

typedef enum
{

  PM_FG_BATT_SOC_RST_CTRL_DVD_RST, /* Local DVD RST CTRL*/
  PM_FG_BATT_SOC_RST_CTRL_BCL_RST, /* Local BCL RST CTRL*/
  PM_FG_BATT_SOC_RST_CTRL_MEM_RST, /* Local Memory Interface Reset CTRL*/
  PM_FG_BATT_SOC_RST_CTRL_ALG_RST, /* Local FG Algo reset CTRL*/
  PM_FG_BATT_SOC_RST_CTRL_INVALID, /* Invalid RST CTRL*/
}pm_fg_batt_soc_rst_ctrl;

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
* @param[in] pm_fg_batt_soc_irq_type               irq type
* @param[in] boolean        enable/disable value
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_soc_irq_enable(uint32 device_index, pm_fg_batt_soc_irq_type irq, boolean enable);

/**
* @brief This function clears irq
* 
* @details
*  This function clears irq
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_batt_soc_irq_type               irq type
*
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
  pm_err_flag_type pm_fg_batt_soc_irq_clear(uint32  device_index, pm_fg_batt_soc_irq_type irq);
  
/**
* @brief This function sets interrupt triggers
* 
* @details
*  This function sets interrupt triggers
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_batt_soc_irq_type               irq type
* @param[in] pm_irq_trigger_type               trigger
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_soc_irq_set_trigger(uint32 device_index, pm_fg_batt_soc_irq_type irq, pm_irq_trigger_type trigger);

/**
* @brief This function returns irq status
* 
* @details
*  This function returns irq status
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_batt_soc_irq_type        irq type
* @param[in] pm_irq_status_type        irq status type
* @param[in] status                   irq TRUE/FALSE status
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_soc_irq_status(uint32 device_index, pm_fg_batt_soc_irq_type irq, pm_irq_status_type type, boolean *status);


/**
* @brief This function returns the monontonic soc
* 
* @details
*  This function returns the monontonic soc
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] soc              Monotonic SOC
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_soc_get_monotonic_soc(uint32 device_index, uint32* soc);


/**
* @brief This function Control to take FG in and out shutown
* 
* @details
*  This function Control to take FG in and out shutown
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] enable              TRUE : fg_active; FALSE : fg_in_shutdown
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_soc_enable_fg_algorithm(uint32 device_index, boolean enable);

/**
* @brief This function Trigger a RELOAD/RESTART sequence
* 
* @details
*  This function Trigger a RELOAD/RESTART sequence
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] restart             TRUE : execute_sequence; FALSE : do_not_execute_sequence
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_soc_restart(uint32 device_index, boolean restart);

/**
* @brief This function Trigger requested FG RESTART 
* 
* @details
*  This function Triggers requested FG RESTART 
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] fg_rst_ctrl : refer pm_fg_batt_soc_rst_ctrl
* @param[in] enable              TRUE/FALSE
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_soc_set_fg_rst_ctrl(uint32 device_index, pm_fg_batt_soc_rst_ctrl fg_rst_ctrl, boolean enable);

/**
* @brief This function gets FG RESTART status
* 
* @details
*  This function gets FG RESTART status
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] fg_rst_ctrl : refer pm_fg_batt_soc_rst_ctrl
* @param[out] enable              TRUE/FALSE
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_soc_get_fg_rst_ctrl(uint32 device_index, pm_fg_batt_soc_rst_ctrl fg_rst_ctrl, boolean *enable);

/**
* @brief This function gets Fule Gauge batt_soc_enable_ctl  sts
* 
* @details
*  This function gets Fule Gauge batt_soc_enable_ctl  sts
* 
* @param[in]  device_index.           Primary: 0 Secondary: 1
* @param[out] enable                  TRUE/FALSE
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_get_batt_soc_enable_ctl(uint32 pmic_device, boolean *enable);


#endif /* __PM_FG_BATT_SOC_H__ */
