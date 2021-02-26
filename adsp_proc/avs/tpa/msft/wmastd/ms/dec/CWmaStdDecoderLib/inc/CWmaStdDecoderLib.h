/* ========================================================================
   WMA Std decoder library wrapper header file

  *//** @file CWmaStdDecoderLib.h
  This is a wrapper code for WMA Standard decoder library, and functions
  in this are called by Decoder service
 

  Copyright (c) 2014 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/* =========================================================================
                             Edit History

   when       who              what, where, why
   --------   ---           ------------------------------------------------------
  05/18/09    dp            copy/paste/replace from Wma Pro
  05/20/10    SarithaJ      Added additions for AudioLite
                            Made it more close to standalone setup
  16/12/14    aabraham      CAPIV2 port of Wma std decoder                             
   ========================================================================= */
   
#ifndef WMASTD_CAPIV2_HEADER_H_
#define WMASTD_CAPIV2_HEADER_H_


#include "Elite_CAPI_V2.h"
#include "mmdefs.h"

#ifdef  __cplusplus
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

capi_v2_err_t __attribute__ ((visibility ("default"))) wmastd_dec_capi_v2_get_static_properties (
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

            
capi_v2_err_t __attribute__ ((visibility ("default"))) wmastd_dec_capi_v2_init (
            capi_v2_t *pif,
            capi_v2_proplist_t *init);
            
#ifdef __cplusplus            
   }    // extern "C"
#endif    
   
            
#endif // WMASTD_CAPIV2_HEADER_H_   