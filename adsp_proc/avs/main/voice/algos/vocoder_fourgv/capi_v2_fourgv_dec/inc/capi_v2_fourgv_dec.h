#ifndef CAPI_V2_FOURGV_DEC_H_
#define CAPI_V2_FOURGV_DEC_H_

/* ========================================================================
   FOURGV decoder library wrapper header file

  *//** @file capi_v2_fourgv_dec.h
  This is a wrapper code for FOURGV Core decoder library.
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

capi_v2_err_t capi_v2_fourgv_nb_audio_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_fourgv_nb_audio_dec_init (
   capi_v2_t *_pif,
   capi_v2_proplist_t *init_set_properties);

capi_v2_err_t capi_v2_fourgv_wb_audio_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_fourgv_wb_audio_dec_init (
   capi_v2_t *_pif,
   capi_v2_proplist_t *init_set_properties);

capi_v2_err_t capi_v2_fourgv_nb_voice_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_fourgv_nb_voice_dec_init (
   capi_v2_t *_pif,
   capi_v2_proplist_t *init_set_properties);

capi_v2_err_t capi_v2_fourgv_wb_voice_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_fourgv_wb_voice_dec_init (
   capi_v2_t *_pif,
   capi_v2_proplist_t *init_set_properties);

capi_v2_err_t capi_v2_fourgv_nw_modem_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_fourgv_nw_modem_dec_init (
   capi_v2_t *_pif,
   capi_v2_proplist_t *init_set_properties);

capi_v2_err_t capi_v2_fourgv_nw_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_fourgv_nw_dec_init (
   capi_v2_t *_pif,
   capi_v2_proplist_t *init_set_properties);

capi_v2_err_t capi_v2_fourgv_nw_2k_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_fourgv_nw_2k_dec_init (
   capi_v2_t *_pif,
   capi_v2_proplist_t *init_set_properties);

/* -------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* CAPI_V2_FOURGV_DEC_H_ */
