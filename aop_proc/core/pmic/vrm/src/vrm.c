/*! \file vrm.c
*
*  \brief Implementation file for VRM public APIs. This is mainly a routing layer
*         to VRM driver functions where the actual implementation is present.
* 
*  &copy; Copyright 2016 - 2018 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/src/vrm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_inc.h"
#include "vrm_driver.h"
#include "vrm_utils.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

//******************************************************************************
// Global Data
//******************************************************************************

static boolean vrm_post_initialized = FALSE;
static boolean vrm_task_initialized = FALSE;

//******************************************************************************
// Local Helper Functions
//******************************************************************************

//******************************************************************************
// Public API Functions
//******************************************************************************
/**
 * @brief Initializes the VRM driver
 *
 * This function initializes the VRM driver and does the basic 
 * VRM HW configuration. This function also does the command DB 
 * initialization for VRM entries. It is mandatory to call this 
 * function before using any other APIs of this driver. 
 * 
 * @param none
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_init()
{
   vrm_result result = VRM_SUCCESS;

   result = vrm_driver_init();

   CORE_VERIFY(result == VRM_SUCCESS);

   return result;
}
                            
/**
 * @brief Pre-register the parent resource names.
 *
 * This function assigns (or reserves) a resource id for the 
 * parent resource name and saves the number of children 
 * information per parent. This is purely for memory and latency
 * optimization purposes for AOP parent-child dependency. User 
 * has to pre-register any parent resource name before 
 * registering any child resource of that parent. This function 
 * also adds the command db entry for parent resource. 
 * 
 * @param[in] rsrc_category Resource category 
 * @param[in] rsrc_name Parent resource name which needs 
 *                      pre-assigned resource id.
 * @param[in] num_children Number of children for the parent 
 *                         resource name being sent
 *
 * @return  VRM_SUCCESS on success, error code on error
 * 
 * @see vrm_register_rsrc
 */
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_pre_register_parent_rsrc_names(vrm_rsrc_category rsrc_category, char* rsrc_name, uint32 num_children)
{
   vrm_result result = VRM_SUCCESS;
   vrm_rsrc_names rsrc_names;
   uint32 rsrc_id = 0;

   rsrc_names.name = NULL;
   rsrc_names.alias_name = NULL;
   rsrc_names.parent_name = NULL;

   if((num_children == 0) || (num_children >= VRM_MAX_NUM_CHILDREN))
   {
      result = VRM_OUT_OF_BOUND_ERROR;
   }
   else if(vrm_post_initialized == TRUE)
   {
      result = VRM_FAILURE;
   }
   else if((rsrc_name == NULL) || (strlen(rsrc_name) == 0))
   {
      result = VRM_INVALID_PARAM;
   }
   else
   {
      rsrc_names.name = rsrc_name;

      result = vrm_assign_rsrc_id(rsrc_category, &rsrc_names, num_children, &rsrc_id);
   }

   CORE_VERIFY(result == VRM_SUCCESS);

   return result;
}
                            
/**
 * @brief Registers the resource in VRM.
 *
 * This function assigns a resource id for the resource name and 
 * configures the resource data and corresponding type data in 
 * the VRM HW. If the resource has parent resource name in the
 * pmic_rsrc_data then the dependency interrupt of the resource 
 * will be enabled in VRM HW and the dependency mapping will be 
 * maintained within VRM driver for run time parent-child 
 * dependency interrupt handling. This functions also adds a 
 * command DB entry for the registered resource. 
 * 
 * @param[in] pmic_rsrc_data PMIC resource data that needs to be
 *                           configured in VRM HW.
 *
 * @return  VRM_SUCCESS on success, error code on error
 * 
 * @see vrm_pre_register_parent_rsrc_names
 */

// making this static variable instead of local variable to avoid stack overflow
__attribute__((section("pm_dram_reclaim_pool")))
static vrm_rsrc_type_data type_data;
__attribute__((section("pm_dram_reclaim_pool")))
static vrm_rsrc_data rsrc_data;

