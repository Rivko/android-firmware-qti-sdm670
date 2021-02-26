#ifndef ADSP_AMDB_STATIC_H
#define ADSP_AMDB_STATIC_H
/*==============================================================================

Copyright (c) 2016 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.

==============================================================================*/

#include "qurt_elite_types.h"
#include "Elite_CAPI_V2.h"

#include "adsp_amdb_api.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

/******************************************************************************* 
 ADSP Audio Module DataBase (adsp_amdb) API Overview

 AMDB is a repository for CAPI based audio modules.  It allows both
 static and dynamic modules to be added, queried, and removed.
 
 Modules added to the database are tracked using numeric reference counting.
 The reference is incremented by one when the module is added to the database
 and for each time the module is successfully queried.  upon a successful query
 a pointer to the module is returned, the reference count can be incremented or
 decremented using the addref and release functions.  When no longer needed the
 reference count added when the module is queried must be released.
 
 For example:
 
 adsp_amdb_add_capi - sets ref count to 1
 adsp_amdb_get_capi() - increments reference count and returns pointer to module
 adsp_amdb_capi_addref - increments ref count
 adsp_amdb_capi_release - decrements ref count
 adsp_amdb_capi_release - decrements ref count
 adsp_amdb_remove_capi - decreement ref count, count is no zero and module is
                         freed
 
 As long as a module is being used a reference count must be non-zero.
 
 Once a module is queried, that module can be used to create instances of
 the CAPI objects.  The modules reference count is incremented each time
 a CAPI object is created and decremented when the CAPI End()
 method is called.
 
 To create a CAPI object:
 
   call adsp_amdb_capi_new_f()
 
 Once a CAPI object is created that interface's End() method must be
 called to decrement the reference count.  If it is not then that object will
 never be freed.
 
 When dealing with dynamic audio modules the module will only be unloaded when
 all objects using it are freed (ref count goes to zero).
 
*******************************************************************************/

/******************************************************************************* 
  init/deinit, operates on a global instance of adsp_amdb
*******************************************************************************/
ADSPResult adsp_amdb_init(void);
void adsp_amdb_deinit(void);

/******************************************************************************* 
  amdb modules types
*******************************************************************************/
typedef struct adsp_amdb_capi_v2_t adsp_amdb_capi_v2_t;
typedef struct adsp_amdb_stub_t adsp_amdb_stub_t;
/******************************************************************************* 
  creates new CAPI object
*******************************************************************************/
capi_v2_err_t adsp_amdb_capi_v2_get_static_properties_f(adsp_amdb_capi_v2_t* me,
                                                     capi_v2_proplist_t* init_set_properties,
                                                     capi_v2_proplist_t* static_properties);
capi_v2_err_t adsp_amdb_capi_v2_init_f(adsp_amdb_capi_v2_t* me,
                                    capi_v2_t* _pif,
                                    capi_v2_proplist_t* init_set_properties);


/******************************************************************************
 * Addition of special types of module entries for CAPIv2 modules
******************************************************************************/
// The entry type with the lowest value has the highest priority
static const uint32_t ADSP_AMDB_ENTRY_TYPE_LEGACY = 0x10; // Entries made using the old API.
static const uint32_t ADSP_AMDB_ENTRY_TYPE_CLIENT = 0x20; // Entries set by the client processor.
static const uint32_t ADSP_AMDB_ENTRY_TYPE_INIT   = 0x30; // Entries set during DSP boot.
static const uint32_t ADSP_AMDB_ENTRY_TYPE_STATIC = 0x40; // Entries set before DSP boot.


/***************************************************************************
 API for querying for module handles.
 ***************************************************************************/

// Interface type of the module present in the AMDB
enum adsp_amdb_interface_type
{
   CAPI_V2 = 2,
   STUB,
   INVALID = 0x7FFFFFFF
};

// Handle information that is provided by the AMDB per module.
typedef struct adsp_amdb_module_handle_info_t
{
   adsp_amdb_interface_type interface_type; // To be filled by AMDB. AMDB will return the interface type of the module if it returns a valid handle.
   int type; // To be filled by the caller for CAPIv2 modules. Should be set to 0 for all other types.
   int id1; // To be filled by the caller for CAPI and CAPIv2 modules.
   int id2; // To be filled by the caller for CAPIv2 modules. Should be set to 0 for all other types.
   union adsp_amdb_handle_union
   {
      // To be filled by AMDB. Only one of the following values will be valid, based on the interface type.
      adsp_amdb_capi_v2_t *capi_v2_handle;
      adsp_amdb_stub_t *stub_handle;
   } h;
   ADSPResult result; // To be filled by AMDB. If result is not ADSP_EOK, the handle is not valid.
} adsp_amdb_module_handle_info_t;

// Function signature for the callback function that is called when all the module handles are ready.
typedef void (*adsp_amdb_get_modules_callback_f)(void *callback_context);

void adsp_amdb_get_modules_request(
      adsp_amdb_module_handle_info_t module_handle_info[], // An array of num_modules elements, for the modules whose handles are required.
      uint32_t num_modules, // Number of modules for which handles are being queried.
      adsp_amdb_get_modules_callback_f callback_function, // The callback function that is called when the handles are available for all modules. If NULL, this function will be blocking.
      void *callback_context); // Context to be passed to the callback function.

void adsp_amdb_release_handles(
      adsp_amdb_module_handle_info_t module_handle_info[], // Array of num_modules_elements, whose handles are to be released. If the handle value is NULL or if the 'result' value is not EOK, the corresponding entry will be ignored.
      uint32_t num_modules // Number of modules whose handles are to be released.
      );

// function to get the dlinfo for any module. only dynamically loaded modules have dlinfo.
void adsp_amdb_get_dl_info(adsp_amdb_module_handle_info_t *module_handle_info, //handle to the amdb.
      bool_t *is_dl, //is module dynamically loaded.
      uint32_t **start_addr, //start address (virtual) where the lib is loaded.
      uint32_t *so_size);

// Function prints all the capiv2 modules
ADSPResult adsp_amdb_print_all_capi_v2(void);
    
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // ADSP_AMDB_H
