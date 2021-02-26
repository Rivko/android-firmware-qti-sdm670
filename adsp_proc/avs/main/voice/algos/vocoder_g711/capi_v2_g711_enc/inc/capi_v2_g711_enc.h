/* ======================================================================== */
/**
   @file capi_v2_g711_enc.h
   @brief CAPI_V2 wrapper for g711 encoder
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
   02/16/16   adeepak      Framework support for audio path
   ========================================================================= */

#ifndef CAPI_V2_G711_ENC_H_
#define CAPI_V2_G711_ENC_H_

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
capi_v2_err_t capi_v2_g711_alaw_voice_enc_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_g711_alaw_voice_enc_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);



/* ----- Entry functions for VSM_MEDIA_TYPE_G711_MLAW ----- */
capi_v2_err_t capi_v2_g711_mlaw_voice_enc_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_g711_mlaw_voice_enc_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);



/* ----- Entry functions for VSM_MEDIA_TYPE_G711_ALAW_V2 ----- */
capi_v2_err_t capi_v2_g711_alaw_v2_voice_enc_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_g711_alaw_v2_voice_enc_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);



/* ----- Entry functions for VSM_MEDIA_TYPE_G711_MLAW_V2 ----- */
capi_v2_err_t capi_v2_g711_mlaw_v2_voice_enc_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_g711_mlaw_v2_voice_enc_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);



/* ----- Entry functions for ASM_MEDIA_FMT_G711_ALAW_FS ----- */
capi_v2_err_t capi_v2_g711_alaw_audio_enc_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_g711_alaw_audio_enc_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);



/* ----- Entry functions for ASM_MEDIA_FMT_G711_MLAW_FS ----- */
capi_v2_err_t capi_v2_g711_mlaw_audio_enc_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_g711_mlaw_audio_enc_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);



#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* CAPI_V2_G711_ENC_H_ */
