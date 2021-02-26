#ifndef __PM_DEBUG_H__
#define __PM_DEBUG_H__

/*! \file pm_debug.h
 *  
 *  \brief This file contains PMIC debug related definitions.
 *  
 *    &copy; Copyright 2016 - 2017 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/framework/inc/pm_debug.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/16   kt      Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_bob_driver.h"
#include "pm_clk_driver.h"
#include "pm_ldo_driver.h"
#include "pm_smps_driver.h"
#include "pm_vs_driver.h"
#include "pm_version.h"
#include "pm_prm_driver.h"
#include "pm_app_vrm_init.h"
/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/



/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

typedef struct
{
  pm_device_info_type        *pm_device_info_arr;
  pm_bob_data_type           **pm_bob_data_arr;
  pm_clk_data_type           **pm_clk_data_arr;
  pm_ldo_data_type           **pm_ldo_data_arr;
  pm_smps_data_type          **pm_smps_data_arr;
  pm_vs_data_type            **pm_vs_data_arr;

  pm_prm_driver_info_type    *pm_prm_driver_info;
  pm_app_vrm_info_type       *pm_app_vrm_info;
}pm_debug_info_type;

pm_debug_info_type* pm_get_debug_info(void);

pm_err_flag_type pm_debug_info_init(void);

#endif // __PM_DEBUG_H__