__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_register_rsrc(vrm_pmic_rsrc_data* pmic_rsrc_data)
{
   vrm_result result = VRM_SUCCESS;
   vrm_rsrc_names rsrc_names;
   uint32 time_vrm = 0;
   uint32 stepper_vrm_mv = 0;
   uint32 type_id = 0;
   uint32 seq_id = 0;

   rsrc_names.name = NULL;
   rsrc_names.alias_name = NULL;
   rsrc_names.parent_name = NULL;
   
   if(pmic_rsrc_data == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else if(vrm_post_initialized == TRUE)
   {
      result = VRM_FAILURE;
   }
   else
   {
      vrm_os_memset(&type_data, 0, sizeof(vrm_rsrc_type_data));
      vrm_os_memset(&rsrc_data, 0, sizeof(vrm_rsrc_data));

      // enable address offset is the only type info needed by all resources
      type_data.enable_addr_offset = pmic_rsrc_data->enable_info.addr_offset;

      if(pmic_rsrc_data->rsrc_category == VRM_RSRC_REGULATOR)
      {
         type_data.mode_addr_offset = pmic_rsrc_data->mode_info.addr_offset;
         result |= vrm_get_time_vrm_cycles(pmic_rsrc_data->mode_info.fixed_time_usec, &time_vrm);
         type_data.mode_fixed_time = time_vrm;
         type_data.volt_addr_offset = pmic_rsrc_data->volt_info.addr_offset;

         // if VREG_READY based settling is used then need following data
         if(pmic_rsrc_data->settling_en)
         {
            type_data.settling_clr_addr_offset = pmic_rsrc_data->settling_clr_addr_offset;
            type_data.settling_clr_data = pmic_rsrc_data->settling_clr_data;
            type_data.settling_wdog_timer = pmic_rsrc_data->settling_wdog_timer;
         }

         if(pmic_rsrc_data->enable_info.fixed_time_usec)
         {
            result |= vrm_get_time_vrm_cycles(pmic_rsrc_data->enable_info.fixed_time_usec, &time_vrm);
            type_data.enable_fixed_time = time_vrm;
         }
         else
         {
            result |= vrm_get_stepper_rate_vrm_cycles(pmic_rsrc_data->enable_stepper_rate_uv_us, &stepper_vrm_mv);
            type_data.enable_stepper_rate = stepper_vrm_mv;

            result |= vrm_get_time_vrm_cycles(pmic_rsrc_data->enable_stepper_warm_up_time_usec, &time_vrm);
            type_data.enable_stepper_warm_up_time = time_vrm;
         }

         if(pmic_rsrc_data->volt_info.fixed_time_usec)
         {
            result |= vrm_get_time_vrm_cycles(pmic_rsrc_data->volt_info.fixed_time_usec, &time_vrm);
            type_data.volt_fixed_time = time_vrm;

            result |= vrm_get_time_vrm_cycles(pmic_rsrc_data->volt_fixed_down_time_usec, &time_vrm);
            type_data.volt_fixed_down_time = time_vrm;
         }
         else
         {
            result |= vrm_get_stepper_rate_vrm_cycles(pmic_rsrc_data->volt_stepper_rate_uv_us, &stepper_vrm_mv);
            type_data.volt_stepper_rate = stepper_vrm_mv;

            result |= vrm_get_time_vrm_cycles(pmic_rsrc_data->volt_stepper_extra_up_time_usec, &time_vrm);
            type_data.volt_stepper_extra_up_time = time_vrm;

            result |= vrm_get_time_vrm_cycles(pmic_rsrc_data->volt_stepper_extra_down_time_usec, &time_vrm);
            type_data.volt_stepper_extra_down_time = time_vrm;

            result |= vrm_get_time_vrm_cycles(pmic_rsrc_data->volt_discharge_time_usec, &time_vrm);
            type_data.volt_discharge_time = time_vrm;
         }
      }

      // if there is an existing type with given type data, this function assigns corresponding type id
      // else creates a new type and returns the new type id
      if(result == VRM_SUCCESS)
      {
         // no vrm types needed for local resources
         if(pmic_rsrc_data->rsrc_category != VRM_RSRC_LOCAL)
         {
            result = vrm_add_rsrc_type(&type_data, &type_id);
         }
      }

      if(result == VRM_SUCCESS)
      {
         // for clocks alone the settling time is part of rsrc cfg and not type cfg
         if(pmic_rsrc_data->rsrc_category != VRM_RSRC_REGULATOR)
         {
            result |= vrm_get_time_vrm_cycles(pmic_rsrc_data->enable_info.fixed_time_usec, &time_vrm);
            rsrc_data.enable_fixed_time = time_vrm;
         }

         rsrc_data.pmic_index = pmic_rsrc_data->periph_info.pmic_index;
         rsrc_data.periph_type = pmic_rsrc_data->periph_info.periph_type;
         rsrc_data.periph_id = pmic_rsrc_data->periph_info.periph_index + 1;
         rsrc_data.arb_id = pmic_rsrc_data->vrm_arb_id;
         rsrc_data.rsrc_category = pmic_rsrc_data->rsrc_category;
         rsrc_data.type_id = type_id;
         rsrc_data.drv_owner_mask = pmic_rsrc_data->drv_owner_mask;
         rsrc_data.volt_current_state = pmic_rsrc_data->volt_info.current_state;
         rsrc_data.enable_current_state = pmic_rsrc_data->enable_info.current_state;
         rsrc_data.mode_current_state = pmic_rsrc_data->mode_info.current_state;
         rsrc_data.volt_min_state = pmic_rsrc_data->volt_info.min_state;
         rsrc_data.enable_min_state = pmic_rsrc_data->enable_info.min_state;
         rsrc_data.mode_min_state = pmic_rsrc_data->mode_info.min_state;
         rsrc_data.hr_min_state = pmic_rsrc_data->hr_info.min_state;
         rsrc_data.volt_max_state = pmic_rsrc_data->volt_max_state;
         rsrc_data.aop_handling = pmic_rsrc_data->aop_handling;
         rsrc_data.settling_en = pmic_rsrc_data->settling_en;
         rsrc_data.settling_err_en = pmic_rsrc_data->settling_err_en;
         rsrc_data.enable_pin_ctrl = pmic_rsrc_data->enable_pin_ctrl;

         result |= vrm_get_seq_id(pmic_rsrc_data->enable_info.seq_type, &seq_id);
         rsrc_data.enable_seq_id = seq_id;

         result |= vrm_get_seq_id(pmic_rsrc_data->mode_info.seq_type, &seq_id);
         rsrc_data.mode_seq_id = seq_id;

         result |= vrm_get_seq_id(pmic_rsrc_data->volt_info.seq_type, &seq_id);
         rsrc_data.volt_seq_id = seq_id;

         result |= vrm_get_seq_id(pmic_rsrc_data->hr_info.seq_type, &seq_id);
         rsrc_data.hr_seq_id = seq_id;

         if(pmic_rsrc_data->dep_en == TRUE)
         {
            rsrc_data.dep_enable = 1;
         }

         if(result == VRM_SUCCESS)
         {
            if(strlen(pmic_rsrc_data->rsrc_name) != 0)
            {
               rsrc_names.name = pmic_rsrc_data->rsrc_name;
            }

            if(strlen(pmic_rsrc_data->rsrc_alias_name) != 0)
            {
               rsrc_names.alias_name = pmic_rsrc_data->rsrc_alias_name;
            }

            if((pmic_rsrc_data->dep_en == TRUE) && (strlen(pmic_rsrc_data->rsrc_parent_name) != 0))
            {
               rsrc_names.parent_name = pmic_rsrc_data->rsrc_parent_name;
            }

            if(pmic_rsrc_data->rsrc_category == VRM_RSRC_LOCAL)
            {
               result = vrm_add_local_rsrc(&rsrc_names, pmic_rsrc_data->rsrc_category, pmic_rsrc_data->base_addr);
            }
            else
            {
               // Assign resource id and configure resource settings in HW
               result = vrm_add_rsrc(&rsrc_names, &rsrc_data);
            }
         }
      }
   }

   CORE_VERIFY(result == VRM_SUCCESS);

   return result;
}
                            
/**
 * @brief VRM post initialization for enabling VRM and 
 *        completion of command DB init. This function needs to
 *        be called at the end after registering all the
 *        resources.
 *
 * This function completes the command DB initialization and 
 * enables the VRM. This function also does error checking 
 * mainly to ensure correct parent-child dependency mapping. 
 * 
 * @param None
 *
 * @return  VRM_SUCCESS on success, error code on error
 * 
 * @see vrm_init, vrm_register_rsrc
 */
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_post_init()
{
   vrm_result result = VRM_SUCCESS;

   result = vrm_driver_post_init();

   CORE_VERIFY(result == VRM_SUCCESS);

   vrm_post_initialized = TRUE;

   return result;
}
                            
/**
 * @brief This is OS specific functionality. On supported OS, 
 *        VRM function call to create a task which handles VRM
 *        IRQ and this task will be signalled from VRM ISR when
 *        there is a VRM interrupt triggered.
 * 
 * @param None
 *
 * @return None
 */
void vrm_task_init()
{
   vrm_result result = VRM_SUCCESS;

   result = vrm_os_create_task();

   CORE_VERIFY(result == VRM_SUCCESS);

   vrm_task_initialized = TRUE;
}
                            
/**
 * @brief Sends the vote on requested resource setting and data 
 *        through rpmh client driver after checking for minimum
 *        requirements on the resource.
 * 
 * @param[in] rsrc_vote Pointer to resource vote info like id, 
 *                      setting, data, rpmh set and completion.
 * 
 * @param[out] msg_id rpmh client driver message id after 
 *                    sending the vote.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_send_rpmh_vote(vrm_rsrc_vote_data* rsrc_vote, uint32 *msg_id)
{
   vrm_result result = VRM_SUCCESS;

   result = vrm_send_vote(rsrc_vote, msg_id);

   return result;
}
                            
/**
 * @brief Returns VRM resource id for given resource category 
 *        type and vrm address offset.
 * 
 * @param[in] rsrc_category Resource category REG, XOB or XO 
 * 
 * @param[in] vrm_addr_offset VRM address offset returned from 
 *                            cmd db.
 * 
 * @param[out] rsrc_id VRM resource id corresponding to the VRM 
 *                     address offset.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */

vrm_result vrm_get_rsrc_id(vrm_rsrc_category rsrc_category, uint32 vrm_addr_offset, uint32 *rsrc_id)
{
   vrm_result result = VRM_SUCCESS;

   if (rsrc_id == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else if (rsrc_category == VRM_RSRC_REGULATOR)
   {
      *rsrc_id = ((vrm_addr_offset >> 8) & 0xFF);
   }
   else
   {
      result = VRM_INVALID_PARAM;
   }

   return result;
}
                            




