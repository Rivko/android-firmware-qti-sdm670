/* ========================================================================
   ComboMp3 decoder library wrapper header file

 *//** @file CMp3DecoderLib.h
  This is a wrapper code for 24 bit and 16 bit Mp3 decoder libraries.

Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/* =========================================================================
                             Edit History

   when       who            what, where, why
   --------   --------     -- ----------------------------------------------
   9/17/2014   rbhatnk        Combo CAPI V2 implementation of MP3

   ========================================================================= */


/* =======================================================================
 *                       DEFINITIONS AND DECLARATIONS
 * ====================================================================== */

#ifndef COMBO_MP3_CAPIV2_HEADER_H_
#define COMBO_MP3_CAPIV2_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Elite_CAPI_V2.h"
#include "mmdefs.h"


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
capi_v2_err_t combo_mp3_dec_capi_v2_get_static_properties (
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
capi_v2_err_t combo_mp3_dec_capi_v2_init (
      capi_v2_t*              _pif,
      capi_v2_proplist_t      *init_set_properties);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* COMBO_MP3_CAPIV2_HEADER_H_ */
