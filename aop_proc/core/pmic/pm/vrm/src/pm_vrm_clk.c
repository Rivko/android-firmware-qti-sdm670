/*! \file pm_vrm_clk.c
 *  \n
 *  \brief This file contains implementation for PMIC CLK resource registration to RPMh VRM.
 *  \n  
 *  \n &copy; Copyright 2016 - 2018 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/vrm/src/pm_vrm_clk.c#1 $  

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/29/16   kt      Created.
===========================================================================*/
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#include "pm_vrm.h"
#include "pm_rpmh_target.h"
#include "pm_target_information.h"
#include "page_select.h"


// additional clock warmup delay time in micro secs
#define PM_CLK_EXTRA_WARMUP_DELAY_USEC  30

__attribute__((section("pm_cram_reclaim_pool")))
void pm_vrm_clk_pre_register_resource_dependencies(uint8 pmic_index, pm_vrm_periph_type clk_type, uint32 num_clks)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   vrm_result result = VRM_SUCCESS;
   pm_pwr_resource** rsrc_info = NULL;
   pm_pwr_resource* pm_rsrc_info = NULL;
   pm_pwr_resource_dependency_info** dep_info = NULL;
   pm_pwr_resource_dependency_info* pm_dep_info = NULL;
   char parent_name[VRM_MAX_RSRC_NAME_LEN] = {0};
   uint32 num_children = 0;
   uint32 clk_index = 0;
   uint32 rsrc_len = 0;
           
   if(num_clks > 0)
   {
      rsrc_info = (pm_pwr_resource**)pm_target_information_get_specific_info(PM_PROP_CLK_RSRC);
      CORE_VERIFY_PTR(rsrc_info);

      dep_info = (pm_pwr_resource_dependency_info**)pm_target_information_get_specific_info(PM_PROP_CLK_DEP);
      CORE_VERIFY_PTR(dep_info);

      for( clk_index = 0; clk_index < num_clks; clk_index++)
      {
         memset(&parent_name, 0, VRM_MAX_RSRC_NAME_LEN);
         num_children = 0;

         // *****GROUP ALL THE TARGET CONFIG ACCESSES TO ALLOW MOVING CONFIG DATA TO DDR*****
         
         // get local AOP address for the corresponding DDR address
         pm_rsrc_info = (pm_pwr_resource*)set_ddr_access((uint64_t)rsrc_info[pmic_index]);

         if(pm_rsrc_info == NULL)
         {
            err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
         }

         pm_dep_info = (pm_pwr_resource_dependency_info*)set_ddr_access((uint64_t)dep_info[pmic_index]);

         if(pm_dep_info == NULL)
         {
            err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
         }

         if((pm_rsrc_info != NULL) && (pm_dep_info != NULL) && (pm_dep_info[clk_index+1].children != NULL))
         {
            if(pm_rsrc_info[clk_index+1].name != NULL)
            {
               rsrc_len = strlen(pm_rsrc_info[clk_index+1].name)+1;
               if (rsrc_len <= VRM_MAX_RSRC_NAME_LEN)
               {
                  strlcpy(parent_name, pm_rsrc_info[clk_index+1].name, rsrc_len);
               }
               else
               {
                  err_flag |= PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
               }
            }
            else
            {
               err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
            }
            num_children = pm_dep_info[clk_index+1].num_children;
         }

         // DDR access complete for rsrc_info, dep_info
         set_page_select(0);
         set_page_select(0);

         // abort after ddr access is done
         CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);

         if(num_children > 0)
         {
            result = vrm_pre_register_parent_rsrc_names(VRM_RSRC_XO_BUFFER, parent_name, num_children);
            CORE_VERIFY(result == VRM_SUCCESS);
         }
      }
   }
}

// making this static variable instead of local variable to avoid stack overflow
__attribute__((section("pm_dram_reclaim_pool")))
static vrm_pmic_rsrc_data pmic_rsrc_data;

