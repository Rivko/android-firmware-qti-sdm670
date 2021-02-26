#ifndef C_ETSI_AMRWBPLUS_DEC_LIB_H
#define C_ETSI_AMRWBPLUS_DEC_LIB_H

/* =========================================================================
Copyright (c) 2009-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =========================================================================*/

/**
 * @file CEtsiAmrWbPlusDec.h
 * ETSI AmrWbPlus decoder library wrapper header file
 *
 * This is a wrapper code for ETSI AmrWbPlus Core decoder library.
 * the function in this file are called by the CEtsiAmrWbPlusDec media
 * module. It is derived from CAudioProcLib class
 */

/* =========================================================================
 * Edit History
 *
 * when        who          what, where, why
 * ---------   ---          ------------------------------------------------
 * 11/2/2015   snaginen     CAPI V2 implementation of ETSI AmrWbPlus Decoder
 *
 * =========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif

#include "Elite_CAPI_V2.h"

/**
 * Get static properties of a module such as
 * memory, stack requirements etc.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t __attribute__ ((visibility ("default"))) amrwbplus_dec_capi_v2_get_static_properties (
   capi_v2_proplist_t   *init_set_properties,
   capi_v2_proplist_t   *static_properties);


/**
 * Instantiates(and allocates) the module memory.
 * See Elite_CAPI_V2.h for more details.
 */
capi_v2_err_t __attribute__ ((visibility ("default"))) amrwbplus_dec_capi_v2_init (
   capi_v2_t            *_pif,
   capi_v2_proplist_t   *init_set_properties);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* C_ETSI_AMRWBPLUS_DEC_LIB_H */

