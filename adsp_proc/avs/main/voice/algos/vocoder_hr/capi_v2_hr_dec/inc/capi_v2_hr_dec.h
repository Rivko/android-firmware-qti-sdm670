#ifndef CAPI_V2_HR_DEC_H_
#define CAPI_V2_HR_DEC_H_

/*========================================================================

Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

====================================================================== */

/**
 * @file capi_v2_hr_dec.h
 *
 * CAPIv2 wrapper for HR decoder.
 */

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

capi_v2_err_t capi_v2_hr_dec_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_hr_dec_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* CAPI_V2_HR_DEC_H_ */
