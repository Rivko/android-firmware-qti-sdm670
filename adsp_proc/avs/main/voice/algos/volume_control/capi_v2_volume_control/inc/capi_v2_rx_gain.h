/**
@file capi_v2_rx_gain.h

@brief CAPI V2 API wrapper for  voice rx gain module

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/voice/algos/volume_control/capi_v2_volume_control/inc/capi_v2_rx_gain.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
04/10/2015  kn       Created
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef CAPI_V2_RX_GAIN
#define CAPI_V2_RX_GAIN


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
capi_v2_err_t capi_v2_rx_gain_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_rx_gain_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


#ifdef __cplusplus
}
#endif //__cplusplus
#endif //CAPI_V2_VOICE_RX_GAIN
