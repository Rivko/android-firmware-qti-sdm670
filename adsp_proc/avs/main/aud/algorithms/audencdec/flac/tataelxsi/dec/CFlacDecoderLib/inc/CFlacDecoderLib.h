/* ========================================================================
   Flac decoder library wrapper header file

  *//** @file CFlacDecoderLib.h
  This is a wrapper code for Flac Core decoder library.
  the function in this file are called by the CFlacDecoder media module.
  It is derived from CAudioProcLib class

Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/* =========================================================================
                             Edit History

   when       who            what, where, why
   --------   --------     -- ----------------------------------------------
   11/24/14   vnadipal        created for CAPI V2 for flac decoder

   ========================================================================= */



/* =======================================================================
 *                       DEFINITIONS AND DECLARATIONS
 * ====================================================================== */

#ifndef FLAC_CAPIV2_HEADER_H_
#define FLAC_CAPIV2_HEADER_H_


#include "Elite_CAPI_V2.h"
#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup
Used to query for static properties of the module that are independent of the
instance. This function is used to query the memory requirements of the module
in order to create an instance. The same properties that are sent to the module
in the call to init() are also sent to this function to enable the module to
calculate the memory requirement.

@param[in]  init_set_properties  The same properties that will be sent in the
                         call to the init() function.
@param[out] static_properties   Pointer to the structure that the module must
                         fill with the appropriate values based on the property
                         id.

@return
Indication of success or failure.

@dependencies
None.
 */
capi_v2_err_t __attribute__ ((visibility ("default"))) flac_dec_capi_v2_get_static_properties (
      capi_v2_proplist_t *init_set_properties,
      capi_v2_proplist_t *static_properties);


/** @ingroup
Instantiates the module to set up the virtual function table, and also
allocates any memory required by the module. States within the module must
be initialized at the same time.

@datatypes
capi_v2_t \n
capi_v2_proplist_t \n
capi_v2_event_cb \n
void *

@param[in,out] _pif            Pointer to the module object.
@param[in]    init_set_properties Properties set by the service to be used
                              while init().

@return
Indication of success or failure.

@dependencies
 */
capi_v2_err_t __attribute__ ((visibility ("default"))) flac_dec_capi_v2_init (
      capi_v2_t*              _pif,
      capi_v2_proplist_t      *init_set_properties);

#ifdef __cplusplus
} //extern "C"
#endif

#endif /* FLAC_CAPIV2_HEADER_H_ */