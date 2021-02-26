/*! \file pm_target_information.c
*  
*  \brief This file contains functions to return target specific and common PMIC settings.
*  \n   
*  &copy; Copyright 2010-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/framework/src/pm_target_information.c#1 $ 

when       who        	what, where, why
--------   ---    	   ----------------------------------------------------------
06/11/13    hs          Adding settling time for regulators.
06/04/13    aks         Remove range related hacks
05/29/13    aks         Clk driver input check (do not disable critical clocks)
05/29/13    kt          Added Peripheral Probe fix.
05/20/13    aks         Clk driver code re-architecture
04/23/13    hs          Fixed the naming convention in \config.
04/16/13    hs          Added workaround for 8110 L1.
04/12/13    hs          Code refactoring.
02/27/13    hs          Code refactoring.
08/07/12    hs          Added support for 5V boost.
04/16/12    hs          Removed irq files.
04/03/10    wra	        Creating file for MSM8960
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_target_information.h"
#include "pm_version.h"
#include "pm_comm.h"
#include "device_info.h"
#include "pm_prm_device_defs.h"

/*===========================================================================

                        STATIC/EXTERN VARIABLES 

===========================================================================*/

// common info
extern pm_clk_type *clk_common;
extern pm_clk_register_info_type* clk_reg;
extern pm_pwr_register_info_type* smps_reg;
extern pm_pwr_register_info_type* ldo_reg;
extern pm_pwr_register_info_type* vs_reg;
extern pm_pwr_register_info_type* bob_reg;
extern uint32* unified_rails_stepper_rate;
extern uint32* bob_stepper_rate;

// specific info
extern pm_vrm_vreg_rail_info_type** smps_rail;
extern pm_pwr_resource** smps_rsrc;
extern pm_pwr_resource_dependency_info** smps_dep;
extern uint32* num_of_smps;

extern pm_vrm_vreg_rail_info_type** ldo_rail;
extern pm_pwr_resource** ldo_rsrc;
extern pm_pwr_resource_dependency_info** ldo_dep;
extern uint32* num_of_ldo;

extern pm_vrm_vreg_rail_info_type** vs_rail;
extern pm_pwr_resource** vs_rsrc;
extern pm_pwr_resource_dependency_info** vs_dep;
extern uint32* num_of_vs;

extern pm_vrm_vreg_rail_info_type** bob_rail;
extern pm_pwr_resource** bob_rsrc;
extern pm_pwr_resource_dependency_info** bob_dep;
extern uint32* num_of_bob;

extern pm_vrm_clk_info_type** clk_info;
extern pm_pwr_resource** clk_rsrc;
extern pm_pwr_resource_dependency_info** clk_dep;


extern pm_vrm_target_rsrc_info_type* target_rsrc_info;
extern uint32* num_of_target_rsrc;

extern uint32* rpmh_drv_id;
extern pm_prm_node_resource_info* pm_prm_pam_rsrcs;
extern uint32* num_of_pam_rsrcs;
extern pm_prm_resource_type* pm_prm_device_rsrcs;

/*===========================================================================

                     FUNCTION DEFINITIONS

===========================================================================*/
__attribute__((section("pm_cram_reclaim_pool")))
void pm_target_information_init(void)
{
}
pm_err_flag_type pm_target_information_read_peripheral_rev(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type base_address = (pm_register_address_type)peripheral_info->base_address;
    uint32 digital_minor_index = 0;
    uint32 digital_major_index = 1;
    uint32 analog_minor_index = 2;
    uint32 analog_major_index = 3;
    uint32 peripheral_type_index = 4;
    uint32 peripheral_subtype_index = 5;
    uint32 peripheral_dummy_index = 6;
    const uint32 num_of_bytes = 7;
    pm_register_data_type temp_peripheral_info[7] = {0};

    if(!comm_ptr)
    {
        return PM_ERR_FLAG_BUS_ERR;
    }

    // Reset the peripheral info
    peripheral_info->peripheral_type = 0;
    peripheral_info->peripheral_subtype = 0;
    peripheral_info->analog_major_version = 0;
    peripheral_info->analog_minor_version = 0;
    peripheral_info->digital_major_version = 0;
    peripheral_info->digital_minor_version = 0;

    /* Burst read the peripheral info */
    pm_comm_read_byte_array(comm_ptr->slave_id, base_address, num_of_bytes, temp_peripheral_info, 0);

    /* When we burst read multiple bytes from a non-existent peripheral, the data returned
       should be same on all the bytes so we need to do the below check and return error */
    if(temp_peripheral_info[peripheral_subtype_index] == temp_peripheral_info[peripheral_dummy_index]) 
    {
        return PM_ERR_FLAG_PERIPHERAL_ERR;
    }

    /* Valid peripheral type can never be 0 */   
    if(temp_peripheral_info[peripheral_type_index] == 0) 
    {
        return PM_ERR_FLAG_PERIPHERAL_ERR;
    }
    
    peripheral_info->peripheral_type = temp_peripheral_info[peripheral_type_index];    
    peripheral_info->peripheral_subtype = temp_peripheral_info[peripheral_subtype_index];
    peripheral_info->digital_major_version = temp_peripheral_info[digital_major_index];
    peripheral_info->digital_minor_version = temp_peripheral_info[digital_minor_index];
    peripheral_info->analog_major_version = temp_peripheral_info[analog_major_index];
    peripheral_info->analog_minor_version = temp_peripheral_info[analog_minor_index];
    
    return err_flag;
}

