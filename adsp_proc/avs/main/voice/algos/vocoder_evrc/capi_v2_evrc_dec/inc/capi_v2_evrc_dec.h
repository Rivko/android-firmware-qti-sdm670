#ifndef CAPI_V2_EVRC_DEC_H_
#define CAPI_V2_EVRC_DEC_H_

/* ========================================================================
   EVRC decoder library wrapper header file

  *//** @file capi_v2_evrc_dec.h
  This is a wrapper code for EVRC Core decoder library.
  the function in this file are called by Vdec Service.

Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*//*====================================================================== */

/* =========================================================================
                             Edit History

   when       who                      what, where, why
   --------   ---       ----------------------------------------------------
   09/20/15   shridhar       Created file.
   ========================================================================= */

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

capi_v2_err_t capi_v2_evrc_audio_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_evrc_audio_dec_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


capi_v2_err_t capi_v2_evrc_voice_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_evrc_voice_dec_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* CAPI_V2_EVRC_DEC_H_ */
