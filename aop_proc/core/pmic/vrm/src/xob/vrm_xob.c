/*! \file vrm_xob.c
*
*  \brief Implementation file for VRM XOB driver.
*  \n
*  &copy; Copyright 2016 - 2018 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/src/xob/vrm_xob.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_xob.h"
#include "vrm_hal.h"
#include "vrm_utils.h"
#include "vrm_cmd_db.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define VRM_ENABLE_ADDR_OFFSET 4

//******************************************************************************
// Global Data
//******************************************************************************

//******************************************************************************
// Local Helper Functions
//******************************************************************************
                            
/**
 * @brief Adds VRM XOB related resource info to the command DB.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info.  
 * 
 * @param[in] name VRM XOB resource name  
 * 
 * @param[in] rsrc_id VRM XOB resource id.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
 __attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_xob_add_cmd_db_entry(vrm_info* vrm_ptr, const char* name, uint32 rsrc_id)
{
   vrm_result result = VRM_SUCCESS;
   uint32 addr_offset = 0;

   // add command db entry
   result = vrm_hal_votetable_xob_get_addr_offset(vrm_ptr, rsrc_id, VRM_ENABLE, vrm_ptr->config->vrm_slave_id, &addr_offset);
   addr_offset = addr_offset - VRM_ENABLE_ADDR_OFFSET;

   if (result == VRM_SUCCESS)
   {
      result = vrm_cmd_db_add_entry(vrm_ptr, VRM_RSRC_CMD_DB_XOB, name, addr_offset);
   }

   return result;
}

//******************************************************************************
// API Functions
//******************************************************************************
                            
/**
 * @brief Assigns resource id for requested VRM XOB resource.
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
vrm_result vrm_xob_assign_rsrc_id(vrm_info* vrm_ptr, vrm_rsrc_names* rsrc_names, uint32 num_children, uint32* rsrc_id)
{
   vrm_result result = VRM_SUCCESS;
   static uint32 current_rsrc_id = 0;

   if((rsrc_names == NULL) || (rsrc_names->name == NULL) || (rsrc_id == NULL))
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else if (((strlen(rsrc_names->name) + 1) > VRM_MAX_RSRC_NAME_LEN) || (current_rsrc_id >= VRM_NUM_XOB))
   {
      result = VRM_OUT_OF_BOUND_ERROR;
   }
   else
   {
      result = vrm_xob_add_cmd_db_entry(vrm_ptr, rsrc_names->name, current_rsrc_id);
      if ((result == VRM_SUCCESS) && (rsrc_names->alias_name != NULL))
      {
         result = vrm_xob_add_cmd_db_entry(vrm_ptr, rsrc_names->alias_name, current_rsrc_id);
      }
      *rsrc_id = current_rsrc_id;
      current_rsrc_id = current_rsrc_id + 1;
   }

   return result;
}
