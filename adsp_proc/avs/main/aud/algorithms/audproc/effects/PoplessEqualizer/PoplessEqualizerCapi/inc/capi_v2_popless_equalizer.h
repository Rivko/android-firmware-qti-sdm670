/* =========================================================================
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =========================================================================*/

/**
 * @file capi_v2_popless_equalizer.h
 *
 * Common Audio Processor Interface for Popless Equalizer.
 */

/* =========================================================================
 * Edit History:
 * when         who         what, where, why
 * ----------   -------     ------------------------------------------------
 * 2014/09/27   kgodara     CAPI_V2 Interface for P_EQ
 * =========================================================================*/

#ifndef CAPI_V2_POPLESS_EQUALIZER_H
#define CAPI_V2_POPLESS_EQUALIZER_H

#include "Elite_CAPI_V2.h"
#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get static properties of popless equalizer module such as
 * memory, stack requirements etc.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t capi_v2_p_eq_get_static_properties(
        capi_v2_proplist_t* init_set_properties,
        capi_v2_proplist_t* static_properties);


/**
 * Instantiates(and allocates) the module memory.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t capi_v2_p_eq_init(
        capi_v2_t*          _pif,
        capi_v2_proplist_t* init_set_properties);

#ifdef __cplusplus
}
#endif

#endif /* CAPI_V2_POPLESS_EQUALIZER_H */

