/* =====================================================================
Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =====================================================================*/

/**
 * @file capi_v2_shoebox.h
 *
 * Header file to implement the Audio Post Processor Interface for
 * Shoebox module
 */

/* =====================================================================
 * Edit History:
 * when         who         what, where, why
 * ----------   -------     --------------------------------------------
 * 14/12/16     bvodapal    create file
 * =====================================================================*/

#ifndef CAPI_V2_SHOEBOX_H
#define CAPI_V2_SHOEBOX_H

#include "Elite_CAPI_V2.h"
#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get static properties of shoebox module such as
 * memory, stack requirements etc.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t capi_v2_shoebox_get_static_properties(
        capi_v2_proplist_t* init_set_properties,
        capi_v2_proplist_t* static_properties);


/**
 * Instantiates(and allocates) the module memory.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t capi_v2_shoebox_init(
        capi_v2_t*          _pif,
        capi_v2_proplist_t* init_set_properties);

#ifdef __cplusplus
}
#endif

#endif /* CAPI_V2_SHOEBOX_H */

