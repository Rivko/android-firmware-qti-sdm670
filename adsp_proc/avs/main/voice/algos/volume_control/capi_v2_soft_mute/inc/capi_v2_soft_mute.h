/**
@file fir_capi.h

@brief CAPI V2 API wrapper for Voice FIR

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/voice/algos/volume_control/capi_v2_soft_mute/inc/capi_v2_soft_mute.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
09/17/2014  sa       Created
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef VOICE_SOFT_MUTE_CAPI_H
#define VOICE_SOFT_MUTE_CAPI_H


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

// gain control module. does not support soft volume
capi_v2_err_t capi_v2_voice_soft_mute_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_voice_soft_mute_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


#ifdef __cplusplus
}
#endif //__cplusplus
#endif //VOICE_SOFT_MUTE_CAPI_H
