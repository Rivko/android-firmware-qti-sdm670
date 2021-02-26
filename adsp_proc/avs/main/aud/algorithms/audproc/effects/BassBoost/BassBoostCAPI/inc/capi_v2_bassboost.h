/* =========================================================================
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =========================================================================*/

/**
 * @file capi_v2_bassboost.h
 *
 * Header file to implement the Audio Post Processor Interface for
 * acoustic bass enhancement
 */

/* =========================================================================
 * Edit History:
 * when         who         what, where, why
 * ----------   -------     ------------------------------------------------
 * 2014/04/28   WJ          Initial creation
 * 2014/09/29   VB          Updated based on APPI latest reference
 * =========================================================================*/

#ifndef CAPI_V2_BASSBOOST_H
#define CAPI_V2_BASSBOOST_H

#include "Elite_CAPI_V2.h"
#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get static properties of bassboost module such as
 * memory, stack requirements etc.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t capi_v2_bassboost_get_static_properties(
        capi_v2_proplist_t* init_set_properties,
        capi_v2_proplist_t* static_properties);


/**
 * Instantiates(and allocates) the module memory.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t capi_v2_bassboost_init(
        capi_v2_t*          _pif,
        capi_v2_proplist_t* init_set_properties);

#ifdef __cplusplus
}
#endif

#endif /* CAPI_V2_BASSBOOST_H */

