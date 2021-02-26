/* ======================================================================== */
/**
   @file capi_v2_g711_dec.h
   @brief CAPI_V2 wrapper for g711 decoder
*/

/*========================================================================

Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

====================================================================== */

/* =========================================================================
   Edit History

   when         who        what, where, why
   --------     ---        --------------------------------------------------
   11/17/15   adeepak      Initial creation
   02/09/16   adeepak      Framework support for audio path
   ========================================================================= */

#ifndef CAPI_V2_G711_DEC_H_
#define CAPI_V2_G711_DEC_H_

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/


/* ----- Entry functions for VSM_MEDIA_TYPE_G711_ALAW ----- */
capi_v2_err_t capi_v2_g711_alaw_voice_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_g711_alaw_voice_dec_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);



/* ----- Entry functions for VSM_MEDIA_TYPE_G711_MLAW ----- */
capi_v2_err_t capi_v2_g711_mlaw_voice_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_g711_mlaw_voice_dec_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


/* ----- Entry functions for VSM_MEDIA_TYPE_G711_ALAW_V2 ----- */
capi_v2_err_t capi_v2_g711_alaw_v2_voice_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_g711_alaw_v2_voice_dec_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


/* ----- Entry functions for VSM_MEDIA_TYPE_G711_ALAW_V2 ----- */
capi_v2_err_t capi_v2_g711_mlaw_v2_voice_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_g711_mlaw_v2_voice_dec_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);



/* ----- Entry functions for ASM_MEDIA_FMT_G711_ALAW_FS ----- */
capi_v2_err_t capi_v2_g711_alaw_audio_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_g711_alaw_audio_dec_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);



/* ----- Entry functions for ASM_MEDIA_FMT_G711_MLAW_FS ----- */
capi_v2_err_t capi_v2_g711_mlaw_audio_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_g711_mlaw_audio_dec_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);



#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* CAPI_V2_G711_DEC_H_ */
