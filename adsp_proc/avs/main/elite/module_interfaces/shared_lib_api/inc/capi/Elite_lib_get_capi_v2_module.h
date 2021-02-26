#ifndef ELITE_LIB_GET_CAPI_V2_MODULE_H
#define ELITE_LIB_GET_CAPI_V2_MODULE_H

/**
  @file Elite_lib_get_capi_v2_module.h

  @brief A CAPIv2 supporting library for creating other CAPIv2 modules
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. The description
      after the Edit History below is pulled in by the Elite_CAPIv2_mainpage.
      dox file and is displayed in the PDF. Contact Tech Pubs for assistance.
===========================================================================*/
/*=============================================================================
  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*=============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/capi/Elite_lib_get_capi_v2_module.h#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  06/05/15    sw      (Tech Pubs) Edited Doxygen markup/comments for 8952 (2.6)
  12/29/14    deepang   Created file.
=============================================================================*/

/** @weakgroup weakf_capiv2_lib_mod_intro

The Module Creation library is used to create instances of other CAPIv2 modules
that are available in the system.

When working with CAPI_V2 modules, use the following table to determine the
values of id1 and id2.

@lib_mod_creation_table{
Generic    & Set module ID & Set to zero   @tblendline
Encoder    & Output format & Set to zero   @tblendline
Decoder    & Input format  & Set to zero   @tblendline
Converter  & Input format  & Output format @tblendline
Packetizer & Input format  & Set to zero   @tblendline}
*/


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"


/** @addtogroup capiv2_lib_module
@{ */

/** Unique identifier to represent this library interface.*/
#define ELITE_LIB_GET_CAPI_V2_MODULE 0x00010914

/*------------------------------------------------------------------------------
 * Interface definition
 *----------------------------------------------------------------------------*/

typedef struct elite_lib_get_capi_v2_t elite_lib_get_capi_v2_t;
typedef struct elite_lib_capi_v2_handle elite_lib_capi_v2_handle;

/** Library type is Generic.

  Use this library type when there is no information about a module or when the
  module functionality is not one of the defined library types.
 */
#define ELITE_LIB_CAPI_V2_TYPE_GENERIC 0

/** Library type is a Decoder. */
#define ELITE_LIB_CAPI_V2_TYPE_DECODER 1

/** Library type is an Encoder. */
#define ELITE_LIB_CAPI_V2_TYPE_ENCODER 2

/** Library type is a Converter.

  For example, the converter might be a transcoder that converts one bitstream
  input format to another bitstream format.
  @if ISV_only @newpage @endif
 */
#define ELITE_LIB_CAPI_V2_TYPE_CONVERTER 3

/** Library type is a Packetizer.

  Data is packetized into one of the IEC 61937/60958 packetization formats.
  @newpage
 */
#define ELITE_LIB_CAPI_V2_TYPE_PACKETIZER 4


/** Virtual function table of the library interface.

  To use this library:
   -# Get an object of type elite_lib_get_capi_v2_t using
      #CAPI_V2_EVENT_GET_LIBRARY_INSTANCE.
   -# Call get_handle with the appropriate values of type, id1, and id2 to get
      a handle that can be used to call the capi_v2 get_static_properties()
      and init() functions.
      - Use the handle to call the get_static_properties() of the module
        using call_get_static_properties().
      - Use the handle to call the init() function of the module using
        call_init().
   -# Create multiple instances of the module with multiple calls to init()
      (using call_init()).
   -# After the required instances are created, release the handle using
      release_handle().
      - Do not use the handle after this call.
      - The module instances might continue to exist after the handle
        is released.
   -# Query multiple capi_v2 handles using a single library instance.
      Repeat steps 2 to 4 for all modules that are to be created.
   -# Once all the modules are created and all the handles are closed, destroy
      the library instance by calling b.end. The module instances might
      continue to exist after the library instance is destroyed.
 */
struct elite_lib_get_capi_v2_vtable
{
   capi_v2_library_base_t b;
   capi_v2_err_t (*get_handle)(elite_lib_get_capi_v2_t *obj_ptr, uint32_t type, uint32_t id1, uint32_t id2, elite_lib_capi_v2_handle** h_ptr);
   capi_v2_err_t (*call_get_static_properties)(elite_lib_get_capi_v2_t *obj_ptr, elite_lib_capi_v2_handle* h, capi_v2_proplist_t* init_set_properties, capi_v2_proplist_t* static_properties);
   capi_v2_err_t (*call_init)(elite_lib_get_capi_v2_t *obj_ptr, elite_lib_capi_v2_handle* h, capi_v2_t* _pif, capi_v2_proplist_t* init_set_properties);
   capi_v2_err_t (*release_handle)(elite_lib_get_capi_v2_t *obj_ptr, elite_lib_capi_v2_handle *h);
};

/** Contains a pointer to the CAPIv2 virtual function table
  defined in elite_lib_get_capi_v2_vtable.
 */
struct elite_lib_get_capi_v2_t
{
   const elite_lib_get_capi_v2_vtable *vtable;
   /**< Pointer to the virtual function table. */
};

/** @} *//* end_addtogroup capiv2_lib_module */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* #ifndef ELITE_LIB_GET_CAPI_V2_MODULE_H */
