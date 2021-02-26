/**
@file fir_capi.h

@brief CAPI V2 API wrapper for Voice FIR

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/voice/algos/volume_control/capi_v2_volume_control/inc/capi_v2_rx_volume_control.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
09/17/2014   sa       Created
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef _CAPI_V2_RX_VOLUME_CONTROL_H_
#define _CAPI_V2_RX_VOLUME_CONTROL_H_


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"
#include "Elite_fwk_extns_avctx_ecns_rx_drc_vol_imc.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

// gain control module. does not support soft volume
capi_v2_err_t capi_v2_rx_volume_control_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_rx_volume_control_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


#ifdef __cplusplus
}
#endif //__cplusplus
#endif //_CAPI_V2_RX_VOLUME_CONTROL_H_
