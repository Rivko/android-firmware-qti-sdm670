/* =========================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =========================================================================*/

/**
 * @file iec61937_depacketizer_capi_v2.h
 *
 * Depacketizer capi_v2 interface file.
 */

/* =========================================================================
 * Edit History:
 * when         who         what, where, why
 * ----------   -------     ------------------------------------------------
 * 2016/06/26   kgodara     CAPI_V2 Interface for depacketizer
 * =========================================================================*/

#ifndef C_DEPACKETIZER_LIB_H
#define C_DEPACKETIZER_LIB_H

#include "Elite_CAPI_V2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get static properties of depacketizer such as memory, stack
 * requirements etc.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t capiv2_iec61937_depack_get_static_properties(
        capi_v2_proplist_t *init_set_properties,
        capi_v2_proplist_t *static_properties);


/**
 * Instantiates(and allocates) the module memory.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t capiv2_iec61937_depack_init(
        capi_v2_t          *_pif,
        capi_v2_proplist_t *init_set_properties);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* C_DEPACKETIZER_LIB_H */
