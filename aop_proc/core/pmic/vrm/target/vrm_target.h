#ifndef __VRM_TARGET_H__
#define __VRM_TARGET_H__

/*! \file vrm_target.h
*
*  \brief This header file contains internal VRM target specific definitions.
*  \n
*  &copy; Copyright 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/target/vrm_target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/06/17   kt      Initial version
===========================================================================*/

#include "vrm.h"
#include "vrm_utils.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

//******************************************************************************
// API Functions
//******************************************************************************
                            
/**
 * @brief VRM target specific initialization. 
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_target_post_init(vrm_info* vrm_ptr);
                            
/**
 * @brief Handles VRM related target specific dependency 
 *        interrupts.
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
vrm_result vrm_target_dep_irq_handler(vrm_info* vrm_ptr, uint64* dep_irq_status, uint32 num_settings);
                            
/**
 * @brief Handles VRM related target specific change interrupts.
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
vrm_result vrm_target_change_irq_handler(vrm_info* vrm_ptr, uint64* change_irq_status, uint32 num_settings);
                            
/**
 * @brief Handles VRM related target specific settled 
 *        interrupts.
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
vrm_result vrm_target_settled_irq_handler(vrm_info* vrm_ptr, uint64* settled_irq_status, uint32 num_settings);

#endif // __VRM_TARGET_H__