__attribute__((section("pm_cram_reclaim_pool")))
void* pm_target_information_get_common_info(uint32 prop_id)
{
  void *info = NULL;

  switch(prop_id)
  {
     case PM_PROP_SMPS_REG:
        info = (void*)&smps_reg;
        break;
     case PM_PROP_LDO_REG:
        info = (void*)&ldo_reg;
        break;
     case PM_PROP_VS_REG:
        info = (void*)&vs_reg;
        break;
     case PM_PROP_BOB_REG:
        info = (void*)&bob_reg;
        break;
     case PM_PROP_CLK_REG:
        info = (void*)&clk_reg;
        break;
     case PM_PROP_CLK_COMMON:
        info = (void*)&clk_common;
        break;
     case PM_PROP_UNIFIED_RAILS_STEPPER_RATE:
        info = (void*)&unified_rails_stepper_rate;
        break;
     case PM_PROP_BOB_STEPPER_RATE:
        info = (void*)&bob_stepper_rate;
        break;
     default:
        info = NULL;
        break;
  }

  return info;
}

__attribute__((section("pm_cram_reclaim_pool")))
void* pm_target_information_get_specific_info(uint32 prop_id )
{
  void *info = NULL;

  switch(prop_id)
  {
     case PM_PROP_SMPS_RAIL:
        info = (void*)&smps_rail;
        break;
     case PM_PROP_SMPS_RSRC:
        info = (void*)&smps_rsrc;
        break;
     case PM_PROP_SMPS_DEP:
        info = (void*)&smps_dep;
        break;
     case PM_PROP_LDO_RAIL:
        info = (void*)&ldo_rail;
        break;
     case PM_PROP_LDO_RSRC:
        info = (void*)&ldo_rsrc;
        break;
     case PM_PROP_LDO_DEP:
        info = (void*)&ldo_dep;
        break;
     case PM_PROP_VS_RAIL:
        info = (void*)&vs_rail;
        break;
     case PM_PROP_VS_RSRC:
        info = (void*)&vs_rsrc;
        break;
     case PM_PROP_VS_DEP:
        info = (void*)&vs_dep;
        break;
     case PM_PROP_BOB_RAIL:
        info = (void*)&bob_rail;
        break;
     case PM_PROP_BOB_RSRC:
        info = (void*)&bob_rsrc;
        break;
     case PM_PROP_BOB_DEP:
        info = (void*)&bob_dep;
        break;
     case PM_PROP_CLK_INFO:
        info = (void*)&clk_info;
        break;
     case PM_PROP_CLK_RSRC:
        info = (void*)&clk_rsrc;
        break;
     case PM_PROP_CLK_DEP:
        info = (void*)&clk_dep;
        break;
     case PM_PROP_TARGET_RSRC_INFO:
        info = (void*)&target_rsrc_info;
        break;
     case PM_PROP_TARGET_RSRC_NUM:
        info = (void*)&num_of_target_rsrc;
        break;
     case PM_PROP_RPMH_DRV_ID:
        info = (void*)&rpmh_drv_id;
        break;
     case PM_PROP_PRM_PAM_RSRCS:
        info = (void*)&pm_prm_pam_rsrcs;
        break;
     case PM_PROP_PRM_PAM_RSRC_NUM:
        info = (void*)&num_of_pam_rsrcs;
        break;
     case PM_PROP_PRM_DEVICE_RSRCS:
        info = (void*)&pm_prm_device_rsrcs;
        break;
     default:
        info = NULL;
        break;
  }

  return info;
}

__attribute__((section("pm_cram_reclaim_pool")))
uint32 pm_target_information_get_periph_count_info(uint32 prop_id, uint8 pmic_index)
{
  uint32 *peripheral_count = NULL;

  if(pmic_index < PM_MAX_NUM_PMICS)
  {
     switch(prop_id)
     {
        case PM_PROP_SMPS_NUM:
           peripheral_count = (uint32*)&num_of_smps;
           break;
        case PM_PROP_LDO_NUM:
           peripheral_count = (uint32*)&num_of_ldo;
           break;
        case PM_PROP_VS_NUM:
           peripheral_count = (uint32*)&num_of_vs;
           break;
        case PM_PROP_BOB_NUM:
           peripheral_count = (uint32*)&num_of_bob;
           break;
        default:  
           return 0;
     }
     return peripheral_count[pmic_index];
  }

  return 0;
}

