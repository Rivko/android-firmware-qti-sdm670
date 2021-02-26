#ifndef ADSP_AMDB_INTERNAL_H
#define ADSP_AMDB_INTERNAL_H
/*==============================================================================

Copyright (c) 2016 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.

==============================================================================*/



#include "adsp_amdb_static.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus



/******************************************************************************* 
  init/deinit, operates on a global instance of adsp_amdb
*******************************************************************************/
ADSPResult adsp_amdb_init(void);
void adsp_amdb_deinit(void);


/*******************************************************************************
  Add a static module to the database
*******************************************************************************/
ADSPResult adsp_amdb_add_static_capi_v2(uint32_t entry_type, int type, int id1, int id2,
                                        capi_v2_get_static_properties_f get_static_properties_f,
                                        capi_v2_init_f init_f);


// In the calls below, calls with the same client id are not thread-safe.

ADSPResult adsp_amdb_add_capi_v2_with_entry_type(uint32_t entry_type, int type, int id1, int id2,
      boolean preload, const char* filename_str, const char* tag_str, ADSPResult *dynamic_loading_err_ptr = NULL, uint32_t client_id = 0);
ADSPResult adsp_amdb_remove_capi_v2_with_entry_type(uint32_t entry_type, int type, int id1, int id2);

ADSPResult adsp_amdb_add_stub_with_entry_type(uint32_t entry_type, int type, int id1, int id2, ADSPResult *dynamic_loading_err_ptr = NULL, uint32_t client_id = 0);
ADSPResult adsp_amdb_remove_stub_with_entry_type(uint32_t entry_type, int type, int id1, int id2);
ADSPResult adsp_amdb_remove_with_entry_type(uint32_t entry_type, int type, int id1, int id2);


/*******************************************************************************
  When working with CAPI_V2 modules use the following table to
  determine the values of id1 and id2

  Type       | id2           | id1
  -----------+---------------+---------------
  Generic    | Module id Set | Set to zero
  Encoder    | Output format | Set to zero
  Decoder    | Input format  | Set to zero
  Converter  | Input format  | Output format
  Packetizer | Input format (or module-id) | Set to zero
  Depacketizer | Module-id | Set to zero

*******************************************************************************/

/*****************************************************************************
    All modules of a particular entry type can be disabled.
    This will prevent any future calls of adsp_amdb_get_modules_request
    from getting modules of this entry type.
*****************************************************************************/
void adsp_amdb_disable_entry_type(uint32_t entry_type);
void adsp_amdb_enable_entry_type(uint32_t entry_type);


/*****************************************************************************
    This function returns the aggregate error code for all the module
    additions for this client since the last time this function was called
    Per module error codes can be obtained by providing the
    dynamic_loading_err_ptr values in the add APIs.
*****************************************************************************/
ADSPResult adsp_amdb_wait_for_module_additions(uint32_t client_id = 0);


/***************************************************************************** 
    Add a dynamic CAPI_V2 module to the database
   
    type - type of module to be added (encoder, decoder, etc...)
    id1, id2 - depends on type
    preload - true if the shared object should be loaded during the add process
              false if the shared object should be loaded only when a module
              is queried that requires it
    filename_str - name of the shared object file (absolute path)
    getsize/getstatic properties_str - name of the module's getsize() or
                                       get_static_properties_str()
    ctor/init_str - name of the module's ctor() or init() function
*****************************************************************************/
ADSPResult adsp_amdb_add_capi_v2(int type, int id1, int id2, boolean preload, const char* filename_str, const char* get_static_properties_str, const char* init_str);


/***************************************************************************** 
    Remove an CAPI_V2 module from the database.  if
    filename_str is an empty string ("") then the module matching the type/ids
    will be removed.  Otherwise all the CAPI_V2 associated with the
    shared object listed in filename_str will be removed.

    type - type of module to be added
    id1, id2 - depends on type
    filename_str - name of the shared object, must not be NULL
*****************************************************************************/
ADSPResult adsp_amdb_remove_capi_v2(int type, int id1, int id2, const char* filename_str);


/***************************************************************************** 
    Remove all CAPI_V2 modules from the database (static modules are not removed)
*****************************************************************************/
ADSPResult adsp_amdb_remove_all_capi_v2(void);
    
    
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // ADSP_AMDB_INTERNAL_H
