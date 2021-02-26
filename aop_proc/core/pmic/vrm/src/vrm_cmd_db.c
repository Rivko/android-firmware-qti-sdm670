/*! \file vrm_cmd_db.c
*
*  \brief Implementation file for VRM CMD DB related functions
*  \n
*  &copy; Copyright 2018 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/src/vrm_cmd_db.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/
#include "vrm_utils.h"
#include "vrm_cmd_db.h"
#include "cmd_db_aop.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

//******************************************************************************
// Global Data
//******************************************************************************

//******************************************************************************
// API Functions
//******************************************************************************
/**
 * @brief Initializes the VRM CMD_DB.
 *
 * This function initializes the command DB to add VRM resource entries.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_cmd_db_init(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;
   cmd_db_err_type cmd_db_err = CMD_DB_SUCCESS;

   // initialize command db for VRM
   cmd_db_err = cmd_db_add_entry_init(vrm_ptr->config->vrm_slave_id, VRM_CMD_DB_AUX_DATA_VER);

   if (cmd_db_err != CMD_DB_SUCCESS)
   {
      result |= VRM_CMD_DB_ERROR;
   }

   return result;
}

/**
 * @brief VRM Command DB initialization is complete with the 
 * registered PMIC resource names and corresponding offset data.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_cmd_db_post_init(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;
   cmd_db_err_type cmd_db_err = CMD_DB_SUCCESS;

   // complete command db initialization for VRM
   cmd_db_err = cmd_db_add_entry_complete(vrm_ptr->config->vrm_slave_id);

   if (cmd_db_err != CMD_DB_SUCCESS)
   {
      result |= VRM_CMD_DB_ERROR;
   }

   return result;
}
                            
/**
 * @brief Adds VRM related resource info to the command DB.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 * 
 * @param[in] rsrc_type VRM resource type to be added as CMD_DB 
 *                       aux data
 * 
 * @param[in] name VRM resource name  
 * 
 * @param[in] addr_offset VRM resource address offset.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
static cmd_db_add_entry_type vrm_cmddb_entry;

__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_cmd_db_add_entry(vrm_info* vrm_ptr, vrm_cmd_db_rsrc_type rsrc_type, const char* name, uint32 addr_offset)
{
   vrm_result result = VRM_SUCCESS;
   uint32 rsrc_len = 0;
   cmd_db_err_type cmd_db_err = CMD_DB_SUCCESS;
   uint8 cmd_db_aux_data[VRM_CMD_DB_AUX_DATA_LEN] = {0};

   if (name != NULL)
   {
      rsrc_len = strlen(name)+1;
   }

   if (name == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else if ((rsrc_len == 0) || 
            (rsrc_len > VRM_MAX_RSRC_NAME_LEN) || 
            (rsrc_type >= VRM_RSRC_CMD_DB_INVALID))
   {
      result = VRM_OUT_OF_BOUND_ERROR;
   }
   else
   {
      // add command db entry
      cmd_db_aux_data[0] = rsrc_type;

      vrm_os_memset(&vrm_cmddb_entry, 0, sizeof(cmd_db_add_entry_type));
      strlcpy(vrm_cmddb_entry.res_id, name, rsrc_len);
      vrm_cmddb_entry.addr = addr_offset;
      vrm_cmddb_entry.len = VRM_CMD_DB_AUX_DATA_LEN; 
      cmd_db_err = cmd_db_add_entry(vrm_ptr->config->vrm_slave_id, &vrm_cmddb_entry, cmd_db_aux_data);
      if (cmd_db_err != CMD_DB_SUCCESS)
      {
         result = VRM_CMD_DB_ERROR;
      }
   }

   return result;
}