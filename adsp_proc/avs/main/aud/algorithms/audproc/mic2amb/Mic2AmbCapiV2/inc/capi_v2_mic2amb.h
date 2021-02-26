/* =====================================================================
Copyright (c) 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =====================================================================*/

/**
 * @file capi_v2_mic2amb.h
 *
 * Header file to implement the Common Audio Processor Interface v2(Capiv2) for
 * mic2amb module
 */

/* =====================================================================
 * Edit History:
 * when         who         what, where, why
 * ----------   -------     --------------------------------------------
 * 08/02/2018   snaginen    create file
 * =====================================================================*/

#ifndef CAPI_V2_MIC2AMB_H
#define CAPI_V2_MIC2AMB_H

#include "Elite_CAPI_V2.h"
#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get static properties of mic2amb module such as
 * memory, stack requirements etc.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t __attribute__ ((visibility ("default"))) capi_v2_mic2amb_get_static_properties(
        capi_v2_proplist_t* init_set_properties,
        capi_v2_proplist_t* static_properties);


/**
 * Instantiates(and allocates) the module memory.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t __attribute__ ((visibility ("default"))) capi_v2_mic2amb_init(
        capi_v2_t*          _pif,
        capi_v2_proplist_t* init_set_properties);

#ifdef __cplusplus
}
#endif

#endif /* CAPI_V2_MIC2AMB_H */

