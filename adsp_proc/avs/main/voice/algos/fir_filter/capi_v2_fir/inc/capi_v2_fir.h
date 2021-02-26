/**
@file fir_capi.h

@brief CAPI V2 API wrapper for Voice FIR

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/voice/algos/fir_filter/capi_v2_fir/inc/capi_v2_fir.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
09/15/2014  ka       Created
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/

#ifndef FIR_CAPI_H
#define FIR_CAPI_H


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

capi_v2_err_t capi_v2_voice_fir_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_voice_fir_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


#ifdef __cplusplus
}
#endif //__cplusplus
#endif //FIR_CAPI_H
