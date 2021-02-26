/*! \file vrm_utils.c
*
*  \brief Implementation file for VRM util functions.
*  \n
*  &copy; Copyright 2016 - 2018 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/utils/vrm_utils.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_utils.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

//******************************************************************************
// Global Data
//******************************************************************************

//******************************************************************************
// Local Helper Functions
//******************************************************************************
/**
 * @brief converts time from uS to number of vrm_cycles (19.2MHz clock).
 * 
 * @param[in] time_usec time in micro seconds. 
 *  
 * @param[out] time_vrm time returned in vrm_cycles.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_get_time_vrm_cycles(uint32 time_usec, uint32* time_vrm)
{
   vrm_result result = VRM_SUCCESS;

   if (time_vrm == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else if (time_usec >= VRM_MAX_SETTLING_TIME_USEC)
   {
      result = VRM_OUT_OF_BOUND_ERROR;
   }
   else
   {
      *time_vrm = ((time_usec * VRM_CLK_FREQ_KHZ)/1000);
   }

   return result;
}
                            
/**
 * @brief converts stepper rate from uV/uS to vrm_cycles/mV
 *        (19.2MHz clock).
 * 
 * @param[in] stepper_uv_us Stepper rate in uV/uS. 
 *  
 * @param[out] stepper_vrm_mv Stepper rate returned in 
 *                            vrm_cycles/mV.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_get_stepper_rate_vrm_cycles(uint32 stepper_uv_us, uint32* stepper_vrm_mv)
{
   vrm_result result = VRM_SUCCESS;

   if (stepper_vrm_mv == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else if (stepper_uv_us == 0)
   {
      *stepper_vrm_mv = 0;
   }
   else
   {
      *stepper_vrm_mv = (VRM_CLK_FREQ_KHZ/stepper_uv_us);
   }

   return result;
}

/**
 * @brief converts time from number of vrm_cycles (19.2MHz clock) to uS.
 * 
 * @param[in] time_vrm time in vrm_cycles. 
 *
 * @param[out] time_usec time returned in micro seconds.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_get_time_usec(uint32 time_vrm, uint32* time_usec)
{
   vrm_result result = VRM_SUCCESS;

   if (time_usec == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      *time_usec = ((time_vrm * 1000)/VRM_CLK_FREQ_KHZ) + 1;
   }

   return result;
}
                            
/**
 * @brief Logs events and corresponding data for debug.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info. 
 * 
 * @param[in] rsrc_category rsrc category indicating whether 
 *                          rsrc is VREG or XOB.
 *  
 * @param[in] rsrc_id VMR resource id. 
 *  
 * @param[in] setting VRM resource setting.
 * 
 * @param[in] *data VRM resource data. 
 *  
 * @param[in] result result of the event. 
 *  
 * @param[in] log_event VRM software event type.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_log(vrm_info* vrm_ptr, vrm_rsrc_category rsrc_category, uint32 rsrc_id, 
                   uint32 setting, uint32* data, vrm_result result, vrm_log_event log_event)
{
   vrm_ptr->log_info.log_idx = vrm_ptr->log_info.log_idx + 1;

   if (vrm_ptr->log_info.log_idx >= VRM_MAX_NUM_LOGS)
   {
      vrm_ptr->log_info.log_idx = 0;
   }

   if ((rsrc_category == VRM_RSRC_REGULATOR) && (rsrc_id < VRM_NUM_VREG))
   {
      vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].rsrc_id = rsrc_id;
      vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].pmic_index = vrm_ptr->vreg_info.vreg[rsrc_id].pmic_index;
      vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].periph_type = vrm_ptr->vreg_info.vreg[rsrc_id].periph_type;
      vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].periph_id = vrm_ptr->vreg_info.vreg[rsrc_id].periph_id;
   }
   else
   {
      vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].rsrc_id = 0;
      vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].pmic_index = 0;
      vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].periph_type = PM_VRM_PERIPH_NONE;
      vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].periph_id = 0;
   }
   vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].setting = (vrm_settings)setting;

   if (data != NULL)
   {
      vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].data = *data;
   }
   vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].timestamp = vrm_os_timestamp();
   vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].log_event = log_event;
   vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].result = result;

   if (result != VRM_SUCCESS)
   {
      vrm_ptr->log_info.err_idx = vrm_ptr->log_info.err_idx + 1;

      if (vrm_ptr->log_info.err_idx >= VRM_MAX_NUM_ERR_LOGS)
      {
         vrm_ptr->log_info.err_idx = 0;
      }

      vrm_ptr->log_info.err[vrm_ptr->log_info.err_idx] = vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx];

      VRM_LOG_ERROR(PMIC_EVENT_VRM_ERROR, log_event, result,
                    VRM_PMIC_RSRC_INFO(rsrc_category, rsrc_id, setting,
                                       vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].pmic_index,
                                       vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].periph_type,
                                       vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].periph_id),
                    vrm_ptr->log_info.log[vrm_ptr->log_info.log_idx].data);
   }

   return VRM_SUCCESS;
}

