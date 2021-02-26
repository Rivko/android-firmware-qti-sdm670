#ifndef __VRM_UTILS_H__
#define __VRM_UTILS_H__

/*! \file vrm_utils.h
*
*  \brief This header file contains internal VRM driver utility function definitions.
*  \n
*  &copy; Copyright 2016 - 2018 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/utils/vrm_utils.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_os.h"
#include "vrm_types.h"
#include "aop_ulog.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define VRM_READ_BIT(value, pos)  ((value >> pos) & 0x1)

// VRM clock freq in KHz
#define VRM_CLK_FREQ_KHZ 19200

#define VRM_LOG(...) SWEVENT(__VA_ARGS__)
#define VRM_LOG_ERROR(...) VRM_LOG(__VA_ARGS__)

#define VRM_PMIC_RSRC_INFO(rsrc_cat, rsrc_id, setting, pmic_idx, periph_type, periph_id) \
                  (((rsrc_cat)<<28)|((rsrc_id)<<22)|((setting)<<18)|((pmic_idx)<<14)|((periph_type)<<8)|((periph_id)))

//******************************************************************************
// Internal API Functions
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
vrm_result vrm_get_time_vrm_cycles(uint32 time_usec, uint32* time_vrm);
                            
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
vrm_result vrm_get_stepper_rate_vrm_cycles(uint32 stepper_uv_us, uint32* stepper_vrm_mv);

/**
 * @brief converts time from number of vrm_cycles (19.2MHz clock) to uS.
 * 
 * @param[in] time_vrm time in vrm_cycles. 
 *
 * @param[out] time_usec time returned in micro seconds.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_get_time_usec(uint32 time_vrm, uint32* time_usec);
                            
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
                   uint32 setting, uint32* data, vrm_result result, vrm_log_event log_event);

#endif
