/*! \file pm_vrm_vreg.c
 *  \n
 *  \brief This file contains implementation for PMIC VREG resource registration to RPMh VRM.
 *  \n  
 *  \n &copy; Copyright 2016 - 2018 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/vrm/src/pm_vrm_vreg.c#1 $  

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/29/16   kt      Created.
===========================================================================*/
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#include "pm_vrm.h"
#include "pm_target_information.h"
#include "pm_app_vrm_init.h"
#include "pm_rpmh_target.h"
#include "page_select.h"
#include "device_info.h"

__attribute__((section("pm_cram_reclaim_pool")))
void pm_vrm_vreg_pre_register_resource_dependencies(uint8 pmic_index, pm_vrm_periph_type vreg_type, uint32 num_rails)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   vrm_result result = VRM_SUCCESS;
   pm_app_vrm_pwr_info pwr_info = {0};
   pm_pwr_resource** rsrc_info = NULL;
   pm_pwr_resource* pm_rsrc_info = NULL;
   pm_pwr_resource_dependency_info** dep_info = NULL;
   pm_pwr_resource_dependency_info* pm_dep_info = NULL;
   char parent_name[VRM_MAX_RSRC_NAME_LEN] = {0};
   uint32 num_children = 0;
   uint32 rail_index = 0;
   uint32 rsrc_len = 0;
           
   if(num_rails > 0)
   {
      //abort if there was no driver initialized for this pmic
      err_flag = pm_app_vrm_vreg_get_pwr_info(pmic_index, vreg_type, &pwr_info);
      CORE_VERIFY(err_flag == PM_ERR_FLAG_SUCCESS);

      rsrc_info = (pm_pwr_resource**)pm_target_information_get_specific_info(pwr_info.rsrc_prop_id);
      CORE_VERIFY_PTR(rsrc_info);

      dep_info = (pm_pwr_resource_dependency_info**)pm_target_information_get_specific_info(pwr_info.dep_prop_id);
      CORE_VERIFY_PTR(dep_info);

      for( rail_index = 0; rail_index < num_rails; rail_index++)
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

         if((pm_rsrc_info != NULL) && (pm_dep_info != NULL) && (pm_dep_info[rail_index+1].children != NULL))
         {
            if(pm_rsrc_info[rail_index+1].name != NULL)
            {
               rsrc_len = strlen(pm_rsrc_info[rail_index+1].name)+1;
               if (rsrc_len <= VRM_MAX_RSRC_NAME_LEN)
               {
                  strlcpy(parent_name, pm_rsrc_info[rail_index+1].name, rsrc_len);
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
            num_children = pm_dep_info[rail_index+1].num_children;
         }

         // DDR access complete for rsrc_info, dep_info
         set_page_select(0);
         set_page_select(0);

         // abort after ddr access is done
         CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);

         if(num_children > 0)
         {
            result = vrm_pre_register_parent_rsrc_names(VRM_RSRC_REGULATOR, parent_name, num_children);
            CORE_VERIFY(result == VRM_SUCCESS);
         }
      }
   }
}

// making this static variable instead of local variable to avoid stack overflow
__attribute__((section("pm_dram_reclaim_pool")))
static vrm_pmic_rsrc_data pmic_rsrc_data;

