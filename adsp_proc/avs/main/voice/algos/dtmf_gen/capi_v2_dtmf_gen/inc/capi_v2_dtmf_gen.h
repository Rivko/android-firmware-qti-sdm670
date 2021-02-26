#ifndef CAPI_V2_DTMFGEN_H_
#define CAPI_V2_DTMFGEN_H_

/* ========================================================================
   DTMF generation CAPI_V2 header file

  *//** @file capi_v2_dtmf_gen.h
  This is a wrapper code for DTMF generation library.
  the function in this file are called by Venc Service.

  Copyright (c) 2016 QUALCOMM Technologies Incorporated.
  All Rights Reserved. Qualcomm Proprietary and Confidential.
  *//*====================================================================== */

/* =========================================================================
                             Edit History

   when       who       what, where, why
   --------   ---       ----------------------------------------------------
   01/08/16   shridhar  Creation.
   ========================================================================= */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus



/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

capi_v2_err_t capi_v2_dtmf_gen_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_dtmf_gen_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* CAPI_V2_DTMFGEN_H_ */
