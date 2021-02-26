#ifndef __VRM_VREG_H__
#define __VRM_VREG_H__

/*! \file vrm_vreg.h
*
*  \brief This header file contains internal VRM regulator related definitions.
*  \n
*  &copy; Copyright 2016 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/src/vreg/vrm_vreg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_types.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

//******************************************************************************
// Internal API Functions
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
vrm_result vrm_vreg_assign_rsrc_id(vrm_info* vrm_ptr, vrm_rsrc_names* rsrc_names, uint32 num_children, uint32* rsrc_id);
                            
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
vrm_result vrm_vreg_data_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data);
                            
/**
 * @brief Checks for any parent-child dependency mapping errors 
 *        post initialization.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_vreg_check_post_dep_error(vrm_info* vrm_ptr);
                            
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
vrm_result vrm_vreg_rsrc_settle(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 settling_time_usec);
                            
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
vrm_result vrm_vreg_dep_irq_handler(vrm_info* vrm_ptr, uint64* dep_irq_status, uint32 num_settings);
                            
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
vrm_result vrm_vreg_change_irq_handler(vrm_info* vrm_ptr, uint64* change_irq_status, uint32 num_settings);
                            
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
vrm_result vrm_vreg_settled_irq_handler(vrm_info* vrm_ptr, uint64* settled_irq_status, uint32 num_settings);
                            
/**
 * @brief Handles VRM VREG related error interrupts.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_vreg_error_irq_handler(vrm_info* vrm_ptr);
                            
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
vrm_result vrm_vreg_send_vote(vrm_info* vrm_ptr, vrm_rsrc_vote_data* rsrc_vote, uint32 *msg_id, boolean is_ext_vote);

/**
 * @brief Does a single byte SPMI write for the given PMIC 
 *        periph_id and addr_offset using a VRM VREG resource
 *        (API for internal use only).
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info.
 * 
 * @param[in] vrm_arb_id VRM PMIC_ARB_ID corresponding to PMIC 
 *                       periph_id
 * 
 * @param[in] addr_offset PMIC address offset.
 * 
 * @param[out] data Data that needs to be written.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_vreg_spmi_write(vrm_info* vrm_ptr, uint16 vrm_arb_id, uint8 addr_offset, uint8 data);

#endif
