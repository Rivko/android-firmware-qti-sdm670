/* ======================================================================== */
/**
@file capi_v2_acd.h

   Header file to implement the Common Audio Processor Interface V2(CAPI_V2)
   for voicewakeup algorithm   */

/* =========================================================================
  Copyright (c) 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  ========================================================================== */


/*------------------------------------------------------------------------
 * Include files
 * -----------------------------------------------------------------------*/
#ifndef __CAPI_V2_ACD_H
#define __CAPI_V2_ACD_H

#include "Elite_CAPI_V2.h"
/*------------------------------------------------------------------------
 * Macros, Defines, Type declarations
 * -----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Function declarations
 * -----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

capi_v2_err_t capi_v2_acd_get_static_properties (capi_v2_proplist_t *init_set_prop_ptr,
                                                 capi_v2_proplist_t *static_prop_ptr);


capi_v2_err_t capi_v2_acd_init (capi_v2_t* capi_v2_ptr,
                                capi_v2_proplist_t *init_set_prop_ptr);
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif //__CAPI_V2_ACD_H

