#ifndef __PM_APP_CHGR_COMMON__H__
#define __PM_APP_CHGR_COMMON__H__

/*! \file pm_app_chgr_common.h
 *  \n
 *  \brief  PMIC charger algorithms commonly used by XBL loader and XBL core
 *  \details  This file contains functions and variable declarations to support
 *   the PMIC SBL charging
 *
 *
 *  \n &copy; Copyright (c) 2017  by QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================
                                Edit History
This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
 
05/15/17   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_pbs_client.h"

/*=========================================================================== 
 
                     GLOBAL DEFINES
 
===========================================================================*/



/*=========================================================================== 
 
                     GLOBAL TYPE DEFINITIONS
 
===========================================================================*/

typedef struct
{
   uint8   hot_limit; 
   uint8   cold_limit;
} pm_app_chg_temp_limit_config_type;


typedef struct
{
   uint8   pmic_index;
   uint8   limit_reg_sid_index;
   uint32  hot_limit_reg_addr;
   uint32  cold_limit_reg_addr;
   pm_pbs_client_type               client;
   pm_pbs_client_trigger            client_trig_num;
   pm_pbs_client_trigger_edge_type  client_trig_edge;
   uint8*   pbs_afp_enable_trigger_arg_value;
   uint8*   pbs_afp_disable_trigger_arg_value;
   boolean enable_afp;
} pm_app_chg_temp_monitor_config_type;



/*===========================================================================

                     PROTOTYPES 

===========================================================================*/

/**
 * @name 
 *     pm_app_chgr_enable_temp_monitoring
 *
 * @description
 *    This function is used to config temperature threshold and
 *    trigger AFP PBS routine
 * 
 * @param 
 *    hot_limit_val: uint8
 *    cold_limit_val: uint8
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type
pm_app_chgr_enable_temp_monitoring(uint8 hot_limit_val, uint8 cold_limit_val);



#endif  //__PM_APP_CHGR_COMMON__H__

