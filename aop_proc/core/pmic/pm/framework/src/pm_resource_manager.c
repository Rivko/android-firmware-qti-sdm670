/*! \file pm_resource_manager.c
 *  
 *  \brief  This file contains the initialization functions for PMIC resource manager.
 *  
 *  &copy; Copyright 2011-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/framework/src/pm_resource_manager.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/12/13   hs      Code refactoring.
02/27/13   hs      Code refactoring.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_resource_manager.h"
#include "pm_target_information.h"
#include "pm_comm.h"

/* PMIC Driver includes */
#include "pm_smps_driver.h"
#include "pm_ldo_driver.h"
#include "pm_vs_driver.h"
#include "pm_bob_driver.h"
#include "pm_clk_driver.h"
#include "pm_app_vrm_init.h"

#include "hw_module_type.h"
#include "device_info.h"

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/*! \brief This function returns the parameter specified pmic resource.
 *
 *  \param[in] comm_ptr The comm object of the pmic resource.
 *
 *  \param[in] peripheral_info The peripheral info of the pmic resource.
 */
static void pm_resource_manager_create_pmic_resource
       (pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

__attribute__((section("pm_cram_reclaim_pool")))
void pm_resource_manager_init(void)
{
    peripheral_info_type peripheral_info;

    uint32 pmic_index = 0;
    uint32 sid_index = 0;
    uint32 peripheral_index = 0;
       
    pm_comm_info_type *comm_ptr = NULL;

    uint8  num_slaves_per_pmic = pm_get_num_slaves_per_pmic();

    memset(&peripheral_info, 0 , sizeof(peripheral_info_type));
   
    /*  Go through all valid Comm buses and read peripheral info needed for driver initializations */
    for(; pmic_index < PM_MAX_NUM_PMICS ; pmic_index++)
    {
        if(pm_get_pmic_model(pmic_index ) != PMIC_IS_UNKNOWN )
        	{
				        //every  pmic chip has one or more slaves
				        for(sid_index = 0; sid_index < num_slaves_per_pmic ; sid_index++)
				        {
				            comm_ptr = pm_comm_get_comm_info( pmic_index, sid_index );

				            
				                // Go through each of the 256 base addresses and get peripheral info
				                for(peripheral_index = 0; peripheral_index < 256; peripheral_index++)
				                {
				                    peripheral_info.base_address = peripheral_index * 0x0100;
				                    //read revision information here
				                    if(pm_target_information_read_peripheral_rev(comm_ptr, &peripheral_info) == PM_ERR_FLAG_SUCCESS)
				                    {
				                        pm_resource_manager_create_pmic_resource(comm_ptr, &peripheral_info, pmic_index);
				                    }
				                }
				            
				        }
        	}
    }

    // initialize app layer
    pm_app_vrm_init();
}

__attribute__((section("pm_cram_reclaim_pool")))
void pm_resource_manager_create_pmic_resource(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{    
    switch((pm_hw_module_type)peripheral_info->peripheral_type)
    {
        case PM_HW_MODULE_FTS:
        case PM_HW_MODULE_HF_BUCK:
        case PM_HW_MODULE_ULT_BUCK:
            pm_smps_driver_init(comm_ptr, peripheral_info, pmic_index);
            break;
        case PM_HW_MODULE_LDO:
        case PM_HW_MODULE_ULT_LDO:
            pm_ldo_driver_init(comm_ptr, peripheral_info, pmic_index);
            break;
        case PM_HW_MODULE_VS:
            pm_vs_driver_init(comm_ptr, peripheral_info, pmic_index);
            break;
        case PM_HW_MODULE_BOB:
            pm_bob_driver_init(comm_ptr, peripheral_info, pmic_index);
            break;
        case PM_HW_MODULE_CLOCK:
            pm_clk_driver_init(comm_ptr, peripheral_info, pmic_index);
            break;
        default:
            break;
    }
}

