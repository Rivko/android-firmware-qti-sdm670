#ifndef __PM_PON_CFG_H__
#define __PM_PON_CFG_H__

/*! \ file pm_pon_cfg.h
*
 * \brief   <Brief Comment Block/>
 * \details <Detailed Comment Block/>
 * \n &copy; Copyright 2015-2017 QUALCOMM Technolgoies Incorporated, All Rights Reserved
 */

/*===================================================================
                    EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tz/2.0.2/pmic/pm/framework/inc/pm_pon_cfg.h#1 $
$Author: pwbldsvc $

when       who     what, where, why
--------    ---     ------------------------------------------------
02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
04/15/16    aab     Added pm_tgt_get_pshold_reset_cfg(); pm_pon_gp_reset_cfg() and pm_pon_tgt_specific_pshold_cfg()
10/07/15    vtw     Created.
====================================================================*/

#include "pm_pon.h"
#include "pm_lib_err.h"


/*=========================================================================== 
 
                     GLOBAL DEFINES
 
===========================================================================*/
enum
{
  PON_WARM_RST                      = PM_PON_RESET_CFG_WARM_RESET,
  PON_SHDN                          = PM_PON_RESET_CFG_NORMAL_SHUTDOWN,
  PON_DVDD_SHDN                     = PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN,
  PON_XVDD_SHDN                     = PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  PON_HARD_RST                      = PM_PON_RESET_CFG_HARD_RESET,
  PON_DVDD_HARD_RST                 = PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET,
  PON_XVDD_HARD_RST                 = PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PON_IMMEDIATE_XVDD_SHDN           = PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  PON_WARM_RST_THEN_DVDD_SHDN       = PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_SHUTDOWN,
  PON_WARM_RST_THEN_XVDD_SHDN       = PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  PON_WARM_RST_THEN_SHDN            = PM_PON_RESET_CFG_WARM_RESET_THEN_SHUTDOWN,
  PON_WARM_RST_THEN_HARD_RST        = PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET,
  PON_WARM_RST_THEN_DVDD_HARD_RST   = PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET,
  PON_WARM_RST_THEN_XVDD_HARD_RST   = PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PON_NO_CFG_NEEDED                 = PM_PON_RESET_CFG_INVALID  
};


/* ============================================================================
**    Function Prototype.
** ==========================================================================*/

/**
 * @brief Get PSHOLD configuration.
 *
 * @details This function gets PMIC specific PSHOLD configuration
 *
 *  @param pmic_index [in]  PMIC device index.
 *  @param app_pshold_cfg [in]  desired pshold configuration
 *  @param pshold_cfg [out]     Pointer to PSHOLD configuration for a specific PMIC
 *
 * @return pm_err_flag_type
 *  PM_ERR_FLAG__SUCCESS -- on success otherwise PMIC error code. \n
 * @dependencies
 *  None.
 * @sa None.
 *
 */
pm_err_flag_type 
pm_tgt_get_pshold_reset_cfg( uint32                      pmic_index,
                             pm_app_pon_reset_cfg_type   app_pshold_cfg,
                             pm_pon_reset_cfg_type*      pshold_cfg);



/**
 * @brief Configures PMIC GP1/GP2 Reset. .
 *  
 * @details Configure GP1/GP2 PON source and reset type for all the
 *        applicable PMICs internally
 *  
 * @param app_pon_reset_source  :   pm_app_pon_reset_source_type 
 *  
 *@param app_pon_reset_cfg : Supported configurations: 
 *                               PM_APP_PON_CFG_DVDD_HARD_RESET 
 *                               PM_APP_PON_CFG_WARM_RESET,
 *   
 *                   
 * @return pm_err_flag_type PM_ERR_FLAG__SUCCESS = SUCCESS else 
 *         ERROR.
 */
pm_err_flag_type
pm_pon_gp_reset_cfg( uint32  pmic_device_index, pm_pon_reset_source_type reset_source, pm_pon_reset_cfg_type reset_cfg_type) ;




/**
 * @brief Check for target specific  Reset configuration.
 *
 * @details This function checks for target specific aditional 
 *          reset configuration for the specific
 *          PMIC.
 *
 *  @param pmic_index [in]  PMIC device index.
 *  @param pon_cfg [in]  Desired PON Configuration.
 *  
 *  @return pm_err_flag_type
 *  PM_ERR_FLAG__SUCCESS -- on success otherwise PMIC error code. \n
 * @dependencies
 *  None.
 * @sa None.
 *
 */
pm_err_flag_type
pm_pon_tgt_specific_pshold_cfg(uint32 pmic_index, pm_app_pon_reset_cfg_type app_pshold_cfg);



#endif // PM_PON_CFG_H
