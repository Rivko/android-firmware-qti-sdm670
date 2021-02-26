/*! \file pm_utils.c
 *  
 *  \brief This file contain commonly used PMIC utils related function implementation.
 *  \details This file contain PMIC timetick related helper functions implementation
 *           and wrapper function of DALSYS_Malloc()
 *  
 *    &copy; Copyright 2016 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/framework/src/pm_debug.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/16   al      Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_debug.h"

pm_debug_info_type pm_debug;
__attribute__((section("pm_cram_reclaim_pool")))
pm_debug_info_type* pm_get_debug_info(void)
{
  return &pm_debug;
}

__attribute__((section("pm_cram_reclaim_pool")))
pm_err_flag_type pm_debug_info_init(void)
{
  pm_debug_info_type *pmic_debug = pm_get_debug_info();
  CORE_VERIFY_PTR(pmic_debug);

  return PM_ERR_FLAG_SUCCESS;
}
