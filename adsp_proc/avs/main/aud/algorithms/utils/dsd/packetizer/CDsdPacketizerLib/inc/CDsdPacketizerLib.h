#ifndef C_DSD_PACKETIZER_LIB_H
#define C_DSD_PACKETIZER_LIB_H

/* ========================================================================
  DSD Packetizer library wrapper header file

  @file CDsdPacketizerLib.h
  This is a wrapper code for DSD Core Packetizer library.

Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  ====================================================================== */

/* =========================================================================
                             Edit History

   when       who           what, where, why
   --------   ---           ------------------------------------------------
  11/09/15    YW            Created file.

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

capi_v2_err_t dsd_packetizer_capi_v2_get_static_properties (
        capi_v2_proplist_t *init_set_properties,
        capi_v2_proplist_t *static_properties);


capi_v2_err_t dsd_packetizer_capi_v2_init (
        capi_v2_t*              _pif,
        capi_v2_proplist_t      *init_set_properties);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* C_DSD_PACKETIZER_LIB_H */