__attribute__((section("pm_cram_reclaim_pool")))
void pm_vrm_vreg_register_resources(uint8 pmic_index, pm_vrm_periph_type vreg_type, uint32 num_rails)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   vrm_result result = VRM_SUCCESS;
   uint32 rail_index = 0;
   uint32 slave_id = 0;
   uint32 rsrc_len = 0;
   uint16 base_addr = 0;
   pm_app_vrm_pwr_info pwr_info = {0};
   pm_vrm_vreg_rail_info_type** rail_info = NULL;
   pm_vrm_vreg_rail_info_type* pm_rail_info = NULL;
   pm_pwr_resource** rsrc_info = NULL;
   pm_pwr_resource* pm_rsrc_info = NULL;
   pm_pwr_resource* parent = NULL;
   pm_pwr_resource_dependency_info** dep_info = NULL;
   pm_pwr_resource_dependency_info* pm_dep_info = NULL;
   uint8 vstep = 0;
   uint8 ss_ctl = 0;
   uint8 select_pin = 0;
   uint32 *stepper_rate = NULL;
   pm_volt_level_type vol = 0;
   pm_on_off_type en_status = PM_OFF;
   pm_on_off_type pin_status = PM_OFF;
   uint8 mode_ctl = 0;
   boolean settle_flag = FALSE;
   boolean settle_status = FALSE;
   uint8 access_allowed = 0;
   uint32 drv_owner_mask = 0;
           
   if(num_rails > 0)
   {
      //abort if there was no driver initialized for this pmic
      err_flag = pm_app_vrm_vreg_get_pwr_info(pmic_index, vreg_type, &pwr_info);
      CORE_VERIFY(err_flag == PM_ERR_FLAG_SUCCESS);

      rsrc_info = (pm_pwr_resource**)pm_target_information_get_specific_info(pwr_info.rsrc_prop_id);
      CORE_VERIFY_PTR(rsrc_info);

      dep_info = (pm_pwr_resource_dependency_info**)pm_target_information_get_specific_info(pwr_info.dep_prop_id);
      CORE_VERIFY_PTR(dep_info);

      rail_info = (pm_vrm_vreg_rail_info_type**)pm_target_information_get_specific_info(pwr_info.rail_prop_id);
      CORE_VERIFY_PTR(rail_info);

      // stepper_rate config is common info which is not moved to DDR
      if(vreg_type == PM_VRM_PERIPH_BOB)
      {
         stepper_rate = (uint32*)pm_target_information_get_common_info(PM_PROP_BOB_STEPPER_RATE);
      }
      else
      {
         stepper_rate = (uint32*)pm_target_information_get_common_info(PM_PROP_UNIFIED_RAILS_STEPPER_RATE);
      }
      CORE_VERIFY_PTR(stepper_rate);

      for( rail_index = 0; rail_index < num_rails; rail_index++)
      {
         settle_flag = FALSE;
         access_allowed = 0;

         // *****GROUP ALL THE TARGET CONFIG ACCESSES TO ALLOW MOVING CONFIG DATA TO DDR*****
         
         // get local AOP address for the corresponding DDR address
         pm_rail_info = (pm_vrm_vreg_rail_info_type*)set_ddr_access((uint64_t)rail_info[pmic_index]);

         if(pm_rail_info == NULL)
         {
            err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
         }
         else
         {
            access_allowed = pm_rail_info[rail_index].AccessAllowed;
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

         if((access_allowed) && (pm_rail_info != NULL) && (pm_rsrc_info != NULL) && (pm_dep_info != NULL))
         {
            memset(&pmic_rsrc_data, 0, sizeof(vrm_pmic_rsrc_data));

            // pmic rsrc info
            if(pm_rsrc_info[rail_index+1].name != NULL)
            {
               rsrc_len = strlen(pm_rsrc_info[rail_index+1].name)+1;
               if (rsrc_len <= VRM_MAX_RSRC_NAME_LEN)
               {
                  strlcpy(pmic_rsrc_data.rsrc_name, pm_rsrc_info[rail_index+1].name, rsrc_len);
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

            if(pm_rsrc_info[rail_index+1].alias_name != NULL)
            {
               rsrc_len = strlen(pm_rsrc_info[rail_index+1].alias_name)+1;
               if (rsrc_len <= VRM_MAX_RSRC_NAME_LEN)
               {
                  strlcpy(pmic_rsrc_data.rsrc_alias_name, pm_rsrc_info[rail_index+1].alias_name, rsrc_len);
               }
               else
               {
                  err_flag |= PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
               }
            }

            if(pm_dep_info[rail_index+1].parent != NULL)
            {
               pmic_rsrc_data.dep_en = TRUE;

               // get local AOP addr for DDR parent addr
               parent = (pm_pwr_resource*)set_ddr_access((uint64_t)pm_dep_info[rail_index+1].parent);

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
            pmic_rsrc_data.periph_info.periph_type = vreg_type;
            pmic_rsrc_data.periph_info.periph_index = rail_index;

            pmic_rsrc_data.rsrc_category = VRM_RSRC_REGULATOR;
            err_flag |= pm_vrm_get_drv_mask(pm_rail_info[rail_index].DrvOwnMask, &drv_owner_mask);
            pmic_rsrc_data.drv_owner_mask = drv_owner_mask;

            // mode info
            pmic_rsrc_data.mode_info.min_state = pm_rail_info[rail_index].MinMode;
            pmic_rsrc_data.mode_info.seq_type = (vrm_seq_type)pm_rail_info[rail_index].ModeSeqType;
            pmic_rsrc_data.mode_info.fixed_time_usec = pm_rail_info[rail_index].ModeFixedTime;

            // voltage info
            pmic_rsrc_data.volt_info.min_state = pm_rail_info[rail_index].MinVoltage;
            pmic_rsrc_data.volt_max_state = pm_rail_info[rail_index].MaxVoltage;
            pmic_rsrc_data.volt_info.seq_type = (vrm_seq_type)pm_rail_info[rail_index].VoltSeqType;
            pmic_rsrc_data.volt_info.fixed_time_usec = pm_rail_info[rail_index].VoltFixedUpTime;
            pmic_rsrc_data.volt_fixed_down_time_usec = pm_rail_info[rail_index].VoltFixedDownTime;

            //if fixed time is provided in the config, stepper rate will be ignored by VRM
            pmic_rsrc_data.volt_stepper_extra_up_time_usec = pm_rail_info[rail_index].VoltStepperExtraUpTime;
            pmic_rsrc_data.volt_stepper_extra_down_time_usec = pm_rail_info[rail_index].VoltStepperExtraDownTime;
            pmic_rsrc_data.volt_discharge_time_usec = pm_rail_info[rail_index].VoltDischargeTime;
            pmic_rsrc_data.volt_info.current_state = pmic_rsrc_data.volt_info.min_state;

            // headroom info
            pmic_rsrc_data.hr_info.min_state = pm_rail_info[rail_index].SafetyHR;
            pmic_rsrc_data.hr_info.seq_type = (vrm_seq_type)pm_rail_info[rail_index].HrSeqType;

            // enable info
            pmic_rsrc_data.enable_info.min_state = pm_rail_info[rail_index].AlwaysOn;
            pmic_rsrc_data.enable_info.seq_type = (vrm_seq_type)pm_rail_info[rail_index].EnableSeqType;
            pmic_rsrc_data.enable_info.fixed_time_usec = pm_rail_info[rail_index].EnableFixedTime;

            //if fixed time is provided in the config, stepper rate will be ignored by VRM
            pmic_rsrc_data.enable_stepper_warm_up_time_usec = pm_rail_info[rail_index].EnableStepperWarmUpTime;

            // vreg_ready info
            pmic_rsrc_data.settling_en = pm_rail_info[rail_index].VregReadyEn;
            pmic_rsrc_data.settling_err_en = pm_rail_info[rail_index].VregReadyErrEn;
            pmic_rsrc_data.settling_wdog_timer = (vrm_settling_wdog_timer)pm_rail_info[rail_index].VregWdogTimer;
            pmic_rsrc_data.aop_handling = pm_rail_info[rail_index].AopHandling;
         }

         // DDR access complete for rail_info, rsrc_info, dep_info
         set_page_select(0);
         set_page_select(0);
         set_page_select(0);

         // abort after ddr access is done
         CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);

         // Update any platform/power-grid specific configurations here
         err_flag = pm_rpmh_update_rsrc_config(&pmic_rsrc_data, &access_allowed);

         CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);

         if(access_allowed)
         {
            CORE_VERIFY_PTR(pwr_info.pwr_data);
            CORE_VERIFY_PTR(pwr_info.comm_ptr);

            //skip over invalid peripherals
            if(pwr_info.pwr_data->pwr_specific_info[rail_index].periph_type == 0)
            {
               CORE_VERIFY(0);
            }

            //*****SPMI COMM ACCESS START FROM HERE*****
            
            // Get VRM arbiter id
            slave_id = pwr_info.comm_ptr->slave_id;
            base_addr = pwr_info.pwr_data->pwr_specific_info[rail_index].periph_base_address;
            pmic_rsrc_data.base_addr = base_addr;
            pmic_rsrc_data.base_addr = (slave_id << 16) | pmic_rsrc_data.base_addr;

            err_flag = pm_comm_get_vrm_id (slave_id, base_addr, &pmic_rsrc_data.vrm_arb_id); 
            CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);

            // mode info
            if(vreg_type != PM_VRM_PERIPH_VS)
            {
               pmic_rsrc_data.mode_info.addr_offset = pwr_info.pwr_data->pwr_reg_table->MODE_CTL;
               err_flag = pm_pwr_sw_mode_status_raw_alg(pwr_info.pwr_data, pwr_info.comm_ptr, rail_index, &mode_ctl);
               CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);
               pmic_rsrc_data.mode_info.current_state = mode_ctl;

               // if current state is less than min state, then apply the min state and update the current state
               if(pmic_rsrc_data.mode_info.current_state < pmic_rsrc_data.mode_info.min_state)
               {
                  err_flag = pm_pwr_sw_mode_raw_alg(pwr_info.pwr_data, pwr_info.comm_ptr, rail_index, 
                                                    pmic_rsrc_data.mode_info.min_state);
                  CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);
                  pmic_rsrc_data.mode_info.current_state = pmic_rsrc_data.mode_info.min_state;
               }
            }

            // voltage info
            if(vreg_type != PM_VRM_PERIPH_VS)
            {
               //verify if the range can meet the rail's min, max voltage limits
               if((((pmic_rsrc_data.volt_info.min_state) * 1000) < pwr_info.pwr_data->pwr_specific_info[rail_index].pwr_vset.RangeMin) ||
                  (((pmic_rsrc_data.volt_max_state) * 1000) > pwr_info.pwr_data->pwr_specific_info[rail_index].pwr_vset.RangeMax))
               {
                  err_flag |= PM_ERR_FLAG_DATA_VERIFY_ERR;
               }

               pmic_rsrc_data.volt_info.addr_offset = pwr_info.pwr_data->pwr_reg_table->VOLTAGE_CTRL1;
               err_flag |= pm_pwr_volt_level_status_alg(pwr_info.pwr_data, pwr_info.comm_ptr, rail_index, &vol);
               CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);
               pmic_rsrc_data.volt_info.current_state = (vol/1000);

               // if current state is less than min state, then apply the min state and update the current state
               if(pmic_rsrc_data.volt_info.current_state < pmic_rsrc_data.volt_info.min_state)
               {
                  err_flag = pm_pwr_volt_level_alg(pwr_info.pwr_data, pwr_info.comm_ptr, rail_index, 
                                                  (pmic_rsrc_data.volt_info.min_state * 1000));
                  CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);
                  pmic_rsrc_data.volt_info.current_state = pmic_rsrc_data.volt_info.min_state;
                  settle_flag = TRUE;
               }
            }

            if(pmic_rsrc_data.volt_info.current_state > pmic_rsrc_data.volt_max_state)
            {
               // abort if current state is already greater than max state
               CORE_VERIFY (0);
            }

            // enable info
            pmic_rsrc_data.enable_info.addr_offset = pwr_info.pwr_data->pwr_reg_table->EN_CTL;
            // Read the voltage step value for stepper rails only
            if(pwr_info.pwr_data->pwr_specific_info[rail_index].is_periph_stepper == TRUE)
            {
               err_flag = pm_pwr_volt_get_vstep(pwr_info.pwr_data, pwr_info.comm_ptr, rail_index, &vstep);
               CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);
               vstep = (vstep & PM_STEPPER_DELAY_MASK); // step_delay mask is 0x3 (bits 1:0)

               if(vreg_type == PM_VRM_PERIPH_SMPS)
               {
                  err_flag = pm_pwr_enable_get_softstart(pwr_info.pwr_data, pwr_info.comm_ptr, rail_index, &ss_ctl);
                  CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);
                  ss_ctl = (ss_ctl & PM_STEPPER_DELAY_MASK); // soft start mask is 0x3 (bits 1:0)
               }
               else if(vreg_type == PM_VRM_PERIPH_BOB)
               {
                  vstep = 0;
                  ss_ctl = 0;
               }
               else
               {
                  ss_ctl = vstep;
               }

               //if FTS then divide the value by 2 due to 4mV step size compared to other rails which have 8mV step size
               if(pwr_info.pwr_data->pwr_specific_info[rail_index].periph_type == PM_HW_MODULE_FTS)
               {
                  pmic_rsrc_data.enable_stepper_rate_uv_us = (stepper_rate[ss_ctl])/2;
                  pmic_rsrc_data.volt_stepper_rate_uv_us = (stepper_rate[vstep])/2;
               }
               else
               {
                  pmic_rsrc_data.enable_stepper_rate_uv_us = stepper_rate[ss_ctl];
                  pmic_rsrc_data.volt_stepper_rate_uv_us = stepper_rate[vstep];
               }
            }

            err_flag = pm_pwr_sw_enable_status_alg(pwr_info.pwr_data, pwr_info.comm_ptr, rail_index, &en_status);
            CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);
            pmic_rsrc_data.enable_info.current_state = en_status;

            // if current state is less than min state, then apply the min state and update the current state
            if(pmic_rsrc_data.enable_info.current_state < pmic_rsrc_data.enable_info.min_state)
            {
               err_flag = pm_pwr_sw_enable_alg(pwr_info.pwr_data, pwr_info.comm_ptr, rail_index, 
                                               (pm_on_off_type)pmic_rsrc_data.enable_info.min_state);
               CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);
               pmic_rsrc_data.enable_info.current_state = pmic_rsrc_data.enable_info.min_state;
               settle_flag = TRUE;
            }

            if(vreg_type != PM_VRM_PERIPH_BOB)
            {
               select_pin = 0;
               err_flag = pm_pwr_pin_ctrl_status_alg(pwr_info.pwr_data, pwr_info.comm_ptr, rail_index, &pin_status, &select_pin);
               CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);

               if (select_pin != 0)
               {
                  pmic_rsrc_data.enable_pin_ctrl = TRUE;
               }
            }

            // vreg_ready info
            pmic_rsrc_data.settling_clr_addr_offset = pwr_info.pwr_data->pwr_reg_table->INT_LATCHED_CLR;
            pmic_rsrc_data.settling_clr_data = PM_VREG_READY_IRQ_LATCHED_CLR_DATA;

            // Enable VREG_READY rising edge interrupt if we are using VREG_READY based settling
            if(pmic_rsrc_data.settling_en)
            {
               // settle for the rail if any of the resource minimum states were applied to PMIC
               if((pmic_rsrc_data.enable_info.current_state == PM_ON) && (settle_flag == TRUE))
               {
                  settle_status = FALSE;
                  err_flag = pm_app_vrm_vreg_settle_status(&pmic_rsrc_data.periph_info,
                                                           pmic_rsrc_data.enable_info.fixed_time_usec, &settle_status);
                  CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);
                  CORE_VERIFY (settle_status == TRUE);
               }

               if(vreg_type != PM_VRM_PERIPH_VS)
               {
                  err_flag = pm_pwr_irq_set_trigger_alg(pwr_info.pwr_data, pwr_info.comm_ptr, rail_index, PM_PWR_IRQ_VREG_READY, PM_IRQ_TRIGGER_RISING_EDGE);
                  CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);

                  err_flag = pm_pwr_irq_enable_alg(pwr_info.pwr_data, pwr_info.comm_ptr, rail_index, PM_PWR_IRQ_VREG_READY, TRUE);
                  CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);
               }
            }

            result = vrm_register_rsrc(&pmic_rsrc_data);
            CORE_VERIFY(result == VRM_SUCCESS);
         }
      }
   }
}

