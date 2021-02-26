/* ======================================================================== */
/**
@file capi_v2_swmad.h

   Header file to implement the Common Audio Processor Interface V2(CAPI_V2)
   for swmad algorithm   */

/* =========================================================================
    Copyright (c) 2013 Qualcomm  Technologies, Inc (QTI).  All rights reserved.
   Qualcomm Technologies Proprietary and Confidential.
  ========================================================================== */


/*------------------------------------------------------------------------
 * Include files
 * -----------------------------------------------------------------------*/
#ifndef __CAPI_V2_SWMAD_H
#define __CAPI_V2_SWMAD_H

#include "Elite_CAPI_V2.h"
/*------------------------------------------------------------------------
 * Macros, Defines, Type declarations
 * -----------------------------------------------------------------------*/

#define CAPI_V2_SWMAD_CFG_STATUS_PARAM_ID   0x1000

typedef struct capi_v2_swmad_cfg_status_t
{
  bool_t cfg_status;
}capi_v2_swmad_cfg_status_t;
/*------------------------------------------------------------------------
 * Function declarations
 * -----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

capi_v2_err_t capi_v2_swmad_get_static_properties (
                                                            capi_v2_proplist_t *init_set_prop_ptr,
                                                            capi_v2_proplist_t *static_prop_ptr);


capi_v2_err_t capi_v2_swmad_init (
                                           capi_v2_t*              _pif,
                                           capi_v2_proplist_t      *init_set_prop_ptr);
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif //__CAPI_V2_SWMAD_H

