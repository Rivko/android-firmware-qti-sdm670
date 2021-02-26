/* ======================================================================== */
/**
    @file capi_v2_byteshifter.h

    Header file to implement the Audio Post Processor Interface for
    Byte-shifter.
*/

/* =========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
   ========================================================================= */

/* =========================================================================
   Edit History

     when       who          what, where, why
   --------   -------     -------------------------------------------------
    4/9/15    hbansal        Initial creation
   ======================================================================== */

/*------------------------------------------------------------------------
 * Include files and Macro definitions
 * -----------------------------------------------------------------------*/
#ifndef CAPI_V2_BYTESHIFTER_H
#define CAPI_V2_BYTESHIFTER_H

#include "Elite_CAPI_V2.h"


/*------------------------------------------------------------------------
* Function declarations
* -----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

capi_v2_err_t capi_v2_byteshifter_get_static_properties (capi_v2_proplist_t *init_set_properties, capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_byteshifter_init (capi_v2_t *_pif, capi_v2_proplist_t *init_set_properties);

#ifdef __cplusplus
}
#endif

#endif //CAPI_V2_BYTESHIFTER_H

