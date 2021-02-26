#ifndef __VRM_DRIVER_H__
#define __VRM_DRIVER_H__

/*! \file vrm_driver.h
*
*  \brief This header file contains internal VRM generic driver related definitions.
*  \n
*  &copy; Copyright 2016 - 2018 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/src/vrm_driver.h#1 $

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
 * @brief Initializes the VRM driver.
 *
 * This function initializes the VRM driver structures/config data 
 * and calls VRM HAL initialization function to do basic VRM HW 
 * configuration and VRM sequence mem configuration.
 * 
 * @param None
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_driver_init(void);

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
vrm_result vrm_driver_post_init(void);
                            
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
vrm_result vrm_assign_rsrc_id(vrm_rsrc_category rsrc_category, vrm_rsrc_names* rsrc_names, 
                              uint32 num_children, uint32* rsrc_id);
                            
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
vrm_result vrm_add_rsrc_type(vrm_rsrc_type_data* type_data, uint32* type_id);
                            
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
vrm_result vrm_add_rsrc(vrm_rsrc_names* rsrc_names, vrm_rsrc_data* rsrc_data);
                            
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
vrm_result vrm_add_local_rsrc(vrm_rsrc_names* rsrc_names, vrm_rsrc_category rsrc_category, uint32 base_addr);
                            
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
vrm_result vrm_get_seq_id(vrm_seq_type seq_type, uint32* seq_id);
                            
/**
 * @brief VRM IRQ handler called from REX task. This is mainly a 
 *        routing function to specific IRQ handler
 *        implementations.
 * 
 * @param[in] irq interrupt which needs to be handled.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_irq_handler(vrm_irq_num irq);
                            
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
vrm_result vrm_send_vote(vrm_rsrc_vote_data* rsrc_vote, uint32 *msg_id);

#endif
