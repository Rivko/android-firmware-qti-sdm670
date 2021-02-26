/**
@file capi_v2_sp_rx.h

@brief CAPI V2 API wrapper for Rx lib

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/algos/sp/capi_v2_rx/inc/capi_v2_sp_rx.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/22/2014   rv       Created
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/

#ifndef CAPI_V2_SP_RX_H_
#define CAPI_V2_SP_RX_H_


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

capi_v2_err_t capi_v2_sp_rx_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_sp_rx_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* CAPI_V2_SP_RX_H_ */
