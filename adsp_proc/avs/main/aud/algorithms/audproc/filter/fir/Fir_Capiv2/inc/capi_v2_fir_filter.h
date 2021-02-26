/* ======================================================================== */
/**
   @file capi_v2_fir_filter.h

   Header file to implement the Common Audio Processor Interface v2 for
   Fir Filter Library
*/

/* =========================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
   ========================================================================== */

/* =========================================================================
   Edit History

   when         who        what, where, why
   --------     ---        --------------------------------------------------
   3/24/2016   pparamat      Initial creation
   ========================================================================= */

/*------------------------------------------------------------------------
 * Include files
 * -----------------------------------------------------------------------*/
#ifndef CAPI_V2_FIR_FILTER_H
#define CAPI_V2_FIR_FILTER_H

#include "Elite_CAPI_V2.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus


/*------------------------------------------------------------------------
 * Function declarations
 * -----------------------------------------------------------------------*/

capi_v2_err_t capi_v2_fir_get_static_properties (
        capi_v2_proplist_t *init_set_properties,
        capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_fir_init (
        capi_v2_t*              _pif,
        capi_v2_proplist_t      *init_set_properties);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif // CAPI_V2_FIR_FILTER_H
