#ifndef __PM_PBS_DRIVER_H__
#define __PM_PBS_DRIVER_H__

/*! \file  pm_pbs_driver.h
 *  
 *  \brief  This file contains the pmic PBS driver definitions for SBL1 boot.
 *  \details  This file contains the pm_pbs_ram_image_load & pm_pbs_ram_image_verify
 *  API definitions for SBl1 boot and also definitions for  other helper functions
 *  used in PBS driver.
 *  
 *  &copy; Copyright 2014-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.


when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
07/16/2014   akm     Comm change Updates
12/18/2013   aab     Updated pm_pbs_ram_image_load() and  pm_pbs_ram_image_verify()  
03/20/2013   kt      Moving and renaming the file to pm_pbs_driver.h 
                     from pm_pbs.h.
10/19/2012   umr     PBS Core Driver.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_lib_err.h"
#include "pmio_pbs_core.h"
#include "com_dtypes.h"


/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/


/** 
  @enum pm_pbs_access_type
  @brief Different modes that can be configured to access PBS 
         ROM/RAM.
 */
typedef enum
{
  PM_PBS_ACCESS_READ_SINGLE,
  PM_PBS_ACCESS_WRITE_SINGLE,
  PM_PBS_ACCESS_READ_BURST,
  PM_PBS_ACCESS_WRITE_BURST,
  PM_PBS_ACCESS_INVALID
}pm_pbs_access_type;



/*=========================================================================
      Function Definitions
==========================================================================*/

/** 
 * @name pm_pbs_enable_access 
 *  
 * @brief This function enables access to PBS ROM/RAM. 
 *  
 * @param[in]  slave_id. Primary PMIC chip: 1. Secondary PMIC Chip: 4
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          else SPMI errors.
 *  
 * @dependencies pm_pbs_disable_access has to be called after 
 *               calling this function to enable access and
 *               required operations to PBS ROM/RAM is done.
 */
pm_err_flag_type 
pm_pbs_enable_access(uint32 slave_id);

/** 
 * @name pm_pbs_disable_access 
 *  
 * @brief This function disables access to PBS ROM/RAM. 
 *  
 * @param[in]  slave_id. Primary PMIC chip: 1. Secondary PMIC Chip: 4
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          else SPMI ERROR.
 *  
 * @dependencies This function is called to disable PBS ROM/RAM 
 *               access only if pm_pbs_enable_access has been
 *               called already to enable PBS ROM/RAM access.
 */
pm_err_flag_type 
pm_pbs_disable_access(uint32 slave_id);

/** 
 * @name pm_pbs_config_access 
 *  
 * @brief This function configures access type to PBS ROM/RAM. 
 *        The PBS ROM/RAM can be accessed in READ SINGLE, WRITE
 *        SINGLE, READ BURST and WRITE BURST modes.
 *  
 * @param[in]  slave_id. Primary PMIC chip: 1. Secondary PMIC Chip: 4
 * @param[in]  addr: PBS address for which the access has to 
 *             configured.
 * @param[in]  access_type: The mode at which you want to access 
 *             PBS ROM/RAM address. Refer to pm_pbs_access_type
 *             enum above for more details.
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Invalid access
 *          type.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          else SPMI ERROR.
 *  
 * @dependencies pm_pbs_enable_access should have been called 
 *               prior to configuring PBS ROM/RAM access type
 *               using this function.
 */
pm_err_flag_type 
pm_pbs_config_access(uint32 slave_id, uint16 addr, pm_pbs_access_type access_type);

#endif // __PM_PBS_DRIVER_H__