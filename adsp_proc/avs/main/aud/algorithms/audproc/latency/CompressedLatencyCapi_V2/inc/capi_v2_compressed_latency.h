/* ======================================================================== */
/**
@file capi_v2_compressed_latency.h

   Header file to implement the Common Audio Post Processor Interface
   for Tx/Rx Tuning compressed_latency block
*/

/* =========================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  ========================================================================== */

/* =========================================================================
 * Edit History:
 * when         who         what, where, why
 * ----------   -------     -------------------------------------
 *  5/12/2016    rishabht     CAPI_V2 Interface for compressed_latency module
   ========================================================================= */

/*------------------------------------------------------------------------
 * Include files
 * -----------------------------------------------------------------------*/
#ifndef CAPI_V2_COMPRESSED_LATENCY_H
#define CAPI_V2_COMPRESSED_LATENCY_H

#include "Elite_CAPI_V2.h"
#include "mmdefs.h"

#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

/**
* Get static properties of compressed_latency module such as
* memory, stack requirements etc.
* See Elite_CAPI_V2.h for more details.
*/
capi_v2_err_t capi_v2_compressed_latency_get_static_properties(
      capi_v2_proplist_t *init_set_properties,
      capi_v2_proplist_t *static_properties);


/**
* Instantiates(and allocates) the module memory.
* See Elite_CAPI_V2.h for more details.
*/
capi_v2_err_t capi_v2_compressed_latency_init(
      capi_v2_t          *_pif,
      capi_v2_proplist_t *init_set_properties);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif //CAPI_V2_COMPRESSED_LATENCY_H