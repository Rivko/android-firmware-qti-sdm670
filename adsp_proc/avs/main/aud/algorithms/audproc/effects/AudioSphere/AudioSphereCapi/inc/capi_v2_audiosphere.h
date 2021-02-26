/* ======================================================================== */
/**
  @file capi_v2_audiosphere.h

  Header file to implement the Audio Post Processor Interface for
  AudioSphere
  */

/* =========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
   ========================================================================== */

/* =========================================================================
   Edit History

   when       who     what, where, why
   --------   ---     ------------------------------------------------------
   01/27/15   BV       Initial creation
   ========================================================================= */

/*------------------------------------------------------------------------
 * Include files
 * -----------------------------------------------------------------------*/
#ifndef CAPI_V2_AUDIOSPHERE_H
#define CAPI_V2_AUDIOSPHERE_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
* Macros, Defines, Type declarations
 * -----------------------------------------------------------------------*/


/*------------------------------------------------------------------------
* Function declarations
* -----------------------------------------------------------------------*/

capi_v2_err_t __attribute__ ((visibility ("default"))) capi_v2_audiosphere_get_static_properties (capi_v2_proplist_t *init_set_properties,
                                                                                                  capi_v2_proplist_t *static_properties);

capi_v2_err_t __attribute__ ((visibility ("default"))) capi_v2_audiosphere_init (capi_v2_t*              _pif,
                                                                                 capi_v2_proplist_t      *init_set_properties);


#ifdef __cplusplus
}
#endif

#endif // CAPI_V2_AUDIOSPHERE_H
