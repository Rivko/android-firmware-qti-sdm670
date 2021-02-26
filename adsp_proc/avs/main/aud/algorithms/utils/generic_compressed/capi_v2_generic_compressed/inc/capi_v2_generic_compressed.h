#ifndef C_RAW_PASSTHRU_LIB_H
#define C_RAW_PASSTHRU_LIB_H

/* ========================================================================
  Compressed Raw Passthrough library wrapper header file

  @file CRawPassthruLib.h
  This is a wrapper code for Compressed Raw Passthrough library.

Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  ====================================================================== */

/* =========================================================================
                             Edit History

   when       who           what, where, why
   --------   ---           ------------------------------------------------
  05/06/16    YW            Created file.

   ========================================================================= */

/*------------------------------------------------------------------------
 * Include files
 * -----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#include "Elite_CAPI_V2.h"


/*------------------------------------------------------------------------
 * Function declarations
 * -----------------------------------------------------------------------*/

capi_v2_err_t raw_passthru_capi_v2_get_static_properties (
        capi_v2_proplist_t *init_set_properties,
        capi_v2_proplist_t *static_properties);


capi_v2_err_t raw_passthru_capi_v2_init (
        capi_v2_t*              _pif,
        capi_v2_proplist_t      *init_set_properties);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* C_RAW_PASSTHRU_LIB_H */
