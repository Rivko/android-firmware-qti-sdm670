#ifndef CAPI_V2_HPCM_STREAM_RX_H_
#define CAPI_V2_HPCM_STREAM_RX_H_

/* ========================================================================
   Host-PCM (HPCM) CAPI_V2 header file

  *//** @file capi_v2_hpcm_stream_rx.h
  This is a wrapper code for HPCM library.
  The functions in this file are called from Vdec service.

  Copyright (c) 2016 QUALCOMM Technologies Incorporated.
  All Rights Reserved. Qualcomm Proprietary and Confidential.
  *//*====================================================================== */

/* =========================================================================
                             Edit History

   when       who       what, where, why
   --------   ---       ----------------------------------------------------
   3/7/16   pgurram   creation
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

capi_v2_err_t capi_v2_hpcm_stream_rx_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_hpcm_stream_rx_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* CAPI_V2_HPCM_STREAM_RX_H_ */
