/* ======================================================================== */
/**
@file capi_v2_multistageiir.h

   Header file to implement the CAPI_V2 Interface for Multi-Stage IIR filter
   object (MSIIR).
*/

/* =========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  ========================================================================== */

/* =========================================================================
                             Edit History

   when       who        what, where, why
   --------   ---        ------------------------------------------------------
   02/16/15   shridhar   created file
   ========================================================================= */

/*------------------------------------------------------------------------
 * Include files
 * -----------------------------------------------------------------------*/
#ifndef __CAPI_V2_MULTISTAGEIIR_H
#define __CAPI_V2_MULTISTAGEIIR_H

#include "Elite_CAPI_V2.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Get static properties of multistage IIR module such as
 * memory, stack requirements etc.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t capi_v2_multistageiir_get_static_properties(
      capi_v2_proplist_t* init_set_properties,
      capi_v2_proplist_t* static_properties);

/**
 * Instantiates(and allocates) the module memory.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t capi_v2_multistageiir_init(
      capi_v2_t*          _pif,
      capi_v2_proplist_t* init_set_properties);

capi_v2_err_t capi_v2_multistageiir_left_init(
      capi_v2_t*          _pif,
      capi_v2_proplist_t* init_set_properties);

capi_v2_err_t capi_v2_multistageiir_right_init(
      capi_v2_t*          _pif,
      capi_v2_proplist_t* init_set_properties);

capi_v2_err_t capi_v2_multistageiir_left_get_static_properties(
      capi_v2_proplist_t* init_set_properties,
      capi_v2_proplist_t* static_properties);

capi_v2_err_t capi_v2_multistageiir_right_get_static_properties(
      capi_v2_proplist_t* init_set_properties,
      capi_v2_proplist_t* static_properties);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__CAPI_V2_MULTISTAGEIIR_H

