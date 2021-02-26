/* ======================================================================== */
/**
    @file capi_v2_cfcm.h

    Header file to implement the Audio/Voice Post Processor Interface for
    CFCM module.
*/

/* =========================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
   ========================================================================= */

/* =========================================================================
   Edit History

     when       who          what, where, why
   --------   -------     -------------------------------------------------
   12/07/16   pallavim          Initial creation
   ======================================================================== */

#ifndef CAPI_V2_CFCM
#define CAPI_V2_CFCM

//#define CAPI_V2_CFCM_DEBUG_MSG

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/
capi_v2_err_t __attribute__  ((visibility ("default")))capi_v2_cfcm_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t __attribute__  ((visibility ("default")))capi_v2_cfcm_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);



#ifdef __cplusplus
}
#endif //__cplusplus
#endif // CAPI_V2_CFCM
