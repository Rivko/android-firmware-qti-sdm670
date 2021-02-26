/**
@file capi_v2_iir_mic1.h

@brief CAPI V2 API wrapper for Voice iir mic1

*/

/*========================================================================

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
09/16/2014  kn       Created
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/

#ifndef CAPI_V2_IIR_MIC1
#define CAPI_V2_IIR_MIC1

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

capi_v2_err_t capi_v2_voice_iir_mic1_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_voice_iir_mic1_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // CAPI_V2_IIR_MIC1

