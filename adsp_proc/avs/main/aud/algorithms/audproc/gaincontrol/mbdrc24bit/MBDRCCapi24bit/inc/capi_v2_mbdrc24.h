/* ======================================================================== */
/**
    @file capi_v2_mbdrc24.h

    Header file to implement the Audio/Voice Post Processor Interface for
    MBDRC module.
*/

/* =========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
   ========================================================================= */

/* =========================================================================
   Edit History

     when       who          what, where, why
   --------   -------     -------------------------------------------------
   10/15/14     kn          Initial creation
   03/28/14   hbansal       Updated for audio interface.
   ======================================================================== */

#ifndef CAPI_V2_MBDRC24
#define CAPI_V2_MBDRC24

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
capi_v2_err_t capi_v2_audio_mbdrc24bit_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_audio_mbdrc24bit_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);

capi_v2_err_t capi_v2_voice_mbdrc24bit_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_voice_mbdrc24bit_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // CAPI_V2_MBDRC24
