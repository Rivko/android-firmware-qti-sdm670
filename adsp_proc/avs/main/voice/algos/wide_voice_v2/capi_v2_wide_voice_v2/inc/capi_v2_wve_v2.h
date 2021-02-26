/**
@file capi_v2_wve_v2.h

@brief CAPI V2 API wrapper header for wide voice V2 algorithm

*/

/*========================================================================

Edit History

when        who            what, where, why
--------    ---            -------------------------------------------------------
12/10/2015  shridhar       Creation
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef CAPI_V2_WVE_V2
#define CAPI_V2_WVE_V2

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

capi_v2_err_t capi_v2_wve_v2_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_wve_v2_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // CAPI_V2_WVE_V2
