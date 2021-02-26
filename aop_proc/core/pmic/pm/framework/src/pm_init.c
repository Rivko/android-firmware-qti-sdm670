/*! \file pm_init.c
*
*  \brief PMIC drivers initialization.
*  \n
*  &copy; Copyright 2010-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/framework/src/pm_init.c#1 $  

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/23/13   hs      Fixed the naming convention in \config.
04/12/13   hs      Code refactoring.
02/27/13   hs      Code refactoring.
05/10/11   jtn     Fix RPM init bug for 8960
07/01/10   umr     Created.
===========================================================================*/
#include "pm_comm.h"
#include "pm_target_information.h"
#include "pm_resource_manager.h"
#include "pm_rpmh_target.h"
#include "device_info.h"
#include "pm_debug.h"
#include "pm_vrm.h"
#include "pm_prm_init.h"
#include "pm_rpmh.h"

#define PM_INDEX   0


__attribute__((section("pm_cram_reclaim_pool")))
void pm_init( void )
{
    pm_comm_channel_init_internal();
    
    pm_version_detect();

    if (TRUE == pm_is_stub_enabled())
    {
        pm_rpmh_pmic_detect_err_init();
        return;
    }

    pm_target_information_init();
    
    pm_rpmh_platform_pre_init();

    pm_comm_info_init();

    pm_resource_manager_init();

    pm_vrm_init();

    pm_rpmh_init();
    
    pm_prm_init();

    pm_rpmh_platform_init();

    pm_rpmh_sleep_init();

    pm_rpmh_platform_post_init();

    pm_debug_info_init();

    return;
}

__attribute__((section("pm_cram_reclaim_pool")))
boolean pm_is_stub_enabled ( void )
{
   return pm_is_target_specific_stub_enable();
    
}
