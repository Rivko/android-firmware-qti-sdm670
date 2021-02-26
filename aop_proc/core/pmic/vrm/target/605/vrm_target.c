/*! \file vrm_target.c
*
*  \brief Contains target specific vrm based implementation.
*  \n
*  &copy; Copyright 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/target/605/vrm_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/19/14   as      Created.s
===========================================================================*/

#include "vrm_target.h"
#include "rinit.h"
#include "vrm_hal.h"
#include "pm_comm_inc.h"


//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

//******************************************************************************
// Global Data
//******************************************************************************


//******************************************************************************
// Local Helper Functions
//******************************************************************************
vrm_result vrm_target_rsrc_aon(vrm_info* vrm_ptr)
{
   return VRM_SUCCESS;
}

vrm_result vrm_target_specific_rinit(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;
   int ret_val = RINIT_SUCCESS;

   ret_val = rinit_module_init ("vrm_tcs");

   if (ret_val != RINIT_SUCCESS)
   {
      result |= VRM_RINIT_ERROR;
   }

   return result;
}

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
vrm_result vrm_target_post_init(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;

   result |= vrm_target_rsrc_aon(vrm_ptr);

   result |= vrm_target_specific_rinit(vrm_ptr);

   return result;
}
                            
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
vrm_result vrm_target_dep_irq_handler(vrm_info* vrm_ptr, uint64* dep_irq_status, uint32 num_settings)
{
   return VRM_SUCCESS;
}
                            
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
vrm_result vrm_target_change_irq_handler(vrm_info* vrm_ptr, uint64* change_irq_status, uint32 num_settings)
{
   return VRM_SUCCESS;
}
                            
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
vrm_result vrm_target_settled_irq_handler(vrm_info* vrm_ptr, uint64* settled_irq_status, uint32 num_settings)
{
   return VRM_SUCCESS;
}

