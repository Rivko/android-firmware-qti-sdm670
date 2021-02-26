/*! \file vrm_driver.c
*
*  \brief Implementation file for generic VRM driver and also routing layer.
*  \n
*  &copy; Copyright 2016 - 2018 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/src/vrm_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_driver.h"
#include "vrm_vreg.h"
#include "vrm_xob.h"
#include "vrm_hal.h"
#include "vrm_os.h"
#include "vrm_config.h"
#include "vrm_target.h"
#include "vrm_cmd_db.h"
#include "rinit.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

//******************************************************************************
// Global Data
//******************************************************************************

vrm_info vrm = {0};

static vrm_os_lock_type vrm_drv_lock;
static uint8 vrm_drv_lock_cnt = 0;

//******************************************************************************
// Local Helper Functions
//******************************************************************************

/**
 * @brief VRM ISR for all the VRM related IRQs.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
static void vrm_isr(int irq)
{
   // disables all the vrm irqs and signals VRM task
   vrm_os_isr(irq);
}

/**
 * @brief Enters vrm driver critical section
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
static vrm_result vrm_driver_lock()
{
   vrm_result result = VRM_SUCCESS;

   result = vrm_os_lock(&vrm_drv_lock);
   vrm_drv_lock_cnt++;

   return result;
}

/**
 * @brief Leaves vrm driver critical section
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
static vrm_result vrm_driver_unlock()
{
   vrm_result result = VRM_SUCCESS;

   vrm_drv_lock_cnt--;
   CORE_VERIFY(vrm_drv_lock_cnt >= 0);

   result = vrm_os_unlock(&vrm_drv_lock);

   return result;
}
                            
/**
 * @brief Handles VRM related dependency interrupts.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
static vrm_result vrm_dep_irq_handler(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;
   uint64 dep_irq_status[VRM_NUM_SETTINGS] = {0};
   uint64 dep_irq_enable[VRM_NUM_SETTINGS] = {0};
   uint32 setting = 0;
   boolean dep_irq_status_flag = FALSE;

   // Read dep irq status registers per setting in the specific order
   for (setting = 0; setting < VRM_NUM_SETTINGS; setting++) 
   {
      result |= vrm_hal_interrupt_dep_irq_status_read(vrm_ptr, setting, &dep_irq_status[setting]);
      result |= vrm_hal_interrupt_dep_irq_enable_read(vrm_ptr, setting, &dep_irq_enable[setting]);

      dep_irq_status[setting] = (dep_irq_status[setting] & dep_irq_enable[setting]);

      if (dep_irq_status[setting] != 0)
      {
         // set the flag if any dep irq is triggered
         dep_irq_status_flag = TRUE;
      }
   }

   // if no dep irq is triggered, then return
   if (dep_irq_status_flag == FALSE)
   {
      return result;
   }

   result |= vrm_target_dep_irq_handler(vrm_ptr, dep_irq_status, VRM_NUM_SETTINGS);
   result |= vrm_vreg_dep_irq_handler(vrm_ptr, dep_irq_status, VRM_NUM_SETTINGS);

   return result;
}
                            
/**
 * @brief Handles VRM related change interrupts.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
static vrm_result vrm_change_irq_handler(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;
   uint64 change_irq_status[VRM_NUM_SETTINGS] = {0};
   uint64 change_irq_enable[VRM_NUM_SETTINGS] = {0};
   uint32 setting = 0;
   boolean change_irq_status_flag = FALSE;

   // Read change irq status registers per setting in the specific order
   for (setting = 0; setting < VRM_NUM_SETTINGS; setting++) 
   {
      result |= vrm_hal_interrupt_change_irq_status_read(vrm_ptr, setting, &change_irq_status[setting]);
      result |= vrm_hal_interrupt_change_irq_enable_read(vrm_ptr, setting, &change_irq_enable[setting]);

      change_irq_status[setting] = (change_irq_status[setting] & change_irq_enable[setting]);

      if (change_irq_status[setting] != 0)
      {
         // set the flag if any change irq is triggered
         change_irq_status_flag = TRUE;
      }
   }

   // if no change irq is triggered, then return
   if (change_irq_status_flag == FALSE)
   {
      return result;
   }

   result |= vrm_target_change_irq_handler(vrm_ptr, change_irq_status, VRM_NUM_SETTINGS);
   result |= vrm_vreg_change_irq_handler(vrm_ptr, change_irq_status, VRM_NUM_SETTINGS);

   return result;
}
                            
/**
 * @brief Handles VRM related settled interrupts.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
static vrm_result vrm_settled_irq_handler(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;
   uint64 settled_irq_status[VRM_NUM_SETTINGS] = {0};
   uint64 settled_irq_enable[VRM_NUM_SETTINGS] = {0};
   uint32 setting = 0;
   boolean settled_irq_status_flag = FALSE;

   // Read change irq status registers per setting in the specific order
   for (setting = 0; setting < VRM_NUM_SETTINGS; setting++) 
   {
      result |= vrm_hal_interrupt_settled_irq_status_read(vrm_ptr, setting, &settled_irq_status[setting]);
      result |= vrm_hal_interrupt_settled_irq_enable_read(vrm_ptr, setting, &settled_irq_enable[setting]);

      settled_irq_status[setting] = (settled_irq_status[setting] & settled_irq_enable[setting]);

      if (settled_irq_status[setting] != 0)
      {
         // set the flag if any vreg change irq is triggered
         settled_irq_status_flag = TRUE;
      }
   }

   // if no settled irq is triggered, then return
   if (settled_irq_status_flag == FALSE)
   {
      return result;
   }

   result |= vrm_target_settled_irq_handler(vrm_ptr, settled_irq_status, VRM_NUM_SETTINGS);
   result |= vrm_vreg_settled_irq_handler(vrm_ptr, settled_irq_status, VRM_NUM_SETTINGS);

   return result;
}
                            
/**
 * @brief Handles VRM settling timeout (VREG_ACK/CLK_ACK) error 
 *        interrupt and calls internal functions to poll for
 *        resource VREG_READY/CLK_OK status.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
static vrm_result vrm_error_irq_settling_timeout(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;
   uint64 error_irq_status = 0;
   uint64 error_irq_enable = 0;
   uint64 bit_mask = 0x1;
   uint32 wrs_data = 0;
   uint32 crs_data = 0;
   uint32 seq_id = 0;
   uint32 slave_id = 0;
   boolean status = FALSE;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);
   vrm_hal_seq_data_type seq_data;


   vrm_hal_interrupt_settling_wdog_irq_status_read(vrm_ptr, &error_irq_status);
   vrm_hal_interrupt_settling_wdog_irq_enable_read(vrm_ptr, &error_irq_enable);

   error_irq_status = (error_irq_status & error_irq_enable);

   // if no vreg ready error irq is triggered, then return
   if (error_irq_status == 0)
   {
      return result;
   }

   for (seq_id = 0; seq_id < VRM_MAX_NUM_SEQCERS; seq_id++)
   {
      if ((error_irq_status>>seq_id) & bit_mask)
      {
         vrm_os_memset(&seq_data, 0, sizeof(vrm_hal_seq_data_type));

         result |= vrm_hal_seq_data_read(vrm_ptr, seq_id, &seq_data);

         if (seq_data.rsrc_category == VRM_RSRC_REGULATOR)
         {
            // settling errors are aborted internally
            vrm_vreg_rsrc_settle(&vrm, seq_data.rsrc_id, seq_data.settling_time_usec);
         }

         // apply the work-around to unblock the VRM sequencer

         // disable WDOG enable for that sequencer
         result |= vrm_hal_settling_wdog_error_enable(vrm_ptr, seq_id, FALSE);

         // clear WDOG interrupt for that sequencer
         result |= vrm_hal_interrupt_settling_wdog_irq_clear(vrm_ptr, seq_id);

         // enable WDOG enable for that sequencer
         result |= vrm_hal_settling_wdog_error_enable(vrm_ptr, seq_id, TRUE);

         CORE_VERIFY(result == VRM_SUCCESS);

      }
   }

   return result;
}
                            
/**
 * @brief Checks for VRM CTT overflow error interrupt and 
 *        aborts.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
static vrm_result vrm_error_irq_ctt_overflow(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;
   uint32 error_irq_status = 0;

   vrm_hal_interrupt_ctt_overflow_irq_status_read(vrm_ptr, &error_irq_status);

   // if ctt overflow error irq is triggered, then abort
   if (error_irq_status != 0)
   {
      result = VRM_FAILURE;

      vrm_log(vrm_ptr, VRM_RSRC_INVALID_CATEGORY, VRM_NUM_VREG, VRM_NUM_SETTINGS, 
              &error_irq_status, result, VRM_EVENT_CTT_OVERFLOW);

      CORE_VERIFY(0);
   }

   return result;
}
                            
/**
 * @brief Checks for VRM PMIC_ARB S_PORT failure interrupt and 
 *        aborts.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
static vrm_result vrm_error_irq_pmic_arb_failure(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;
   uint32 error_irq_status = 0;

   vrm_hal_interrupt_pmic_arb_failure_irq_status_read(vrm_ptr, &error_irq_status);

   // if pmic_arb s_port failure irq is triggered, then abort
   if (error_irq_status != 0)
   {
      result = VRM_FAILURE;

      vrm_log(vrm_ptr, VRM_RSRC_INVALID_CATEGORY, VRM_NUM_VREG, VRM_NUM_SETTINGS, 
              &error_irq_status, result, VRM_EVENT_PMIC_ARB_FAILURE);

      CORE_VERIFY(0);
   }

   return result;
}
                            
/**
 * @brief Handles VRM related error interrupts.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
static vrm_result vrm_error_irq_handler(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;

   result |= vrm_error_irq_settling_timeout(&vrm);

   result |= vrm_error_irq_ctt_overflow(&vrm);

   result |= vrm_error_irq_pmic_arb_failure(&vrm);

   result |= vrm_vreg_error_irq_handler(&vrm);

   return result;
}

/**
 * @brief VRM IRQ initialization which mainly calls OS specific 
 *        register ISR which registers given isr to all the VRM
 *        irqs and enables the VRM irqs in HW.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_irq_init()
{
   vrm_result result = VRM_SUCCESS;

   result |= vrm_os_register_isr(vrm_isr);

   return result;
}

/**
 * @brief VRM RSRC data initialization to initialize driver 
 *        structures with data needed in mission mode.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_rsrc_data_init(vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;

   if (rsrc_data->rsrc_category == VRM_RSRC_REGULATOR)
   {
      result = vrm_vreg_data_init(&vrm, rsrc_data);
   }

   return result;
}

//******************************************************************************
// API Functions
//******************************************************************************
/**
 * @brief Initializes the VRM driver.
 *
 * This function initializes the VRM driver structures/config data 
 * and calls VRM HAL initialization function to do basic VRM HW 
 * configuration and VRM sequence mem configuration. Also initializes 
 * command DB to add VRM resource entries.
 * 
 * @param None
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_driver_init()
{
   vrm_result result = VRM_SUCCESS;
   cmd_db_err_type cmd_db_err = CMD_DB_SUCCESS;
   int ret_val = RINIT_SUCCESS;

   if(!vrm.initialized)
   {
      ret_val = rinit_module_init ("vrm");
      if (ret_val != RINIT_SUCCESS)
      {
         result |= VRM_RINIT_ERROR;
      }

      vrm.config = vrm_get_config_data();

      if (vrm.config != NULL)
      {
         result |= vrm_hal_init(&vrm);
         result |= vrm_irq_init();
         result |= vrm_os_lock_init(&vrm_drv_lock);

         // initialize command db for VRM
         result |= vrm_cmd_db_init(&vrm);

         vrm.rpmh_handle = rpmh_create_handle (RSC_DRV_AOP, "vrm_client");
      }
      else
      {
         result = VRM_NULL_PTR_ERROR;
      }

      if(result == VRM_SUCCESS)
      {
         vrm.initialized = TRUE;
      }
   }

   if(result != VRM_SUCCESS)
   {
      vrm_log(&vrm, VRM_RSRC_INVALID_CATEGORY, VRM_NUM_VREG, VRM_NUM_SETTINGS, 
              NULL, result, VRM_EVENT_INIT);
   }

   return result;
}

/**
 * @brief VRM post driver initialization for enabling VRM and 
 *        command DB initialization.
 *
 * This function initializes command DB and enables the VRM. 
 * Command DB is initialized with the registered PMIC resource 
 * names and corresponding offset data. This function also does 
 * error checking mainly to ensure correct parent-child 
 * dependency mapping.  
 * 
 * @param None
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_driver_post_init()
{
   vrm_result result = VRM_SUCCESS;
   cmd_db_err_type cmd_db_err = CMD_DB_SUCCESS;

   if(!vrm.initialized)
   {
      result = VRM_NOT_INITIALIZED;
   }
   else if(!vrm.post_initialized)
   {
      result |= vrm_vreg_check_post_dep_error(&vrm);

      // complete command db initialization for VRM
      result |= vrm_cmd_db_post_init(&vrm);

      result |= vrm_target_post_init(&vrm);

      if (result == VRM_SUCCESS)
      {
         // enable VRM
         result |= vrm_hal_cfg_vrm_enable(&vrm, vrm.config->vrm_slave_id);
      }

      if (vrm.spmi_info.spmi_flag == TRUE)
      {
         vrm.spmi_info.rpmh_handle = rpmh_create_handle (RSC_DRV_AOP, "spmi_client");
      }

      if(result == VRM_SUCCESS)
      {
         vrm.post_initialized = TRUE;
      }
   }

   if(result != VRM_SUCCESS)
   {
      vrm_log(&vrm, VRM_RSRC_INVALID_CATEGORY, VRM_NUM_VREG, VRM_NUM_SETTINGS, 
              NULL, result, VRM_EVENT_POST_INIT);
   }

   return result;
}
                            
/**
 * @brief Assigns resource id for requested VRM resource.
 * 
 * @param[in] rsrc_category rsrc category indicating whether 
 *                          rsrc is VREG or XOB.
 * 
 * @param[in] rsrc_names rsrc names struct contains the rsrc 
 *                       name, rsrc alias name if any and rsrc
 *                       parent name if any.
 * 
 * @param[in] num_children Number of children if rsrc has any.
 * 
 * @param[out] rsrc_id assigned rsrc id is returned.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_assign_rsrc_id(vrm_rsrc_category rsrc_category, vrm_rsrc_names* rsrc_names, uint32 num_children, uint32* rsrc_id)
{                                               
   vrm_result result = VRM_SUCCESS;

   if(!vrm.initialized)
   {
      result = VRM_NOT_INITIALIZED;
   }
   else if (rsrc_names == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else if (rsrc_category == VRM_RSRC_REGULATOR)
   {
      result = vrm_vreg_assign_rsrc_id(&vrm, rsrc_names, num_children, rsrc_id);
   }
   else if (rsrc_category == VRM_RSRC_XO_BUFFER)
   {
      result = vrm_xob_assign_rsrc_id(&vrm, rsrc_names, num_children, rsrc_id);
   }
   else
   {
      result = VRM_INVALID_PARAM;
   }

   if(result != VRM_SUCCESS)
   {
      vrm_log(&vrm, rsrc_category, *rsrc_id, VRM_NUM_SETTINGS, NULL, result, VRM_EVENT_ASSIGN_RSRC_ID);
   }

   return result;
}
                            
/**
 * @brief Assigns type id for requested VRM resource and 
 *        configures VRM HW TYPECFG for corresponding type.
 *
 * This function returns an existing type id if there is an 
 * existing type with given type data else assigns a new type id
 * and configures the VRM HW TYPECFG with the new type 
 * information. 
 * 
 * @param[in] type_data pointer to rsrc type data 
 *  
 * @param[out] type_id assigned type id is returned 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_add_rsrc_type(vrm_rsrc_type_data* type_data, uint32* type_id)
{
   vrm_result result = VRM_SUCCESS;
   int type_data_cmp = 1;
   uint32 index = 0;

   if(!vrm.initialized)
   {
      result = VRM_NOT_INITIALIZED;
   }
   else if((type_data == NULL) || (type_id == NULL))
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else if (type_data->settling_wdog_timer >= VRM_INVALID_SETTLING_TIMER)
   {
      result = VRM_INVALID_PARAM;
   }
   else
   {
      while(index < VRM_NUM_RSRC_TYPE)
      {
         if(vrm.type_arr[index].type_id_assigned == FALSE)
         {
            break;
         }

         // set the flag just for mem compare and clear it later
         type_data->type_id_assigned = 1;
         type_data_cmp = vrm_os_memcmp(&vrm.type_arr[index], type_data, sizeof(vrm_rsrc_type_data));
         type_data->type_id_assigned = 0;

         if(type_data_cmp == 0)
         {
            *type_id = index;
            break;
         }
         index = index + 1;
      }

      if (index >= VRM_NUM_RSRC_TYPE)
      {
         result = VRM_OUT_OF_BOUND_ERROR;
      }
      else if (type_data_cmp != 0)
      {
         // assign new type id if it doesn't exist
         *type_id = index;
         type_data->type_id_assigned = 1;
         vrm.type_arr[index] = *type_data;

         result = vrm_hal_typecfg_init(&vrm, *type_id, type_data);
      }
   }

   if(result != VRM_SUCCESS)
   {
      vrm_log(&vrm, VRM_RSRC_INVALID_CATEGORY, VRM_NUM_VREG, VRM_NUM_SETTINGS, 
              NULL, result, VRM_EVENT_ADD_RSRC_TYPE);
   }

   return result;
}

                            
/**
 * @brief Gets rsrc id and configures the VRM HW for the given 
 *        resource.
 *
 * This function gets the resource id from vrm_assign_rsrc_id 
 * function and configures VRM RESCFG and VOTETABLE in the HW 
 * with given resource data.
 * 
 * @param[in] rsrc_names rsrc names struct contains the rsrc 
 *                       name, rsrc alias name if any and rsrc
 *                       parent name if any. 
 * 
 * @param[in] rsrc_data pointer to rsrc data 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_add_rsrc(vrm_rsrc_names* rsrc_names, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;
   uint32 rsrc_id = 0;
   vrm_rsrc_category rsrc_category = VRM_RSRC_INVALID_CATEGORY;

   if(!vrm.initialized)
   {
      result = VRM_NOT_INITIALIZED;
   }
   else if((rsrc_names == NULL) || (rsrc_data == NULL))
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      rsrc_category = (vrm_rsrc_category)rsrc_data->rsrc_category;

      // Should assign a new rsrc id if not assigned already
      result = vrm_assign_rsrc_id(rsrc_category, rsrc_names, 0, &rsrc_id);

      if (result == VRM_SUCCESS)
      {
         rsrc_data->rsrc_id = rsrc_id;

         // initialize resource data and votetable in HW
         result = vrm_hal_rescfg_init(&vrm, rsrc_data);
         result |= vrm_hal_votetable_init(&vrm, rsrc_data);
         result |= vrm_hal_interrupt_init(&vrm, rsrc_data);

         // save current resource states only if HW init is success
         if (result == VRM_SUCCESS)
         {
            result = vrm_rsrc_data_init(rsrc_data);
         }
      }
   }

   if(result != VRM_SUCCESS)
   {
      vrm_log(&vrm, rsrc_category, rsrc_id, VRM_NUM_SETTINGS, NULL, result, VRM_EVENT_ADD_RSRC);
   }

   return result;
}
                            
/**
 * @brief Adds locally controlled VRM resource to the VRM CMD 
 *        DB.
 * 
 * @param[in] rsrc_names rsrc names struct contains the rsrc 
 *                       name, rsrc alias name if any and rsrc
 *                       parent name if any. 
 * 
 * @param[in] rsrc_category rsrc category
 * 
 * @param[in] base_addr PMIC base address of the resource
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_add_local_rsrc(vrm_rsrc_names* rsrc_names, vrm_rsrc_category rsrc_category, uint32 base_addr)
{
   vrm_result result = VRM_SUCCESS;

   if(!vrm.initialized)
   {
      result = VRM_NOT_INITIALIZED;
   }
   else if(rsrc_names == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else if((rsrc_category != VRM_RSRC_LOCAL) || (base_addr == 0))
   {
      result = VRM_INVALID_PARAM;
   }
   else
   {
      result = vrm_cmd_db_add_entry(&vrm, VRM_RSRC_CMD_DB_LOCAL, rsrc_names->name, base_addr);

      if ((result == VRM_SUCCESS) && (rsrc_names->alias_name != NULL))
      {
         result = vrm_cmd_db_add_entry(&vrm, VRM_RSRC_CMD_DB_LOCAL, rsrc_names->alias_name, base_addr);
      }
   }

   if(result != VRM_SUCCESS)
   {
      vrm_log(&vrm, rsrc_category, base_addr, VRM_NUM_SETTINGS, NULL, result, VRM_EVENT_ADD_RSRC);
   }

   return result;
}
                            
/**
 * @brief Returns sequence id from config for the given sequence 
 *        type.
 * 
 * @param[in] seq_type sequence type for which sequence id is 
 *                     requested
 * 
 * @param[out] seq_id sequence id is returned from the config 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_get_seq_id(vrm_seq_type seq_type, uint32* seq_id)
{
   vrm_result result = VRM_SUCCESS;

   if(!vrm.initialized)
   {
      result = VRM_NOT_INITIALIZED;
   }
   else if (seq_id == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else if (seq_type >= VRM_MAX_SEQ)
   {
      result = VRM_OUT_OF_BOUND_ERROR;
   }
   else if (vrm.config->seq_id_arr[seq_type] >= VRM_INVALID_SEQ_ID)
   {
      result = VRM_SEQ_NOT_SUPPORTED;
   }
   else
   {
      *seq_id = vrm.config->seq_id_arr[seq_type];
   }

   return result;
}
                            
/**
 * @brief VRM IRQ handler called from REX task. This is mainly a 
 *        routing function to specific IRQ handler
 *        implementations.
 * 
 * @param[in] irq interrupt which needs to be handled.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_irq_handler(vrm_irq_num irq)
{
   vrm_result result = VRM_SUCCESS;

   // lock to synchronize with send vote
   vrm_driver_lock();

   vrm.irq_info.irq_arr_idx = vrm.irq_info.irq_arr_idx + 1;

   if (vrm.irq_info.irq_arr_idx >= VRM_MAX_IRQ_ARR_SIZE)
   {
      vrm.irq_info.irq_arr_idx = 0;
   }

   vrm.irq_info.irq_num_arr[vrm.irq_info.irq_arr_idx] = irq;

   // Error interrupt handler should run first, if an error interrupt is triggered
   if (irq == VRM_IRQ_ERROR)
   {
      result |= vrm_error_irq_handler(&vrm);
   }

   result |= vrm_dep_irq_handler(&vrm);

   result |= vrm_change_irq_handler(&vrm);

   result |= vrm_settled_irq_handler(&vrm);

   // Error interrupt handler can run last, if an error interrupt is not triggered
   if (irq != VRM_IRQ_ERROR)
   {
      result |= vrm_error_irq_handler(&vrm);
   }

   // unlock
   vrm_driver_unlock();

   return result;

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
vrm_result vrm_send_vote(vrm_rsrc_vote_data* rsrc_vote, uint32 *msg_id)
{
   vrm_result result = VRM_SUCCESS;

   // lock to synchronize with irq handler
   vrm_driver_lock();

   if ((rsrc_vote == NULL) || (msg_id == NULL))
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else if (rsrc_vote->rsrc_category == VRM_RSRC_REGULATOR)
   {
      vrm_vreg_send_vote(&vrm, rsrc_vote, msg_id, TRUE);
   }
   else
   {
      result = VRM_INVALID_PARAM;
   }

   // unlock
   vrm_driver_unlock();

   return result;
}
                            
                           
/**
 * @brief Updates ARC DRV mode vote in VRM votetable for the 
 *        given resource.
 * 
 * @param[in] rsrc_name: VRM resource name (used in CmdDB) 
 * 
 * @param[in] mode: Mode value to be updated 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_arc_drv_mode_update(const char* rsrc_name, uint32 mode)
{
   vrm_result result = VRM_SUCCESS;
   uint32 vrm_address = 0;
   uint32 rsrc_id = 0;
   uint32 drv_id = 0;

   if (rsrc_name == NULL)
   {
      return VRM_NULL_PTR_ERROR;
   }

   vrm_address = cmd_db_query_addr(rsrc_name);

   if (vrm_address == 0)
   {
      return VRM_CMD_DB_ERROR;
   }

   result = vrm_get_rsrc_id(VRM_RSRC_REGULATOR, vrm_address, &rsrc_id);

   if (result == VRM_SUCCESS)
   {
      if(DRV_ID_LOOKUP_SUCCESS == get_version_drv_id(RSC_DRV_GLOBAL_ARC_CPRF, &drv_id))
      {
         result = vrm_hal_votetable_vreg_mode_update(&vrm, drv_id, rsrc_id, mode);
      }
      else
      {
         result = VRM_FAILURE;
      }
   }

   return result;

}