__attribute__((section("pm_cram_reclaim_pool")))
void pm_vrm_clk_register_resources(uint8 pmic_index, pm_vrm_periph_type clk_type, uint32 num_clks)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   vrm_result result = VRM_SUCCESS;
   uint32 clk_index = 0;
   uint32 slave_id = 0;
   uint32 rsrc_len = 0;
   uint16 base_addr = 0;
   pm_clk_data_type *clk_ptr = NULL;
   pm_vrm_clk_info_type** clk_info = NULL;
   pm_vrm_clk_info_type* pm_clk_info = NULL;
   pm_pwr_resource** rsrc_info = NULL;
   pm_pwr_resource* pm_rsrc_info = NULL;
   pm_pwr_resource* parent = NULL;
   pm_pwr_resource_dependency_info** dep_info = NULL;
   pm_pwr_resource_dependency_info* pm_dep_info = NULL;
   pm_on_off_type en_status = PM_OFF;
   uint32 warmup_time_usec = 0;
   uint32 enable_fixed_time = 0;
   uint8 access_allowed = 0;
   uint32 drv_owner_mask = 0;
           
   if(num_clks > 0)
   {
      rsrc_info = (pm_pwr_resource**)pm_target_information_get_specific_info(PM_PROP_CLK_RSRC);
      CORE_VERIFY_PTR(rsrc_info);

      dep_info = (pm_pwr_resource_dependency_info**)pm_target_information_get_specific_info(PM_PROP_CLK_DEP);
      CORE_VERIFY_PTR(dep_info);

      clk_info = (pm_vrm_clk_info_type**)pm_target_information_get_specific_info(PM_PROP_CLK_INFO);
      CORE_VERIFY_PTR(clk_info);

      for( clk_index = 0; clk_index < num_clks; clk_index++)
      {
         access_allowed = 0;

         // *****GROUP ALL THE TARGET CONFIG ACCESSES TO ALLOW MOVING CONFIG DATA TO DDR*****
         
         // get local AOP address for the corresponding DDR address
         pm_clk_info = (pm_vrm_clk_info_type*)set_ddr_access((uint64_t)clk_info[pmic_index]);

         if(pm_clk_info == NULL)
         {
            err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
         }
         else
         {
            access_allowed = pm_clk_info[clk_index].AccessAllowed;
         }

         pm_rsrc_info = (pm_pwr_resource*)set_ddr_access((uint64_t)rsrc_info[pmic_index]);

         if(pm_rsrc_info == NULL)
         {
            err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
         }

         pm_dep_info = (pm_pwr_resource_dependency_info*)set_ddr_access((uint64_t)dep_info[pmic_index]);

         if(pm_dep_info == NULL)
         {
            err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
         }

         if((access_allowed) && (pm_clk_info != NULL) && (pm_rsrc_info != NULL) && (pm_dep_info != NULL))
         {
            memset(&pmic_rsrc_data, 0, sizeof(vrm_pmic_rsrc_data));

            // pmic rsrc info
            if(pm_rsrc_info[clk_index+1].name != NULL)
            {
               rsrc_len = strlen(pm_rsrc_info[clk_index+1].name)+1;
               if (rsrc_len <= VRM_MAX_RSRC_NAME_LEN)
               {
                  strlcpy(pmic_rsrc_data.rsrc_name, pm_rsrc_info[clk_index+1].name, rsrc_len);
               }
               else
               {
                  err_flag |= PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
               }
            }
            else
            {
               err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
            }

            if(pm_rsrc_info[clk_index+1].alias_name != NULL)
            {
               rsrc_len = strlen(pm_rsrc_info[clk_index+1].alias_name)+1;
               if (rsrc_len <= VRM_MAX_RSRC_NAME_LEN)
               {
                  strlcpy(pmic_rsrc_data.rsrc_alias_name, pm_rsrc_info[clk_index+1].alias_name, rsrc_len);
               }
               else
               {
                  err_flag |= PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
               }
            }

            if(pm_dep_info[clk_index+1].parent != NULL)
            {
               pmic_rsrc_data.dep_en = TRUE;

               // get local AOP addr for DDR parent addr
               parent = (pm_pwr_resource*)set_ddr_access((uint64_t)pm_dep_info[clk_index+1].parent);

               if((parent != NULL) && (parent->name != NULL))
               {
                  rsrc_len = strlen(parent->name)+1;
                  if (rsrc_len <= VRM_MAX_RSRC_NAME_LEN)
                  {
                     strlcpy(pmic_rsrc_data.rsrc_parent_name, parent->name, rsrc_len);
                  }
                  else
                  {
                     err_flag |= PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
                  }
               }
               else
               {
                  err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
               }

               // DDR access complete for parent
               set_page_select(0);
            }

            pmic_rsrc_data.periph_info.pmic_index = pmic_index;
            pmic_rsrc_data.periph_info.periph_type = PM_VRM_PERIPH_CLK;
            pmic_rsrc_data.periph_info.periph_index = clk_index + 1;

            pmic_rsrc_data.rsrc_category = VRM_RSRC_XO_BUFFER;
            err_flag |= pm_vrm_get_drv_mask(pm_clk_info[clk_index].DrvOwnMask, &drv_owner_mask);
            pmic_rsrc_data.drv_owner_mask = drv_owner_mask;

            // enable info
            pmic_rsrc_data.enable_info.min_state = pm_clk_info[clk_index].AlwaysOn;
            pmic_rsrc_data.enable_info.seq_type = (vrm_seq_type)pm_clk_info[clk_index].EnableSeqType;
            enable_fixed_time = pm_clk_info[clk_index].EnableFixedTime;
         }

         // DDR access complete for clk_info, rsrc_info, dep_info
         set_page_select(0);
         set_page_select(0);
         set_page_select(0);

         // abort after ddr access is done
         CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);

         err_flag = pm_rpmh_update_rsrc_config(&pmic_rsrc_data, &access_allowed);

         if(access_allowed)
         {
            //abort if there was no driver initialized for this pmic 
             clk_ptr = pm_clk_get_data(pmic_index);
             CORE_VERIFY_PTR(clk_ptr);

            //*****SPMI COMM ACCESS START FROM HERE*****
            slave_id = clk_ptr->comm_ptr->slave_id;
            base_addr = clk_ptr->clk_reg_table->base_address + (clk_ptr->clk_reg_table->peripheral_offset*(clk_index+1));
            pmic_rsrc_data.base_addr = base_addr;
            pmic_rsrc_data.base_addr = (slave_id << 16) | pmic_rsrc_data.base_addr;

            // Get VRM arbiter id
            err_flag = pm_comm_get_vrm_id (slave_id, base_addr, &pmic_rsrc_data.vrm_arb_id); 
            CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);

            // enable info
            pmic_rsrc_data.enable_info.addr_offset = clk_ptr->clk_reg_table->EN_CTL;

            // get programmed warm-up time and use that as fixed settling time
            warmup_time_usec = 0;
            err_flag = pm_clk_get_warmup_time(pmic_index, clk_ptr->clk_common[clk_index+1], &warmup_time_usec);
            warmup_time_usec = warmup_time_usec + PM_CLK_EXTRA_WARMUP_DELAY_USEC;

            pmic_rsrc_data.enable_info.fixed_time_usec = MAX(warmup_time_usec, enable_fixed_time);

            err_flag = pm_clk_sw_enable_status(pmic_index, clk_ptr->clk_common[clk_index+1], &en_status);
            CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);
            pmic_rsrc_data.enable_info.current_state = en_status;

            // if current state is less than min state, then apply the min state and update the current state
            if(pmic_rsrc_data.enable_info.current_state < pmic_rsrc_data.enable_info.min_state)
            {
               err_flag = pm_clk_sw_enable(pmic_index, clk_ptr->clk_common[clk_index+1], 
                                           (pm_on_off_type)pmic_rsrc_data.enable_info.min_state);
               CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);
               pmic_rsrc_data.enable_info.current_state = pmic_rsrc_data.enable_info.min_state;
            }

            result = vrm_register_rsrc(&pmic_rsrc_data);
            CORE_VERIFY(result == VRM_SUCCESS);
         }
      }
   }
}

