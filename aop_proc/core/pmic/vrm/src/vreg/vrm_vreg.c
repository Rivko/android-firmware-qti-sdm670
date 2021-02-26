/*! \file vrm_vreg.c
*
*  \brief Implementation file for VRM VREG driver.
*  \n
*  &copy; Copyright 2016 - 2018 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/src/vreg/vrm_vreg.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_vreg.h"
#include "vrm_driver.h"
#include "vrm_hal.h"
#include "vrm_utils.h"
#include "vrm_cmd_db.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define VREG_DATA(vreg_id,setting) vreg_ptr->vreg[vreg_id].vote_data[setting]
#define VREG_DEP(vreg_id)          vreg_ptr->dep_info[vreg_id].dep_data

//******************************************************************************
// Global Data
//******************************************************************************

//******************************************************************************
// Local Helper Functions
//******************************************************************************
                            
/**
 * @brief Adds VRM VREG related resource info to the command DB.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 * 
 * @param[in] name VRM VREG resource name  
 * 
 * @param[in] rsrc_id VRM VREG resource id.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_vreg_add_cmd_db_entry(vrm_info* vrm_ptr, const char* name, uint32 rsrc_id)
{
   vrm_result result = VRM_SUCCESS;
   uint32 addr_offset = 0;

   // add command db entry
   result = vrm_hal_votetable_vreg_get_addr_offset(vrm_ptr, rsrc_id, VRM_VOLTAGE, vrm_ptr->config->vrm_slave_id, &addr_offset);

   if (result == VRM_SUCCESS)
   {
      result = vrm_cmd_db_add_entry(vrm_ptr, VRM_RSRC_CMD_DB_REG, name, addr_offset);
   }

   return result;
}

// update resource voting address offsets for each setting
__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_vreg_update_voting_addr_offset(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 vrm_slave_id)
{
   vrm_result result = VRM_SUCCESS;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);
   uint32 addr_offset = 0;
   uint32 setting = 0;

   for (setting = 0; setting < VRM_NUM_SETTINGS; setting++)
   {
      result = vrm_hal_votetable_vreg_get_addr_offset(vrm_ptr, rsrc_id, setting, vrm_slave_id, &addr_offset);

      if (result != VRM_SUCCESS)
      {
         break;
      }

      VREG_DATA(rsrc_id,setting).addr_offset = addr_offset;
   }

   return result;
}

static vrm_rsrc_vote_data vrm_internal_vote;
static vrm_result vrm_vreg_send_internal_vote(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, uint32 data, boolean completion)
{
   vrm_result result = VRM_SUCCESS;
   uint32 msg_id = 0;

   vrm_os_memset(&vrm_internal_vote, 0, sizeof(vrm_rsrc_vote_data));

   vrm_internal_vote.rpmh_handle = vrm_ptr->rpmh_handle;
   vrm_internal_vote.rsrc_category = VRM_RSRC_REGULATOR;
   vrm_internal_vote.set_type = RPMH_SET_ACTIVE;
   vrm_internal_vote.completion = completion;
   vrm_internal_vote.num_votes = 1;
   vrm_internal_vote.vote_data[0].rsrc_id = rsrc_id;
   vrm_internal_vote.vote_data[0].setting = setting;
   vrm_internal_vote.vote_data[0].data = data;

   // send the active set vote 
   result = vrm_vreg_send_vote(vrm_ptr, &vrm_internal_vote, &msg_id, FALSE);
   rpmh_barrier_single(vrm_ptr->rpmh_handle, msg_id);

   vrm_log(vrm_ptr, VRM_RSRC_REGULATOR, rsrc_id, setting, &data, result, VRM_EVENT_SEND_VOTE);

   return result;
}

// internal function to trigger PBS sequence using VRM PBS resource
static vrm_result vrm_vreg_pbs_sw_trigger(vrm_info* vrm_ptr, uint32 trig_arg)
{
   vrm_result result = VRM_SUCCESS;
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   vrm_soc_info* soc_ptr = &(vrm_ptr->soc_info);
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);
   pm_vrm_periph_info periph_info;
   pm_vrm_pbs_seq_status seq_status;

   periph_info.pmic_index = vreg_ptr->vreg[soc_ptr->pbs_rsrc_id].pmic_index;
   periph_info.periph_type = vreg_ptr->vreg[soc_ptr->pbs_rsrc_id].periph_type;
   periph_info.periph_index = vreg_ptr->vreg[soc_ptr->pbs_rsrc_id].periph_id - 1;

   seq_status.pmic_index = 0;
   seq_status.ret_val = 0;

   // trigger PBS sequence using VRM PBS resource
   err_flag = pm_app_vrm_pbs_sw_trigger(&periph_info, trig_arg, &seq_status);

   if (err_flag != PM_ERR_FLAG_SUCCESS)
   {
      result |= VRM_FAILURE;
   }

   vrm_log(vrm_ptr, VRM_RSRC_REGULATOR, soc_ptr->pbs_rsrc_id, VRM_ENABLE, &trig_arg, result, VRM_EVENT_PBS_SW_TRIGGER);

   return result;
}

static vrm_result vrm_vreg_unblock_parent_votes(vrm_info* vrm_ptr, uint32 parent, uint32 num_settings)
{
   vrm_result result = VRM_SUCCESS;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);
   vrm_vreg_vote_data* data_ptr = NULL;
   vrm_vreg_dep_data* dep_ptr = VREG_DEP(parent);
   uint32 child = 0;
   uint32 setting = 0;

   // check for parent rail setting on which the aggregation changed and send the vote
   for (setting = 0; setting < (num_settings-1); setting++)
   {
      data_ptr = &(VREG_DATA(parent,setting));

      data_ptr->active_children_mask = 0;

      if (data_ptr->curr_agg != data_ptr->prev_agg)
      {
         // send the parent vote 
         vrm_vreg_send_internal_vote(vrm_ptr, parent, setting, data_ptr->curr_agg, FALSE);
      }
   }

   // enable the dep irq for all the children 
   for (child = dep_ptr->child_start_id; child <= dep_ptr->child_end_id; child++)
   {
      for (setting = 0; setting < (num_settings-1); setting++)
      {
         vrm_hal_interrupt_dep_irq_enable(vrm_ptr, child, setting, TRUE);
      }
   }

   dep_ptr->wait_for_children = 0;
   dep_ptr->active_parent = 0;

   return result;
}

static vrm_result vrm_vreg_unblock_children_votes(vrm_info* vrm_ptr, uint32 parent, uint32 num_settings)
{
   vrm_result result = VRM_SUCCESS;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);
   vrm_vreg_vote_data* data_ptr = NULL;
   vrm_vreg_dep_data* dep_ptr = VREG_DEP(parent);
   uint32 child = 0;
   uint32 setting = 0;

   // check for child rail setting on which the dep irq status was triggered
   for (child = dep_ptr->child_start_id; child <= dep_ptr->child_end_id; child++)
   {
      for (setting = 0; setting < (num_settings-1); setting++)
      {
         data_ptr = &(VREG_DATA(child,setting));

         if (data_ptr->dep_irq_status)
         {
            // unblock the child vote at cmd syn
            vrm_hal_cfg_unblock_cmd_syn(vrm_ptr, child, setting);

            data_ptr->dep_irq_status = 0;
         }
      }
   }

   // if parent completion, enable the dep irq for all the children 
   for (child = dep_ptr->child_start_id; child <= dep_ptr->child_end_id; child++)
   {
      for (setting = 0; setting < (num_settings-1); setting++)
      {
         vrm_hal_interrupt_dep_irq_enable(vrm_ptr, child, setting, TRUE);
      }
   }

   dep_ptr->wait_for_parent = 0;
   dep_ptr->active_parent = 0;

   return result;
}

static uint32 vrm_vreg_get_child_req(vrm_info* vrm_ptr, uint32 child, uint32 setting)
{
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);
   uint32 voltage = 0;
   uint32 headroom = 0;

   if (setting == VRM_ENABLE)
   {
      return VREG_DATA(child,setting).ars_data;
   }
   else if (setting == VRM_VOLTAGE)
   {

      // Consider the voltage contribution of a child vreg to the parent voltage if the child is
      // either enabled or pin control enabled.
      if ((VREG_DATA(child,VRM_ENABLE).ars_data) || (VREG_DATA(child,VRM_ENABLE).pin_ctrl))
      {
         voltage = VREG_DATA(child,VRM_VOLTAGE).ars_data;
         headroom = MAX(VREG_DATA(child,VRM_HEADROOM).ars_data, VREG_DATA(child,VRM_HEADROOM).min_vote);

         voltage = voltage + headroom;
         return voltage;
      }
      else
      {
         return 0;
      }
   }
   else
   {
      // return 0 for child headroom requests
      return 0;
   }
}

static vrm_result vrm_vreg_agg_children_req_on_parent(vrm_info* vrm_ptr, uint32 parent, uint32 setting)
{
   vrm_result result = VRM_SUCCESS;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);
   vrm_vreg_vote_data* data_ptr = &(VREG_DATA(parent,setting));
   vrm_vreg_dep_data* dep_ptr = VREG_DEP(parent);
   uint32 child_req = 0;
   uint32 parent_vote = 0;
   uint32 child = 0;
   uint32 bob_mode = 0;

   // aggregate all the child requirements on parent
   for (child = dep_ptr->child_start_id; child <= dep_ptr->child_end_id; child++)
   {
      child_req = vrm_vreg_get_child_req(vrm_ptr, child, setting);

      if (parent_vote < child_req) 
      {
         parent_vote = child_req;
      }
   }

   data_ptr->curr_agg = parent_vote;

   return result;
}

static vrm_result vrm_vreg_dep_irq_child_parent_vote(vrm_info* vrm_ptr, uint32 num_settings)
{
   vrm_result result = VRM_SUCCESS;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);
   vrm_vreg_vote_data* data_ptr = NULL;
   vrm_vreg_dep_data* dep_ptr = NULL;
   boolean parent_direction_up = FALSE;
   uint32 parent_current_lvl = 0;
   uint32 crs_data = 0;
   uint32 wrs_data = 0;
   uint32 parent = 0;
   uint32 child = 0;
   int setting = 0;

   // Process the children requirement per settings in the order: headroom, voltage, enable
   // This ensures that the AOP processes voltage (headroom and voltage) request first and 
   // then the enable request which eventually cascades into sending the child voltage 
   // requirement first and then the enable requirement to the PMIC
   for (parent = 0; parent < vreg_ptr->num_parents; parent++) 
   {
      dep_ptr = VREG_DEP(parent);
      parent_direction_up = FALSE;

      if (dep_ptr->dirty_parent)
      {
         // ignoring mode setting so start the loop from (num_settings - 2) based on enum values
         for (setting = (num_settings - 2); setting >= 0; setting--) 
         {
            data_ptr = &(VREG_DATA(parent,setting));
            parent_current_lvl = data_ptr->curr_agg;
            data_ptr->prev_agg = parent_current_lvl;

            // Aggregate the parent requirement
            vrm_vreg_agg_children_req_on_parent(vrm_ptr, parent, setting);

            // get the direction of parent
            if ((setting == VRM_VOLTAGE) || (setting == VRM_ENABLE))
            {
               // if voltage or enable is going up implies parent direction is up
               // Note enable direction over-writes the voltage direction
               if (data_ptr->curr_agg > parent_current_lvl)
               {
                  parent_direction_up = TRUE;
               }
               else if (data_ptr->curr_agg < parent_current_lvl)
               {
                  parent_direction_up = FALSE;
               }
            }
         }

         // if parent direction is up, enable settled interrupt for parent and send parent vote
         // ignoring mode setting so start the loop from (num_settings - 2) based on enum values
         for (setting = (num_settings - 2); setting >= 0; setting--) 
         {
            if (parent_direction_up == TRUE)
            {
               data_ptr = &(VREG_DATA(parent,setting));

               if (data_ptr->curr_agg != data_ptr->prev_agg)
               {
                  // if VRM SOC resource is voted for sleep then do not change the parent mode
                  // Parent mode will be restored back to active mode during VRM SOC active state transition
                  if ((vrm_ptr->soc_info.soc_flag == 1) && (vrm_ptr->soc_info.soc_active == 0) && (setting == VRM_MODE))
                  {
                     continue;
                  }
                  // clear and enable the settled irq interrupt for active parent
                  vrm_hal_interrupt_settled_irq_clear(vrm_ptr, parent, setting);
                  vrm_hal_interrupt_settled_irq_enable(vrm_ptr, parent, setting, TRUE);

                  vrm_hal_rescfg_crs_read(vrm_ptr, parent, setting, &crs_data);
                  data_ptr->crs_data = crs_data;

                  // keep the parent settled interrupt enabled only if current 
                  // crs is less than the current aggregation on which the vote 
                  // is sent from AOP else disable the settled interrupt since 
                  // no settled interrupt would be triggered in such case
                  if (data_ptr->crs_data < data_ptr->curr_agg)
                  {
                     // set the settled interrupt flags for parent
                     data_ptr->settled_irq_enabled = 1;
                     dep_ptr->wait_for_parent = 1;
                  }
                  else
                  {
                     // clear and disable the settled irq interrupt for active parent
                     vrm_hal_interrupt_settled_irq_clear(vrm_ptr, parent, setting);
                     vrm_hal_interrupt_settled_irq_enable(vrm_ptr, parent, setting, FALSE);
                  }

                  vrm_vreg_send_internal_vote(vrm_ptr, parent, setting, data_ptr->curr_agg, FALSE);
               }
            }
         }

         // if parent direction is down, enable change interrupt for children and unblock their votes at cmd syn.
         // ignoring mode setting so end the loop at (num_settings - 2) based on enum values
         for (setting = 0; setting <= (num_settings - 2); setting++) 
         {
            if (parent_direction_up == FALSE)
            {
               for (child = dep_ptr->child_start_id; child <= dep_ptr->child_end_id; child++)
               {
                  data_ptr = &(VREG_DATA(child,setting));

                  if (data_ptr->dep_irq_status)
                  {
                     // Don't enable headroom change interrupt since no PMIC write for headroom
                     if (setting != VRM_HEADROOM)
                     {
                        // clear and enable the change irq interrupt for active children under parent
                        vrm_hal_interrupt_change_irq_clear(vrm_ptr, child, setting);
                        vrm_hal_interrupt_change_irq_enable(vrm_ptr, child, setting, TRUE);

                        vrm_hal_rescfg_wrs_read(vrm_ptr, child, setting, &wrs_data);
                        data_ptr->wrs_data = wrs_data;

                        // keep the child change interrupt enabled only if current 
                        // wrs is less than the current ars on which the vote 
                        // is sent else disable the change interrupt since 
                        // no change interrupt would be triggered in such case
                        if (data_ptr->wrs_data < data_ptr->ars_data)
                        {
                           // set the change interrupt flags for children
                           data_ptr->change_irq_enabled = 1;
                           dep_ptr->wait_for_children = 1;
                        }
                        else
                        {
                           // clear and disable the change irq interrupt for child
                           vrm_hal_interrupt_change_irq_clear(vrm_ptr, child, setting);
                           vrm_hal_interrupt_change_irq_enable(vrm_ptr, child, setting, FALSE);
                        }
                     }

                     // unblock the child vote at cmd syn
                     vrm_hal_cfg_unblock_cmd_syn(vrm_ptr, child, setting);
                     data_ptr->dep_irq_status = 0;
                  }
               }
            }
         }

         // if only headroom setting changed for the children, then send parent votes immediately
         // since no change interrupt is triggered for headroom.
         if ((parent_direction_up == FALSE) && (dep_ptr->wait_for_children == 0))
         {
            vrm_vreg_unblock_parent_votes(vrm_ptr, parent, num_settings);
         }

         // if parent direction is up but the current state of parent is higher already
         // then unblock the children votes immediately since parent already satisfies the child request
         if ((parent_direction_up == TRUE) && (dep_ptr->wait_for_parent == 0))
         {
            vrm_vreg_unblock_children_votes(vrm_ptr, parent, num_settings);
         }

         dep_ptr->dirty_parent = 0;
      }
   }

   return result;
}

static vrm_result vrm_vreg_dep_irq_get_dirty_parents(vrm_info* vrm_ptr, uint64* dep_irq_status, 
                                                     uint64* child_irq_status, uint32 num_settings)
{
   vrm_result result = VRM_SUCCESS;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);
   vrm_vreg_vote_data* data_ptr = NULL;
   vrm_vreg_dep_data* dep_ptr = NULL;
   uint64 bit_mask = 0x1;
   uint64 child_mask = 0;
   uint64 active_children_mask = 0;
   uint32 ars_data = 0;
   uint32 parent = 0;
   uint32 child = 0;
   uint32 setting = 0;

   // check for parent-child tree which has interrupt triggered
   for (parent = 0; parent < vreg_ptr->num_parents; parent++) 
   {
      dep_ptr = VREG_DEP(parent);

      // if there is an active parent being processed, continue
      if (dep_ptr->active_parent)
      {
         continue;
      }

      // check for dirty children and mark the corresponding parent as dirty
      for (setting = 0; setting < (num_settings-1); setting++)
      {
         active_children_mask = (child_irq_status[setting] >> dep_ptr->child_start_id) & dep_ptr->children_mask;

         if (active_children_mask)
         {
            data_ptr = &(VREG_DATA(parent,setting));

            // set the active parent bit
            dep_ptr->active_parent = 1;

            // set the dirty parent bit
            dep_ptr->dirty_parent = 1;

            data_ptr->active_children_mask = active_children_mask;
         }
      }

      if (dep_ptr->dirty_parent)
      {

         for (child = dep_ptr->child_start_id; child <= dep_ptr->child_end_id; child++)
         {
            for (setting = 0; setting < (num_settings-1); setting++)
            {
               // mask the dep irq enable interrupt for all the children under active parent 
               vrm_hal_interrupt_dep_irq_enable(vrm_ptr, child, setting, FALSE);

               data_ptr = &(VREG_DATA(child,setting));

               // only if child dep irq is triggered for a setting, read the ARS data
               if (VRM_READ_BIT(dep_irq_status[setting], child))
               {
                  vrm_hal_rescfg_ars_read(vrm_ptr, child, setting, &ars_data);

                  data_ptr->ars_data = ars_data;

                  data_ptr->dep_irq_status = 1;

                  // clear the dep status bit
                  child_mask = bit_mask << child;
                  dep_irq_status[setting] = (child_mask ^ dep_irq_status[setting]);
               }
            }
         }
      }
   }

   return result;
}

static vrm_result vrm_vreg_dep_irq_soc_handler(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;
   vrm_soc_info* soc_ptr = &(vrm_ptr->soc_info);
   uint32 ars_data = 0;

   vrm_hal_rescfg_ars_read(vrm_ptr, soc_ptr->rsrc_id, VRM_ENABLE, &ars_data);

   if (ars_data)
   {
      // trigger SOC PBS wake sequence
      result = vrm_vreg_pbs_sw_trigger(vrm_ptr, soc_ptr->pbs_wake_arg);

      // Abort if PBS SOC sequence execution failed for some reason
      CORE_VERIFY(result == VRM_SUCCESS);

      soc_ptr->soc_active = 1;
   }
   else
   {

      soc_ptr->soc_active = 0;

      // trigger SOC PBS sleep sequence
      result = vrm_vreg_pbs_sw_trigger(vrm_ptr, soc_ptr->pbs_sleep_arg);

      // Abort if PBS SOC sequence execution failed for some reason
      CORE_VERIFY(result == VRM_SUCCESS);
   }

   // unblock the VRM SOC resource enable vote at cmd syn
   vrm_hal_cfg_unblock_cmd_syn(vrm_ptr, soc_ptr->rsrc_id, VRM_ENABLE);

   return result;
}

static vrm_result vrm_vreg_change_irq_children(vrm_info* vrm_ptr, uint64* change_irq_status, 
                                                      uint32 parent, uint32 num_settings)
{
   vrm_result result = VRM_SUCCESS;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);
   vrm_vreg_vote_data* data_ptr = NULL;
   vrm_vreg_dep_data* dep_ptr = VREG_DEP(parent);
   boolean block_parent = FALSE;
   uint64 bit_mask = 0x1;
   uint64 child_mask = 0;
   uint32 wrs_data = 0;
   uint32 child = 0;
   uint32 setting = 0;

   // check for children rail setting on which the change irq status was triggered
   for (child = dep_ptr->child_start_id; child <= dep_ptr->child_end_id; child++)
   {
      for (setting = 0; setting < (num_settings-1); setting++)
      {
         data_ptr = &(VREG_DATA(child,setting));

         // check the irq status only for the change irq enabled rails
         if (data_ptr->change_irq_enabled)
         {
            // if the change irq status bit is set, check if the child wrs >= ars state
            // if either of the above are not true, keep the parent vote blocked
            if (VRM_READ_BIT(change_irq_status[setting], child))
            {
               // clear the change irq interrupt for active children
               vrm_hal_interrupt_change_irq_clear(vrm_ptr, child, setting);

               vrm_hal_rescfg_wrs_read(vrm_ptr, child, setting, &wrs_data);
               data_ptr->wrs_data = wrs_data;

               // clear the change irq status bit
               child_mask = bit_mask << child;
               change_irq_status[setting] = (child_mask ^ change_irq_status[setting]);

               if (data_ptr->wrs_data >= data_ptr->ars_data)
               {
                  // disable the change irq interrupt for active children
                  vrm_hal_interrupt_change_irq_enable(vrm_ptr, child, setting, FALSE);

                  // clear the change interrupt flags for child
                  data_ptr->change_irq_enabled = 0;
               }
               else
               {
                  block_parent = TRUE;
               }
            }
            else
            {
               block_parent = TRUE;
            }
         }
      }
   }

   // unblock the parent if all the children new aggregation is written to PMIC
   if (block_parent == FALSE)
   {
      vrm_vreg_unblock_parent_votes(vrm_ptr, parent, num_settings);
   }

   return result;
}

static vrm_result vrm_vreg_settled_irq_parent(vrm_info* vrm_ptr, uint64* settled_irq_status, 
                                              uint32 parent, uint32 num_settings)
{
   vrm_result result = VRM_SUCCESS;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);
   vrm_vreg_vote_data* data_ptr = NULL;
   vrm_vreg_dep_data* dep_ptr = VREG_DEP(parent);
   boolean block_children = FALSE;
   uint64 bit_mask = 0x1;
   uint64 parent_mask = 0;
   uint32 crs_data = 0;
   uint32 child = 0;
   uint32 setting = 0;

   // check for parent rail setting on which the change irq status was triggered
   for (setting = 0; setting < (num_settings-1); setting++)
   {
      data_ptr = &(VREG_DATA(parent,setting));

      // check the irq status only for the settled irq enabled rails
      if (data_ptr->settled_irq_enabled)
      {
         // if the settled irq status bit is set, check if the parent current state >= current aggregation
         // if either of the above are not true, keep the children votes blocked
         if (VRM_READ_BIT(settled_irq_status[setting], parent))
         {
            // clear the settled irq interrupt for active parent
            vrm_hal_interrupt_settled_irq_clear(vrm_ptr, parent, setting);

            vrm_hal_rescfg_crs_read(vrm_ptr, parent, setting, &crs_data);
            data_ptr->crs_data = crs_data;

            // clear the settled irq status bit
            parent_mask = bit_mask << parent;
            settled_irq_status[setting] = (parent_mask ^ settled_irq_status[setting]);

            if (data_ptr->crs_data >= data_ptr->curr_agg)
            {
               // disable the settled irq interrupt for active parent
               vrm_hal_interrupt_settled_irq_enable(vrm_ptr, parent, setting, FALSE);

               // clear the settled interrupt flags for parent
               data_ptr->settled_irq_enabled = 0;
            }
            else
            {
               block_children = TRUE;
            }
         }
         else
         {
            block_children = TRUE;
         }
      }
   }

   // unblock the children if the parent's new aggregation is written to PMIC and settled
   if (block_children == FALSE)
   {
      vrm_vreg_unblock_children_votes(vrm_ptr, parent, num_settings);
   }

   return result;
}
                            
/**
 * @brief Checks for VRM resource MAX_VOLTAGE violation error 
 *        interrupt and aborts.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
static vrm_result vrm_vreg_error_irq_max_voltage_violation(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;
   uint64 error_irq_status = 0;
   uint64 error_irq_enable = 0;
   uint32 error_irq_status0 = 0;
   uint32 error_irq_status1 = 0;
   uint32 rsrc_id = 0;

   vrm_hal_interrupt_max_volt_irq_status_read(vrm_ptr, &error_irq_status);
   vrm_hal_interrupt_max_volt_irq_enable_read(vrm_ptr, &error_irq_enable);

   error_irq_status = (error_irq_status & error_irq_enable);

   // if max voltage violation error irq is triggered, then abort
   if (error_irq_status != 0)
   {
      result = VRM_FAILURE;

      while (rsrc_id < VRM_NUM_VREG)
      {
         if ((error_irq_status >> rsrc_id) & 0x1)
         {
            break;
         }
         rsrc_id = rsrc_id + 1;
      }

      error_irq_status0 = (error_irq_status & HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_0_CSR_RMSK);
      error_irq_status1 = ((error_irq_status >> 32) & HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_1_CSR_RMSK);

      if (error_irq_status0 != 0)
      {
         vrm_log(vrm_ptr, VRM_RSRC_REGULATOR, rsrc_id, VRM_VOLTAGE, &error_irq_status0, result, VRM_EVENT_MAX_VOLTAGE_VIOLATION);
      }
      if (error_irq_status1 != 0)
      {
         vrm_log(vrm_ptr, VRM_RSRC_REGULATOR, rsrc_id, VRM_VOLTAGE, &error_irq_status1, result, VRM_EVENT_MAX_VOLTAGE_VIOLATION);
      }
      CORE_VERIFY(0);
   }

   return result;
}

//******************************************************************************
// API Functions
//******************************************************************************
                            
/**
 * @brief Assigns resource id for requested VRM VREG resource.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info.
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
vrm_result vrm_vreg_assign_rsrc_id(vrm_info* vrm_ptr, vrm_rsrc_names* rsrc_names, uint32 num_children, uint32* rsrc_id)
{
   vrm_result result = VRM_SUCCESS;
   uint64 bit_mask = 0x1;
   uint64 child_mask = 0;
   uint32 index = 0;
   uint32 rsrc_parent_id = 0;
   uint32 rsrc_len = 0;
   uint32 rsrc_parent_len = 0;
   uint32 child_id_diff = 0;
   boolean rsrc_found = FALSE;
   boolean rsrc_parent_found = FALSE;
   static uint32 current_rsrc_id = 0;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);
   vrm_vreg_dep_data* dep_ptr = NULL;
   char* bob_str = "bob";

   if((rsrc_names == NULL) || (rsrc_names->name == NULL) || (rsrc_id == NULL))
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else if (((strlen(rsrc_names->name) + 1) > VRM_MAX_RSRC_NAME_LEN) || 
            ((rsrc_names->alias_name != NULL) && ((strlen(rsrc_names->alias_name) + 1) > VRM_MAX_RSRC_NAME_LEN)) ||
            ((rsrc_names->parent_name != NULL) && ((strlen(rsrc_names->parent_name) + 1) > VRM_MAX_RSRC_NAME_LEN)) ||
            (current_rsrc_id >= VRM_NUM_VREG) || (vreg_ptr->num_parents > VRM_MAX_NUM_PARENTS))
   {
      result = VRM_OUT_OF_BOUND_ERROR;
   }
   else if ((rsrc_names->parent_name != NULL) && (!strncmp(rsrc_names->name, rsrc_names->parent_name, strlen(rsrc_names->name) + 1)))
   {
      result = VRM_INVALID_PARAM;
   }
   else
   {
      rsrc_len = strlen(rsrc_names->name) + 1;

      if (rsrc_names->parent_name != NULL)
      {
         rsrc_parent_len = strlen(rsrc_names->parent_name) + 1;
      }

      // check if rsrc is parent rsrc and the rsrc_id is already assigned
      // checking only for parent rsrc here to save memory and avoid saving all rsrc names
      while(index < vreg_ptr->num_parents)
      {
         if (!strncmp(vreg_ptr->dep_info[index].parent_names, rsrc_names->name, rsrc_len))
         {
            *rsrc_id = index;
            rsrc_found = TRUE;
         }
         else if ((rsrc_names->parent_name != NULL) && 
                  (!strncmp(vreg_ptr->dep_info[index].parent_names, rsrc_names->parent_name, rsrc_parent_len)))
         {
            rsrc_parent_id = index;
            rsrc_parent_found = TRUE;
         }
         index = index + 1;
      }

      // if rsrc has a parent, then it should have been assigned an id and pre-initialized
      if (rsrc_names->parent_name != NULL)
      {
         // If rsrc parent is not assigned return error
         // this implies this specific parent rsrc was not pre-initialized
         // If rsrc parent is assigned but dep map is NULL then return error
         if ((rsrc_parent_found == FALSE) || (VREG_DEP(rsrc_parent_id) == NULL))
         {
            result = VRM_DEP_MAP_ERROR;
         }
         else
         {
            dep_ptr = VREG_DEP(rsrc_parent_id);

            // check if this is first child getting registered
            if (dep_ptr->child_start_id == dep_ptr->parent_id)
            {
               // assign the current new rsrc id to the first child of this parent
               dep_ptr->child_start_id = current_rsrc_id;
               dep_ptr->child_end_id = current_rsrc_id;
               child_id_diff = 0;

               result = vrm_vreg_add_cmd_db_entry(vrm_ptr, rsrc_names->name, current_rsrc_id);
               if ((result == VRM_SUCCESS) && (rsrc_names->alias_name != NULL))
               {
                  result = vrm_vreg_add_cmd_db_entry(vrm_ptr, rsrc_names->alias_name, current_rsrc_id);
               }
               *rsrc_id = current_rsrc_id;

               // reserve the next few rsrc ids to this parent based on it's num_children
               current_rsrc_id = current_rsrc_id + dep_ptr->num_children;
               vreg_ptr->num_vreg = vreg_ptr->num_vreg + dep_ptr->num_children;
            }
            else
            {
               dep_ptr->child_end_id = dep_ptr->child_end_id  + 1;

               child_id_diff = (dep_ptr->child_end_id - dep_ptr->child_start_id);

               // check if num of chidren info assigned for this parent during pre-initialization got exceeded
               if(child_id_diff >= dep_ptr->num_children)
               {
                  result = VRM_DEP_MAP_ERROR;
               }
               else
               {
                  result = vrm_vreg_add_cmd_db_entry(vrm_ptr, rsrc_names->name, dep_ptr->child_end_id);
                  if ((result == VRM_SUCCESS) && (rsrc_names->alias_name != NULL))
                  {
                     result = vrm_vreg_add_cmd_db_entry(vrm_ptr, rsrc_names->alias_name, dep_ptr->child_end_id);
                  }
                  *rsrc_id = dep_ptr->child_end_id;
               }
            }

            // save the children mask to use it later during parent-child dependency handling
            if (result == VRM_SUCCESS)
            {
               child_mask = bit_mask << child_id_diff;
               dep_ptr->children_mask = (dep_ptr->children_mask | child_mask);

               child_mask = bit_mask << *rsrc_id;
               vreg_ptr->children_mask = (vreg_ptr->children_mask | child_mask);
            }
         }
      }
      else if(rsrc_found == FALSE) 
      {
         if (current_rsrc_id >= VRM_NUM_VREG)
         {
            result = VRM_OUT_OF_BOUND_ERROR;

         }// check if it's a parent rsrc
         else if(num_children > 0)
         {
            // if it's a new parent rsrc and the dep map is already initialized, return error
            if ((current_rsrc_id >= VRM_MAX_NUM_PARENTS) || (vreg_ptr->num_parents >= VRM_MAX_NUM_PARENTS))
            {
               result = VRM_OUT_OF_BOUND_ERROR;
            }
            else if (VREG_DEP(current_rsrc_id) != NULL)
            {
               result = VRM_DEP_MAP_ERROR;
            }
            else
            {
               // save the new parent name and increment the num_parents
               strlcpy(vreg_ptr->dep_info[current_rsrc_id].parent_names, rsrc_names->name, rsrc_len);
               vreg_ptr->num_parents = vreg_ptr->num_parents + 1;

               // allocate memory for dep map and save the new parent
               result = vrm_os_malloc(sizeof(vrm_vreg_dep_data),(void**)&VREG_DEP(current_rsrc_id));

               if ((result == VRM_SUCCESS) && (VREG_DEP(current_rsrc_id) != NULL))
               {
                  dep_ptr = VREG_DEP(current_rsrc_id);

                  dep_ptr->parent_id = current_rsrc_id;
                  dep_ptr->child_start_id = current_rsrc_id;
                  dep_ptr->child_end_id = current_rsrc_id;
                  dep_ptr->num_children = num_children;

                  // check if the parent is bob and set the is_parent_bob flag to handle special bob cases
                  if ((strlen(bob_str) <= rsrc_len) && (!strncmp(rsrc_names->name, bob_str, strlen(bob_str))))
                  {
                     dep_ptr->is_parent_bob = 1;
                  }
               }
            }
         }

         if (result == VRM_SUCCESS)
         {
            result = vrm_vreg_add_cmd_db_entry(vrm_ptr, rsrc_names->name, current_rsrc_id);
            if ((result == VRM_SUCCESS) && (rsrc_names->alias_name != NULL))
            {
               result = vrm_vreg_add_cmd_db_entry(vrm_ptr, rsrc_names->alias_name, current_rsrc_id);
            }
            *rsrc_id = current_rsrc_id;
            current_rsrc_id = current_rsrc_id + 1;
            vreg_ptr->num_vreg = vreg_ptr->num_vreg + 1;
         }
      }
   }

   return result;
}
                            
/**
 * @brief Initializes VRM VREG vote related data based on the 
 *        resource data provided.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 * 
 * @param[in] rsrc_data pointer to rsrc data 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_vreg_data_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);
   vrm_vreg_vote_data* data_ptr = NULL;
   vrm_vreg_dep_data* dep_ptr = NULL;
   vrm_settings setting;

   // enable resource data
   setting = VRM_ENABLE;
   data_ptr = &(VREG_DATA(rsrc_data->rsrc_id,setting));
   data_ptr->ars_data = rsrc_data->enable_current_state;
   data_ptr->wrs_data = rsrc_data->enable_current_state;
   data_ptr->crs_data = rsrc_data->enable_current_state;
   data_ptr->curr_agg = rsrc_data->enable_current_state;
   data_ptr->min_vote = rsrc_data->enable_min_state;
   data_ptr->settling_err_en = rsrc_data->settling_err_en;
   data_ptr->pin_ctrl = rsrc_data->enable_pin_ctrl;

   // voltage resource data
   setting = VRM_VOLTAGE;
   data_ptr = &(VREG_DATA(rsrc_data->rsrc_id,setting));
   data_ptr->ars_data = rsrc_data->volt_current_state;
   data_ptr->wrs_data = rsrc_data->volt_current_state;
   data_ptr->crs_data = rsrc_data->volt_current_state;
   data_ptr->curr_agg = rsrc_data->volt_current_state;
   data_ptr->min_vote = rsrc_data->volt_min_state;
   data_ptr->settling_err_en = rsrc_data->settling_err_en;

   // mode resource data
   setting = VRM_MODE;
   data_ptr = &(VREG_DATA(rsrc_data->rsrc_id,setting));
   data_ptr->ars_data = rsrc_data->mode_current_state;
   data_ptr->wrs_data = rsrc_data->mode_current_state;
   data_ptr->crs_data = rsrc_data->mode_current_state;
   data_ptr->curr_agg = rsrc_data->mode_current_state;
   data_ptr->min_vote = rsrc_data->mode_min_state;
   data_ptr->settling_err_en = rsrc_data->settling_err_en;

   // headroom resource data
   setting = VRM_HEADROOM;
   data_ptr = &(VREG_DATA(rsrc_data->rsrc_id,setting));
   data_ptr->ars_data = 0;
   data_ptr->wrs_data = 0;
   data_ptr->crs_data = 0;
   data_ptr->curr_agg = 0;
   data_ptr->min_vote = rsrc_data->hr_min_state;
   data_ptr->settling_err_en = rsrc_data->settling_err_en;

   // update resource voting address offsets for each setting
   result = vrm_vreg_update_voting_addr_offset(vrm_ptr, rsrc_data->rsrc_id, vrm_ptr->config->vrm_slave_id);
   
   // if this is a parent rail, set the parent_initialized flag
   if ((rsrc_data->rsrc_id < vreg_ptr->num_parents) && (result == VRM_SUCCESS))
   {
      dep_ptr = VREG_DEP(rsrc_data->rsrc_id);

      if (dep_ptr == NULL)
      {
         result |= VRM_NULL_PTR_ERROR;
      }
      else
      {
         dep_ptr->parent_initialized = 0x1;
      }
   }

   vreg_ptr->vreg[rsrc_data->rsrc_id].pmic_index = rsrc_data->pmic_index;
   vreg_ptr->vreg[rsrc_data->rsrc_id].periph_type = rsrc_data->periph_type;
   vreg_ptr->vreg[rsrc_data->rsrc_id].periph_id = rsrc_data->periph_id;

   // initialize VRM resource specific data
   if (rsrc_data->periph_type == PM_VRM_PERIPH_SPMI)
   {
      vrm_ptr->spmi_info.spmi_flag = TRUE;
      vrm_ptr->spmi_info.rsrc_id = rsrc_data->rsrc_id;
      vrm_ptr->spmi_info.type_id = rsrc_data->type_id;
      vrm_ptr->spmi_info.max_voltage = rsrc_data->volt_max_state;
      vrm_ptr->spmi_info.voltage = 1;
   }

   if (rsrc_data->periph_type == PM_VRM_PERIPH_SOC)
   {
      vrm_ptr->soc_info.pbs_sleep_arg = vrm_ptr->config->soc_pbs_sleep_arg;
      vrm_ptr->soc_info.pbs_wake_arg = vrm_ptr->config->soc_pbs_wake_arg;
      vrm_ptr->soc_info.soc_flag = 1;
      vrm_ptr->soc_info.soc_active = 1;
      vrm_ptr->soc_info.rsrc_id = rsrc_data->rsrc_id;
   }

   if (rsrc_data->periph_type == PM_VRM_PERIPH_PBS_CLIENT)
   {
      vrm_ptr->soc_info.pbs_rsrc_id = rsrc_data->rsrc_id;
   }

   return result;
}
                            
/**
 * @brief Checks for any parent-child dependency mapping errors 
 *        post initialization.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_vreg_check_post_dep_error(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);
   vrm_vreg_dep_data* dep_ptr = NULL;
   uint32 parent = 0;

   // check for parent resources registration errors
   for (parent = 0; parent < vreg_ptr->num_parents; parent++) 
   {
      dep_ptr = VREG_DEP(parent);

      // if any pre-registered parent is not allocated, return error
      if (dep_ptr == NULL)
      {
         result = VRM_DEP_MAP_ERROR;
         break;
      }

      if (dep_ptr->parent_initialized == 0x0)
      {
         result = VRM_DEP_MAP_ERROR;
         break;
      }
   }

   return result;
}
                            
/**
 * @brief Polls for rail's VREG_READY status based on whether 
 *        the resource is enabled  by checking resource ENABLE
 *        WRS status. if status is FALSE and settling error is
 *        enabled for the resource, this function aborts.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info.
 * 
 * @param[in] rsrc_id VREG rsrc id.
 * 
 * @param[in] settling_time_usec Estimaterd settling time in 
 *                               micro secs.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_vreg_rsrc_settle(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 settling_time_usec)
{
   vrm_result result = VRM_SUCCESS;
   uint32 wrs_data = 0;
   uint32 crs_data = 0;
   pm_vrm_periph_info periph_info;
   boolean status = FALSE;
   boolean ocp_status = FALSE;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);

   if (rsrc_id >= VRM_NUM_VREG)
   {
      result = VRM_OUT_OF_BOUND_ERROR;
   }
   else
   {
      periph_info.pmic_index = vreg_ptr->vreg[rsrc_id].pmic_index;
      periph_info.periph_type = vreg_ptr->vreg[rsrc_id].periph_type;
      periph_info.periph_index = vreg_ptr->vreg[rsrc_id].periph_id - 1;
    
      // vreg_ready is expected to be set only if resource is enabled
      // check if WRS data for enable is set in VRM
      result |= vrm_hal_rescfg_wrs_read(vrm_ptr, rsrc_id, VRM_ENABLE, &wrs_data);
    
      if (wrs_data == 1)
      {
         // check for VREG_READY status with sequencer given settling time
         pm_app_vrm_vreg_settle_status(&periph_info, settling_time_usec, &status);
       
         if (status == FALSE)
         {
            result |= VRM_SETTLING_ERROR;

            // clear and log the VREG OCP status if OCP is seen on the rail
            pm_app_vrm_vreg_ocp_status_clear(&periph_info, &ocp_status);
         }
       
         vrm_log(vrm_ptr, VRM_RSRC_REGULATOR, rsrc_id, VRM_ENABLE, &settling_time_usec, result, VRM_EVENT_SETTLING);
       
         // if status is FALSE, then abort if settling_err flag is enabled for the rail
         if ((status == FALSE) && (VREG_DATA(rsrc_id,VRM_ENABLE).settling_err_en))
         {
            CORE_VERIFY(0);
         }
      }
   }

   return result;
}
                            
/**
 * @brief Handles VRM VREG related dependency interrupts.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 * 
 * @param[in] dep_irq_status dependency interrupt status
 * 
 * @param[in] num_settings num of settings
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_vreg_dep_irq_handler(vrm_info* vrm_ptr, uint64* dep_irq_status, uint32 num_settings)
{
   vrm_result result = VRM_SUCCESS;
   uint64 child_irq_status[VRM_NUM_SETTINGS] = {0};
   uint32 setting = 0;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);
   boolean child_irq_status_flag = FALSE;
   uint64 soc_irq_status = 0x0;
   uint64 bit_mask = 0x1;

   if (num_settings > VRM_NUM_SETTINGS)
   {
      return VRM_OUT_OF_BOUND_ERROR;
   }

   // Check for any VREG children dependency interrupts
   for (setting = 0; setting < (num_settings-1); setting++) 
   {
      child_irq_status[setting] = (dep_irq_status[setting] & vreg_ptr->children_mask);

      if (child_irq_status[setting] != 0)
      {
         // set the flag if any vreg child dep irq is triggered
         child_irq_status_flag = TRUE;
      }
   }

   // check if the vrm soc resource dependency interrupt is triggered
   soc_irq_status = (dep_irq_status[VRM_ENABLE] & (bit_mask << vrm_ptr->soc_info.rsrc_id));

   // if vreg child dep irq is triggered, then handle parent-child dependency
   if (child_irq_status_flag == TRUE)
   {

      // coalesce voltage and headroom interrupt status
      child_irq_status[VRM_VOLTAGE] |= child_irq_status[VRM_HEADROOM];
      child_irq_status[VRM_HEADROOM] = child_irq_status[VRM_VOLTAGE];

      // The following table describes how the enable status affects the voltage
      // processing of a child vreg
      //   enable_int_status voltage_int_status    Comments
      //         0                 0             No change
      //         0                 1             Read the voltage requirement but
      //                                         the contribution of the vreg
      //                                         towards parent would be still 0 and the
      //                                         vreg is unblocked immendiately
      //         1                 0             Apply the vregs latest voltage
      //                                         requirement that was last read 
      //                                         towards the parent's voltage
      //         1                 1             Read the latest voltage
      //                                         requriement and apply it towards
      //                                         parent's voltage and enable is
      //                                         blocked from being unblocked until
      //                                         the vreg's voltage is unblocked 
      child_irq_status[VRM_VOLTAGE] = child_irq_status[VRM_ENABLE] | child_irq_status[VRM_VOLTAGE];

      // get the list of dirty children and it's parent
      vrm_vreg_dep_irq_get_dirty_parents(vrm_ptr, dep_irq_status, child_irq_status, num_settings);

      // handle children which have new requirement on the parent
      vrm_vreg_dep_irq_child_parent_vote(vrm_ptr, num_settings);
   }

   if ((vrm_ptr->soc_info.soc_flag == 1) && (soc_irq_status != 0))
   {
      vrm_vreg_dep_irq_soc_handler(vrm_ptr);
   }

   return result;
}
                            
/**
 * @brief Handles VRM VREG related change interrupts.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 * 
 * @param[in] change_irq_status change interrupt status
 * 
 * @param[in] num_settings num of settings
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_vreg_change_irq_handler(vrm_info* vrm_ptr, uint64* change_irq_status, uint32 num_settings)
{
   vrm_result result = VRM_SUCCESS;
   vrm_vreg_dep_data* dep_ptr = NULL;
   uint32 parent = 0;
   uint32 setting = 0;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);

   // check for parent-child tree which has interrupt triggered
   for (parent = 0; parent < vreg_ptr->num_parents; parent++) 
   {
      dep_ptr = VREG_DEP(parent);

      // if this parent is not active then there shouldn't be a change interrupt
      if (!dep_ptr->active_parent)
      {
         continue;
      }

      // if parent is going down we wait for change interrupt on children
      if (dep_ptr->wait_for_children)
      {
         vrm_vreg_change_irq_children(vrm_ptr, change_irq_status, parent, num_settings);
      }

   }

   return result;
}
                            
/**
 * @brief Handles VRM VREG related settled interrupts.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info.
 * 
 * @param[in] settled_irq_status settled interrupt status
 * 
 * @param[in] num_settings num of settings 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_vreg_settled_irq_handler(vrm_info* vrm_ptr, uint64* settled_irq_status, uint32 num_settings)
{
   vrm_result result = VRM_SUCCESS;
   vrm_vreg_dep_data* dep_ptr = NULL;
   uint32 parent = 0;
   uint32 setting = 0;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);

   // check for parent-child tree which has interrupt triggered
   for (parent = 0; parent < vreg_ptr->num_parents; parent++) 
   {
      dep_ptr = VREG_DEP(parent);

      // if this parent is not active then there shouldn't be a settled interrupt
      if (!dep_ptr->active_parent)
      {
         continue;
      }

      // if parent is going up, we wait for settled interrupt on parent
      if (dep_ptr->wait_for_parent)
      {
         vrm_vreg_settled_irq_parent(vrm_ptr, settled_irq_status, parent, num_settings);
      }

   }

   return result;
}
                            
/**
 * @brief Handles VRM VREG related error interrupts.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_vreg_error_irq_handler(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;

   result |= vrm_vreg_error_irq_max_voltage_violation(vrm_ptr);

   return result;
}
                            
/**
 * @brief Sends the vote on requested vreg rsrc id setting and 
 *        data.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info.
 * 
 * @param[in] rsrc_vote Pointer to resource vote info like id, 
 *                      setting, data, rpmh set and completion.
 * 
 * @param[out] msg_id rpmh client driver message id after 
 *                    sending the vote.
 * 
 * @param[in] is_ext_vote Flag to indicate external votes.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
static rpmh_command_set_t vrm_command_set;
vrm_result vrm_vreg_send_vote(vrm_info* vrm_ptr, vrm_rsrc_vote_data* rsrc_vote, uint32 *msg_id, boolean is_ext_vote)
{
   vrm_result result = VRM_SUCCESS;
   vrm_vreg_info* vreg_ptr = NULL;
   vrm_vreg_vote_data* data_ptr = NULL;
   uint32 index = 0;
   uint32 addr = 0;
   uint32 data = 0;
   uint32 vote_req = 0;

   if ((rsrc_vote->num_votes == 0) || (rsrc_vote->num_votes > VRM_MAX_NUM_VOTES))
   {
      result = VRM_OUT_OF_BOUND_ERROR;
   }
   else if (rsrc_vote->set_type != RPMH_SET_ACTIVE)
   {
      result = VRM_INVALID_PARAM;
   }
   else
   {

      vrm_os_memset(&vrm_command_set, 0, sizeof(rpmh_command_set_t));

      vrm_command_set.set = rsrc_vote->set_type;

      vreg_ptr = &(vrm_ptr->vreg_info);

      while (index < rsrc_vote->num_votes)
      {
         data_ptr = &(VREG_DATA(rsrc_vote->vote_data[index].rsrc_id,rsrc_vote->vote_data[index].setting));
         addr = data_ptr->addr_offset;
         data = MAX(rsrc_vote->vote_data[index].data, data_ptr->min_vote);

         // if rsrc is parent, aggregate both external vote and internal parent aggregation
         if (rsrc_vote->vote_data[index].rsrc_id < vreg_ptr->num_parents)
         {
            vote_req = data;

            if (is_ext_vote == TRUE)
            {
               // if external vote, aggregate with internal parent aggregation and update the external vreg vote
               data = MAX(vote_req, data_ptr->curr_agg);
               data_ptr->active_vote = data;
            }
            else
            {
               // if internal parent vote, aggregate with external vreg vote
               data = MAX(vote_req, data_ptr->active_vote);
            }
         }

         vrm_command_set.commands[index].address = addr;
         vrm_command_set.commands[index].data = data;
         vrm_command_set.commands[index].completion = rsrc_vote->completion;
         vrm_command_set.num_commands = vrm_command_set.num_commands + 1;
         index = index + 1;
      }

      *msg_id = rpmh_issue_command_set (rsrc_vote->rpmh_handle, &vrm_command_set);
   }

   return result;
}

vrm_result vrm_vreg_spmi_write(vrm_info* vrm_ptr, uint16 vrm_arb_id, uint8 addr_offset, uint8 data)
{
   vrm_result result = VRM_SUCCESS;
   uint32 type_id = vrm_ptr->spmi_info.type_id;
   uint32 rsrc_id = vrm_ptr->spmi_info.rsrc_id;
   vrm_vreg_info* vreg_ptr = &(vrm_ptr->vreg_info);
   vrm_vreg_vote_data* data_ptr = &(VREG_DATA(rsrc_id,VRM_VOLTAGE));
   uint32 addr = data_ptr->addr_offset;
   uint32 msg_id = 0;

   result = vrm_hal_rescfg_base_addr_write(vrm_ptr, rsrc_id, vrm_arb_id);

   result |= vrm_hal_typecfg_vreg_clr_offset_data_write(vrm_ptr, type_id, addr_offset, data);

   if (vrm_ptr->spmi_info.voltage >= (vrm_ptr->spmi_info.max_voltage - 1))
   {
      vrm_ptr->spmi_info.voltage = 1;
   }

   msg_id = rpmh_issue_command (vrm_ptr->spmi_info.rpmh_handle, RPMH_SET_ACTIVE, TRUE, addr, vrm_ptr->spmi_info.voltage);

   rpmh_barrier_single (vrm_ptr->spmi_info.rpmh_handle, msg_id);

   vrm_ptr->spmi_info.voltage = vrm_ptr->spmi_info.voltage + 1;

   return result;
}
