#ifndef CAPI_V2_EAMR_DEC_H_
#define CAPI_V2_EAMR_DEC_H_


/*========================================================================

Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

====================================================================== */

/**
 * @file capi_v2_eamr_dec.h
 *
 * CAPIv2 interface for eamr decoder
 */

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

capi_v2_err_t capi_v2_amrnb_voice_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_amrnb_voice_dec_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);

capi_v2_err_t capi_v2_amrnb_audio_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_amrnb_audio_dec_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


capi_v2_err_t capi_v2_amrnb_if2_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_amrnb_if2_dec_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);

capi_v2_err_t capi_v2_eamr_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_eamr_dec_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);



#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* CAPI_V2_EAMR_DEC_H_ */
