/* ======================================================================== */
/**
   @file capi_v2_pbe.h

    Header file to implement the Audio Post Processor Interface for
    Psychoacoustic Bass Enhancer
*/

/* =========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
   ========================================================================== */

/* =========================================================================
   Edit History

     when       who          what, where, why
   --------   -------     -------------------------------------------------
   10/31/14   hbansal        Initial creation
   ======================================================================== */

#ifndef CAPI_V2_PBE_H
#define CAPI_V2_PBE_H

/*------------------------------------------------------------------------
 * Include files
 * -----------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"
#include "mmdefs.h"


#define CAPI_V2_PBE_CHMIXER_PARAM_ID_ENABLE  0x0000FFAA

/*------------------------------------------------------------------------
 * Structure definitions
 * -----------------------------------------------------------------------*/
/** Payload for CAPI_V2_PBE_CHMIXER_PARAM_ID_ENABLE. */

typedef struct capi_v2_pbe_chmixer_enable
{
   uint32_t                          client_enable;               /**< define by client to enable or disable chmixer used in PBE. */
   /*Supported Value :  any non-negative value
    * 					0 : disabled
    * 					any non-zero value : enabled
    * Default        : 0 (disabled)
    */

} capi_v2_pbe_chmixer_enable_t;

/*------------------------------------------------------------------------
 * Function declarations
 * -----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

capi_v2_err_t capi_v2_pbe_get_static_properties (capi_v2_proplist_t *init_set_properties, capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_pbe_init (capi_v2_t *_pif, capi_v2_proplist_t *init_set_properties);

#ifdef __cplusplus
}
#endif

#endif //CAPI_V2_PBE_H

