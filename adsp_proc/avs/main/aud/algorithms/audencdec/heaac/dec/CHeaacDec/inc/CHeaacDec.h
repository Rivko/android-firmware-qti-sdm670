/* ======================================================================== */
/**
   @file CHeaacDec.h

  Header file contains the function declarations to implement
  the CAPIv2 interface for Heaac decoder library.
  The functions in this file are called by decoder svc.
*/

/* =========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
   ========================================================================== */

/* =========================================================================
   Edit History

   when         who        what, where, why
   --------     ---        --------------------------------------------------
   04/14/15   adeepak      Initial creation
   ========================================================================= */

/*------------------------------------------------------------------------
 * Include files
 * -----------------------------------------------------------------------*/
#ifndef CHEAAACDEC_H
#define CHEAAACDEC_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#include "Elite_CAPI_V2.h"


/*------------------------------------------------------------------------
 * Function declarations
 * -----------------------------------------------------------------------*/

capi_v2_err_t heaac_dec_capi_v2_get_static_properties (
        capi_v2_proplist_t *init_set_properties,
        capi_v2_proplist_t *static_properties);


capi_v2_err_t heaac_dec_capi_v2_init (
        capi_v2_t*              _pif,
        capi_v2_proplist_t      *init_set_properties);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // CHEAAACDEC_H
