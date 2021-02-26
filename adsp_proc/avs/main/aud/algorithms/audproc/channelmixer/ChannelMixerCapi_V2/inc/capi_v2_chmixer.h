/* ======================================================================== */
/**
    @file capi_v2_chmixer.h

    Header file to implement the Audio Post Processor Interface for
    Channel Mixer.
*/

/* =========================================================================
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
   ========================================================================= */

/* =========================================================================
   Edit History

     when       who          what, where, why
   --------   -------     -------------------------------------------------
   11/26/14   hbansal        Initial creation
   ======================================================================== */

/*------------------------------------------------------------------------
 * Include files and Macro definitions
 * -----------------------------------------------------------------------*/
#ifndef CAPI_V2_CHMIXER_H
#define CAPI_V2_CHMIXER_H

#include "Elite_CAPI_V2.h"
#include "mmdefs.h"

/*------------------------------------------------------------------------
* Macros, Defines, Type declarations
* -----------------------------------------------------------------------*/
#define CAPI_V2_CHANNELMIXER_PARAM_ID_ENABLE (0)

/*------------------------------------------------------------------------
 * Structure definitions
 * -----------------------------------------------------------------------*/
/** Payload for CHANNELMIXER_PARAM_ID_ENABLE. */
typedef struct capi_v2_chmixer_enable_payload
{
   uint32_t                          enable;               /**< Defined the state of the module. */
} capi_v2_chmixer_enable_payload_t;

/*------------------------------------------------------------------------
* Function declarations
* -----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

capi_v2_err_t capi_v2_chmixer_get_static_properties (capi_v2_proplist_t *init_set_properties, capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_chmixer_init (capi_v2_t *_pif, capi_v2_proplist_t *init_set_properties);

#ifdef __cplusplus
}
#endif

#endif //CAPI_V2_CHMIXER_H

