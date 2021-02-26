/*! \file pm_vrm.c
 *  \n
 *  \brief This file contains implementation for PMIC VRM initialization.
 *  \n  
 *  \n &copy; Copyright 2016 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/vrm/src/pm_vrm.c#1 $  

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/29/16   kt     Created
===========================================================================*/
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#include "pm_vrm.h"
#include "device_info.h"


/*===========================================================================

                  LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/

/*===========================================================================

                         LOCAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                           VARIABLES DEFINITIONS

===========================================================================*/


/*===========================================================================

                            FUNCTION DEFINITIONS

===========================================================================*/

__attribute__((section("pm_cram_reclaim_pool")))
void pm_vrm_register_resources (void)
{
    uint8  pmic_count = 0;
    uint32 num_of_peripherals = 0;
    uint8  num_of_pmics = PM_MAX_NUM_PMICS;
    pm_model_type pmic_model = PMIC_IS_INVALID;

    for(pmic_count = 0; pmic_count < num_of_pmics; pmic_count++)
    {
        pmic_model = pm_get_pmic_model(pmic_count);

        if((pmic_model == PMIC_IS_INVALID) || (pmic_model == PMIC_IS_UNKNOWN))
        {
            continue;
        }

        num_of_peripherals = pm_smps_get_num_peripherals(pmic_count);
        if(num_of_peripherals > 0)
        {
           pm_vrm_vreg_pre_register_resource_dependencies(pmic_count, PM_VRM_PERIPH_SMPS, num_of_peripherals);
        }

        num_of_peripherals = pm_ldo_get_num_peripherals(pmic_count);
        if(num_of_peripherals > 0)
        {
           pm_vrm_vreg_pre_register_resource_dependencies(pmic_count, PM_VRM_PERIPH_LDO, num_of_peripherals);
        }

        num_of_peripherals = pm_vs_get_num_peripherals(pmic_count);
        if(num_of_peripherals > 0)
        {
           pm_vrm_vreg_pre_register_resource_dependencies(pmic_count, PM_VRM_PERIPH_VS, num_of_peripherals);
        }

        num_of_peripherals = pm_bob_get_num_peripherals(pmic_count);
        if(num_of_peripherals > 0)
        {
           pm_vrm_vreg_pre_register_resource_dependencies(pmic_count, PM_VRM_PERIPH_BOB, num_of_peripherals);
        }

        num_of_peripherals = pm_clk_get_num_peripherals(pmic_count);
        if(num_of_peripherals > 0)
        {
           num_of_peripherals = num_of_peripherals - 1; //XO is not regisreted as one of the clk buffer resources
           pm_vrm_clk_pre_register_resource_dependencies(pmic_count, PM_VRM_PERIPH_CLK, num_of_peripherals);
        }
    }

    //To rgister pre dependcency info for target specific resources
    pm_vrm_target_pre_register_resource_dependencies();

    for(pmic_count = 0; pmic_count < num_of_pmics; pmic_count++)
    {
        pmic_model = pm_get_pmic_model(pmic_count);

        if((pmic_model == PMIC_IS_INVALID) || (pmic_model == PMIC_IS_UNKNOWN))
        {
            continue;
        }

        num_of_peripherals = pm_smps_get_num_peripherals(pmic_count);
        if(num_of_peripherals > 0)
        {
           pm_vrm_vreg_register_resources(pmic_count, PM_VRM_PERIPH_SMPS, num_of_peripherals);
        }

        num_of_peripherals = pm_ldo_get_num_peripherals(pmic_count);
        if(num_of_peripherals > 0)
        {
           pm_vrm_vreg_register_resources(pmic_count, PM_VRM_PERIPH_LDO, num_of_peripherals);
        }

        num_of_peripherals = pm_vs_get_num_peripherals(pmic_count);
        if(num_of_peripherals > 0)
        {
           pm_vrm_vreg_register_resources(pmic_count, PM_VRM_PERIPH_VS, num_of_peripherals);
        }

        num_of_peripherals = pm_bob_get_num_peripherals(pmic_count);
        if(num_of_peripherals > 0)
        {
           pm_vrm_vreg_register_resources(pmic_count, PM_VRM_PERIPH_BOB, num_of_peripherals);
        }

        num_of_peripherals = pm_clk_get_num_peripherals(pmic_count);
        if(num_of_peripherals > 0)
        {
           num_of_peripherals = num_of_peripherals - 1; //XO is not regisreted as one of the clk buffer resources
           pm_vrm_clk_register_resources(pmic_count, PM_VRM_PERIPH_CLK, num_of_peripherals);
        }
    }

    // Target specific resources that needs to be registered in VRM (example: pbs, gpio)
    pm_vrm_target_register_resources();
   
}

// initialize VRM and register PMIC resources to VRM
__attribute__((section("pm_cram_reclaim_pool")))
void pm_vrm_init (void)
{
    vrm_init();
                     
    pm_vrm_register_resources ();

    vrm_post_init();
} 


pm_err_flag_type pm_vrm_get_drv_mask(uint32 drv_mask, uint32* ret_drv_mask)
{
   uint32 drv_id = 0;
   uint32 new_drv_id = 0;
   *ret_drv_mask = 0;

   for(drv_id = 0; drv_id < VRM_RSC_DRV_MAX; drv_id++)
   {
      if(((drv_mask >> drv_id) & 0x1)
         && (get_version_drv_id(drv_id, &new_drv_id) == DRV_ID_LOOKUP_SUCCESS)
         )
      {
         *ret_drv_mask = (*ret_drv_mask) | (0x1 << new_drv_id);
      }
   }

   return PM_ERR_FLAG_SUCCESS;
}
