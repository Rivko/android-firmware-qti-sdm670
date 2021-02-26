/**
 @brief This contains wrapper functions for CAPI V2 DUMMY ECNS lib

 */

/*========================================================================

 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------


 ==========================================================================*/

/*-----------------------------------------------------------------------
 Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
 -----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#ifndef CAPI_V2_CUSTOM_DUMMY_ECNS
#define CAPI_V2_CUSTOM_DUMMY_ECNS

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus
#include "Elite_CAPI_V2.h"

#define DUMMY_ECNS_PARAM_ID_1 (0x10010019) // This shows to set custom parameters for module


capi_v2_err_t capi_v2_custom_dummy_sm_ecns_get_static_properties(capi_v2_proplist_t *init_props_ptr, capi_v2_proplist_t *out_props_ptr);
capi_v2_err_t capi_v2_custom_dummy_dm_ecns_get_static_properties(capi_v2_proplist_t *init_props_ptr, capi_v2_proplist_t *out_props_ptr);
capi_v2_err_t capi_v2_custom_dummy_qm_ecns_get_static_properties(capi_v2_proplist_t *init_props_ptr, capi_v2_proplist_t *out_props_ptr);

capi_v2_err_t capi_v2_custom_dummy_sm_ecns_init(capi_v2_t* _pif, capi_v2_proplist_t *init_set_properties);
capi_v2_err_t capi_v2_custom_dummy_dm_ecns_init(capi_v2_t* _pif, capi_v2_proplist_t *init_set_properties);
capi_v2_err_t capi_v2_custom_dummy_qm_ecns_init(capi_v2_t* _pif, capi_v2_proplist_t *init_set_properties);


#if defined(__cplusplus)
}
#endif // __cplusplus
#endif // CAPI_V2_CUSTOM_DUMMY_ECNS
