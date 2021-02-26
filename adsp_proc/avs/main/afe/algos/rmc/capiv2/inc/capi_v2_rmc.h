/**
@file capi_v2_rmc.h

@brief CAPI V2 API wrapper for RMC lib

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/algos/rmc/capiv2/inc/capi_v2_rmc.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
5/4/2014   rv       Created
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/

#ifndef CAPI_V2_RMC_H_
#define CAPI_V2_RMC_H_


#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"
#include "mmdefs.h"

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

capi_v2_err_t capi_v2_rmc_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_rmc_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* CAPI_V2_RMC_H_ */
