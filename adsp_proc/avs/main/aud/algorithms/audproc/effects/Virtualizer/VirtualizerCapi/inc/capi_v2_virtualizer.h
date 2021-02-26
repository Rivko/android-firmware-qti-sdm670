/* =========================================================================
Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =========================================================================*/

/**
 * @file capi_v2_virtualizer.h
 *
 * Header file to implement the Audio Post Processor Interface for
 * virtualization
 */

/* =========================================================================
 * Edit History:
 * when         who         what, where, why
 * ----------   -------     ------------------------------------------------
 * 2014/06/23   VB          Initial creation
 * =========================================================================*/

#ifndef CAPI_V2_VIRTUALIZER_H
#define CAPI_V2_VIRTUALIZER_H

#include "Elite_CAPI_V2.h"
#include "adsp_error_codes.h"
#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif


#define AUDPROC_PARAM_ID_VIRTUALIZER_DIPOLE_SPACING      0x0000DEF0
#define AUDPROC_PARAM_ID_VIRTUALIZER_OUTPUT_MEDIA_FMT    0x0000DEF1
#define AUDPROC_PARAM_ID_VIRTUALIZER_VOLUME_RAMP         0x0000DEF2
#define AUDPROC_PARAM_ID_VIRTUALIZER_DELAY               0x0000DEF3


/**
 * Get static properties of virtualizer module such as
 * memory, stack requirements etc.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t capi_v2_virtualizer_get_static_properties(
        capi_v2_proplist_t *init_set_properties,
        capi_v2_proplist_t *static_properties);


/**
 * Instantiates(and allocates) the module memory.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t capi_v2_virtualizer_init(
        capi_v2_t          *_pif,
        capi_v2_proplist_t *init_set_properties);

#ifdef __cplusplus
}
#endif

#endif // CAPI_V2_VIRTUALIZER_H
