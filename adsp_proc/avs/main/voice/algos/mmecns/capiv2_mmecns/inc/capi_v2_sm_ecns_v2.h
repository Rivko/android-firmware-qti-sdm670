/**
 @file capi_v2_sm_ecns_v2.h

 @brief This contains wrapper functions for CAPI V2 SM_ECNS V2 lib

 */

/*========================================================================
 $Header: //components/rel/avs.adsp/2.8.5/voice/algos/mmecns/capiv2_mmecns/inc/capi_v2_sm_ecns_v2.h#1 $

 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------


 ==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 -----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#ifndef CAPI_V2_SM_ECNS_V2
#define CAPI_V2_SM_ECNS_V2

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus
#include "Elite_CAPI_V2.h"

capi_v2_err_t capi_v2_voice_sm_ecns_v2_get_static_properties(capi_v2_proplist_t *init_props_ptr, capi_v2_proplist_t *out_props_ptr);

capi_v2_err_t capi_v2_voice_sm_ecns_v2_init(capi_v2_t* _pif, capi_v2_proplist_t *init_set_properties);

#if defined(__cplusplus)
}
#endif // __cplusplus
#endif // CAPI_V2_SM_ECNS_V2
