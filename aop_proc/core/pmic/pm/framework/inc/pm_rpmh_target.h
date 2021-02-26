#ifndef PM_RPMH_TARGET_H
#define PM_RPMH_TARGET_H

/*! \file pm_rpmh_target.h
*  \n
*  \brief This file contains PMIC function prototypes for sleep related functions.
*  \n
*  &copy; Copyright 2013-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/framework/inc/pm_rpmh_target.h#2 $  

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/24/14   kt      Updated comments header and added doxygen comments.
08/16/13   hs      Initial version. 
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_app_pwr.h"

/*===========================================================================

                        MACRO DEFINITIONS 

===========================================================================*/

typedef enum
{
	PM_READ,
	PM_WRITE
}pm_read_write;

typedef enum
{
	PM_MODE,
	PM_VOLTAGE
}pm_type_of_read_write;


/*===========================================================================

                        GENERIC FUNCTION PROTOTYPES

===========================================================================*/

/** 
 * @name pm_rpmh_sleep_init 
 *  
 * @brief This function initializes the sleep settings LUT.
 * 
 * @param None
 *
 * @return pm_err_flag_type 
 *  
 * @dependencies Sleep settings should be available in config.
 *
 */
pm_err_flag_type pm_rpmh_sleep_init(void);

/** 
 * @name pm_rpmh_platform_init 
 *  
 * @brief This function initialize the platform specific 
 *        settings.
 * 
 * @param None
 *
 * @return pm_err_flag_type 
 *  
 * @sideeffects None
 *
 */
pm_err_flag_type pm_rpmh_platform_init(void);

/** 
 * @name pm_rpmh_platform_post_init 
 *  
 * @brief This function initalizes the platform settings post 
 *        pmic_init
 * 
 * @param None
 *
 * @return pm_err_flag_type 
 *  
 * @sideeffects None
 *
 */
pm_err_flag_type pm_rpmh_platform_post_init(void);

/**
* @brief This function initialize the platform specific settings
*        prior to drivers initialization
* 
* @param none
*
* @return pm_err_flag_type
*
*/
pm_err_flag_type pm_rpmh_platform_pre_init (void);

/**
* @brief This function is used to stubbed pmic driver for particular target
*        
* 
* @param none
*
* @return pm_err_flag_type
*
*/
boolean pm_is_target_specific_stub_enable(void);

/**
* @brief This function updates the PMIC resource config based on
*        platform specific settings before
*        registering/configuring the resource in RPMh VRM.
* 
* @param PMIC resource data
*
* @return pm_err_flag_type
*
*/
pm_err_flag_type pm_rpmh_update_rsrc_config(vrm_pmic_rsrc_data* data, uint8* access_allowed);

pm_err_flag_type pm_rpmh_pmic_detect_err_init(void);

pm_err_flag_type pm_rpmh_target_core_rail_config(pm_core_rail_name rail_name ,uint64 val , pm_type_of_read_write mode_volt, pm_read_write read_write ,pm_request_type active_ret , uint64 *data);

#endif /* PM_RPMH_TARGET_H */
