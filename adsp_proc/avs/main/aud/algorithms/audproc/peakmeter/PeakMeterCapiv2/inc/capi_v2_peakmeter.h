/* ======================================================================== */
/**
@file capi_v2_peakmeter.h

   Header file to implement the Common Audio Processor Interface for peakmeter module
*/

/* =========================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  ========================================================================== */

/* =========================================================================
                             Edit History

   when         who         what, where, why
   --------     ---         ------------------------------------------------------
   03/19/2016  adeepak      Initial creation
   ========================================================================= */

/*------------------------------------------------------------------------
 * Include files
 * -----------------------------------------------------------------------*/
#ifndef CAPI_V2_PEAKMETER_H
#define CAPI_V2_PEAKMETER_H

#include "Elite_CAPI_V2.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * Function declarations
 * -----------------------------------------------------------------------*/
/**
 * Get static properties of peakmeter module
 */
capi_v2_err_t capi_v2_peakmeter_get_static_properties(
        capi_v2_proplist_t *init_set_properties,
        capi_v2_proplist_t *static_properties);

/**
 * Instantiates (and allocates) the module for peakmeter module
 */
capi_v2_err_t capi_v2_peakmeter_init(
        capi_v2_t          *_pif,
        capi_v2_proplist_t *init_set_properties);

#ifdef __cplusplus
}
#endif

#endif //CAPI_V2_PEAKMETER_H

