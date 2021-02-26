/**
@file afe_capi_v2_util.h

@brief This is the internal header file for CAPI V2 AANC wrapper function calls.

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/algos/common/inc/afe_capi_v2_util.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
4/4/2014   rv       Created

==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/

#ifndef AFECAPIV2UTIL_H_
#define AFECAPIV2UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include "Elite.h"
#include "Elite_CAPI_V2.h"

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/
ADSPResult afe_capi_v2_create(capi_v2_t **capi_v2_ptr_ptr, uint32_t module_id);
ADSPResult afe_capi_v2_destroy(capi_v2_t **capi_v2_ptr_ptr);

ADSPResult afe_capi_v2_get_param(capi_v2_t *capi_v2_ptr, uint32_t param_id, int8_t *payload, uint32_t in_size, uint32_t *out_size);
ADSPResult afe_capi_v2_set_param(capi_v2_t *capi_v2_ptr, uint32_t param_id, int8_t *payload, uint32_t size);
ADSPResult afe_capi_v2_set_property(capi_v2_t *capi_v2_ptr, uint32_t prop_id, int8_t *payload, uint32_t size);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* AFECAPIV2UTIL_H_ */
