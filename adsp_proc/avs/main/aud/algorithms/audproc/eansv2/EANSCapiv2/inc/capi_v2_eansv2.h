/* ======================================================================== */
/**
   @file capi_v2_eans.h

   Header file to implement the Common Audio Processor Interface v2 for
   Enhanced Audio Noise Supression (EANS).
*/

/* =========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
   ========================================================================== */

/* =========================================================================
   Edit History

   when         who        what, where, why
   --------     ---        --------------------------------------------------
   10/18/15   pparamat      Initial creation
   ========================================================================= */

/*------------------------------------------------------------------------
 * Include files
 * -----------------------------------------------------------------------*/
#ifndef CAPI_V2_EANSV2_H
#define CAPI_V2_EANSV2_H

#include "Elite_CAPI_V2.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

/*------------------------------------------------------------------------
 * Function declarations
 * -----------------------------------------------------------------------*/

capi_v2_err_t capi_v2_audio_eansv2_get_static_properties (
        capi_v2_proplist_t *init_set_properties,
        capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_audio_eansv2_init (
        capi_v2_t*              _pif,
        capi_v2_proplist_t      *init_set_properties);

//EANS entry functions for voice client
capi_v2_err_t capi_v2_voice_eansv2_get_static_properties (
		capi_v2_proplist_t *init_set_properties,
		capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_voice_eansv2_init (
		capi_v2_t*  _pif,
		capi_v2_proplist_t      *init_set_properties);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // CAPI_V2_EANS_H
