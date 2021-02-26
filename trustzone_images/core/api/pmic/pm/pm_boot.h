#ifndef __PM_BOOT_H__
#define __PM_BOOT_H__

/*! \file 
 *  \n
 *  \brief  pm_boot.h ---- PMIC boot 
 *  \n
 *  \n This header file contains enums and API definitions for PMIC boot
 *  \n
 *  \n &copy; Copyright 2017 Qualcomm Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tz/2.0.2/api/pmic/pm/pm_boot.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
09/21/11   hs      Added support for TZ(Trust Zone). 
04/21/11   mpt     initial version
=============================================================================*/


#include "comdef.h"
#include "pm_lib_err.h"


/*! \brief This function executes the pmic device initialization
 *  \return Error flag.
 *
 */ 
pm_err_flag_type pm_device_init( void );
/**
 * @brief This function executes the pmic device initialization in TZ.
 * 
 * @details
 *  This function executes the pmic device initialization in TZ, such as 
 *  initializing the SSBI module, etc.
 *   
 * @param[in] void No input needed.
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 *
 */
pm_err_flag_type pm_tz_device_init( void );
/**
 * @brief This function enables the powe rail for Qfprom block.
 * 
 * @details
 *  This function enables the powe rail for Qfprom block. 
 *   
 * @param[in] void No input needed.
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 *
 */
pm_err_flag_type pm_enable_qfuse_power_rail(void);
/**
 * @brief This function disables the powe rail for Qfprom block.
 * 
 * @details
 *  This function disables the powe rail for Qfprom block. 
 *   
 * @param[in] void No input needed.
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 *
 */
pm_err_flag_type pm_disable_qfuse_power_rail(void);

#endif /* PM_BOOT_H */
