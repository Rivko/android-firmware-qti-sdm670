/* =========================================================================
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =========================================================================*/

/**
 * @file capi_v2_soft_vol.h
 *
 * Common Audio Processor Interface for soft_vol.
 */

/* =========================================================================
 * Edit History:
 * when         who         what, where, why
 * ----------   -------     ------------------------------------------------
 * 2015/03/13   kgodara     CAPI_V2 Interface for soft_vol
 * =========================================================================*/

#ifndef CAPI_V2_SOFT_VOL_H
#define CAPI_V2_SOFT_VOL_H

#include "Elite_CAPI_V2.h"
#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AUDPROC_PARAM_ID_SOFT_PAUSE_START
#define AUDPROC_PARAM_ID_SOFT_PAUSE_START                (0x0002FFFF)
#endif

#ifndef AUDPROC_PARAM_ID_SOFT_PAUSE_SET_RAMP_ON_RESUME
#define AUDPROC_PARAM_ID_SOFT_PAUSE_SET_RAMP_ON_RESUME   (0x0003FFFF)
#endif

/* Param-id for communicating headroom requirement to the Volume Control module through
 * the service.  Currently, this is used only by the Popless Equalizer by raising CAPI_V2_EVENT_HEADROOM event */
#ifndef AUDPROC_PARAM_ID_ALGORITHMIC_HEADROOM
#define AUDPROC_PARAM_ID_ALGORITHMIC_HEADROOM            (0x000108d8)
#endif

/**
 * Get static properties of soft_vol module such as
 * memory, stack requirements etc.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t capi_v2_soft_vol_get_static_properties(
        capi_v2_proplist_t *init_set_properties,
        capi_v2_proplist_t *static_properties);


/**
 * Instantiates(and allocates) the module memory.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t capi_v2_soft_vol_init(
        capi_v2_t          *_pif,
        capi_v2_proplist_t *init_set_properties);

#ifdef __cplusplus
}
#endif

#endif /* CAPI_V2_SOFT_VOL_H */

