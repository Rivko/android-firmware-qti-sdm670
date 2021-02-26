/* =====================================================================
Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =====================================================================*/

/**
 * @file capi_v2_iir_mbdrc.h
 *
 */

/* =====================================================================
 * Edit History:
 * when         who         what, where, why
 * ----------   -------     --------------------------------------------
 * 10/13/17       pag        Initial creation
 * =====================================================================*/

#ifndef CAPI_V2_IIR_MBDRC_H
#define CAPI_V2_IIR_MBDRC_H

#include "Elite_CAPI_V2.h"
#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get static properties of mbdrc module such as
 * memory, stack requirements etc.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t capi_v2_iir_mbdrc_get_static_properties(
		capi_v2_proplist_t* init_set_properties,
		capi_v2_proplist_t* static_properties);


/**
 * Instantiates(and allocates) the module memory.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t capi_v2_iir_mbdrc_init(
		capi_v2_t*          _pif,
		capi_v2_proplist_t* init_set_properties);

#ifdef __cplusplus
}
#endif

#endif

