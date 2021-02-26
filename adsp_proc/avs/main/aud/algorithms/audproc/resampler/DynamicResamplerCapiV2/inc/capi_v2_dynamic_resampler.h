/* ======================================================================== */
/**
   @file capi_v2_dynamic_resampler.h

   Header file to implement the Audio Post Processor Interface for
   Dynamic Resampler
*/

/* =========================================================================
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
   ========================================================================== */

/* =========================================================================
   Edit History

   when       who     what, where, why
   --------   ---     ------------------------------------------------------
   07/11/14   BV       Initial creation
   ========================================================================= */

/*------------------------------------------------------------------------
 * Include files
 * -----------------------------------------------------------------------*/
#ifndef CAPI_V2_DYNAMIC_RESAMPLER_H
#define CAPI_V2_DYNAMIC_RESAMPLER_H

#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

#include "Elite_CAPI_V2.h"

/*------------------------------------------------------------------------
 * Macros, Defines, Type declarations
 * -----------------------------------------------------------------------*/

/* Internal Param ID of the Dynamic Resampler to suspend and resume
 * the hardware resampler.
 */
#define AUDPROC_PARAM_ID_RESAMPLER_HW_RESUME_SUSPEND 0xFFFF0001

typedef struct _param_id_dynamic_rs_hw_resume_suspend_t
{
   uint32_t is_suspend;
   /**< Specifies whether to suspend or resume the hardware resampler

        @values
        - Non-zero -- Hardware resampler is suspended.
        - 0        -- Hardware resampler is resumed.*/

} param_id_dynamic_rs_hw_resume_suspend_t;


/** Internal Param ID of the Dynamic Resampler force disable hardware resampler parameter
 * This setparam can be used to disable the usage of hardware resampler for a
 * particular instance of dynamic resampler
 *
 * Notes :
 * 1. The setparam should be sent before sending the input media fmt command
 * otherwise it will be ignored
 * 2. Once disabled, the hardware resampler will not be enabled again for that
 * particular instance of the module
*/

#define AUDPROC_PARAM_ID_RESAMPLER_FORCE_DISABLE_HW_RS 0xFFFF0002


/* Structure for disabling the hardware rs usage in dynamic resampler */
typedef struct _param_id_dynamic_rs_force_disable_hw_rs_t
{
   uint32_t force_disable;
   /**< Specifies whether to disable the usage of hardware resampler

        @values
        - Non-zero -- Hardware resampler disabled
        - 0        -- Ignore */

} param_id_dynamic_rs_force_disable_hw_rs_t;

/*------------------------------------------------------------------------
 * Function declarations
 * -----------------------------------------------------------------------*/

capi_v2_err_t capi_v2_dynamic_resampler_get_static_properties (
        capi_v2_proplist_t  *init_set_properties, 
        capi_v2_proplist_t  *static_properties);

capi_v2_err_t capi_v2_dynamic_resampler_init (
        capi_v2_t           *_pif, 
        capi_v2_proplist_t  *init_set_properties);


#ifdef __cplusplus
}
#endif /*__cplusplus*/


#endif // CAPI_V2_DYNAMIC_RESAMPLER_H
