#ifndef __VRM_CMD_DB_H__
#define __VRM_CMD_DB_H__

/*! \file vrm_cmd_db.h
*
*  \brief This header file contains internal VRM CMD DB related definitions.
*  \n
*  &copy; Copyright 2018 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/src/vrm_cmd_db.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_types.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define VRM_CMD_DB_AUX_DATA_VER 0x100 // V1.0
#define VRM_CMD_DB_AUX_DATA_LEN 4 // Needs to be 4 byte aligned.

/**
 * @brief Supported VRM CMD_DB AUX data resource types. 
 *        These values are shared with other masters so need
 *        to be fixed and can be updated only with above
 *        VRM_CMD_DB_AUX_DATA_VER change.
 */
typedef enum 
{
   VRM_RSRC_CMD_DB_LOCAL = 0,
   VRM_RSRC_CMD_DB_REG = 1,
   VRM_RSRC_CMD_DB_XOB = 2,
   VRM_RSRC_CMD_DB_INVALID,
   VRM_RSRC_CMD_DB_MAX = 255,
}vrm_cmd_db_rsrc_type;

//******************************************************************************
// Internal API Functions
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
vrm_result vrm_cmd_db_init(vrm_info* vrm_ptr);

/**
 * @brief VRM Command DB initialization is complete with the 
 * registered PMIC resource names and corresponding offset data.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_cmd_db_post_init(vrm_info* vrm_ptr);
                            
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
vrm_result vrm_cmd_db_add_entry(vrm_info* vrm_ptr, vrm_cmd_db_rsrc_type rsrc_type, const char* name, uint32 addr_offset);

#endif
