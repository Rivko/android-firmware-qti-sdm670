/**
@file mm_iter.cpp***********************

@brief module manager main

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/module_mgmt/module_mgr/src/mm_main.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/12/2015  rbhatnk      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2015 Qualcomm Technologies, Incorporated.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/
#ifndef _MM_MAIN_H
#define _MM_MAIN_H

//Debug messages
//#define MEDIUM_LOG
//#define VERBOSE
//#define LOC_FILE_BUF_LOGS

#include "adsp_amdb_api.h"

#include "module_mgr.h"

#include "Elite_CAPI_V2.h"
#include "qurt_elite_types.h"

#ifdef STAND_ALONE

#include "test_utils.h"
#include "adsp_error_codes.h"
#include "test_main.h"


#else //STAND_ALONE

#include "qurt_elite.h"
#include "adsp_amdb_static.h"
#include "adsp_amdb_internal.h"

#endif //STAND_ALONE

extern "C" {

#define ADSP_EEOF ADSP_ETERMINATED

#define VERSION_SECTION_MASK 0x1
#define BUILT_IN_SECTION_MASK 0x2
#define MREG_INFO_SECTION_MASK 0x4

#define SECTION_ID_UNKNOWN 0
#define SECTION_ID_VER 1
#define SECTION_ID_BUILTIN 2
#define SECTION_ID_MREG 3

#define SEEK_FROM_FILE_BEGIN 0
#define SEEK_FROM_CURRENT 1

#define AMDB_INTERFACE_TYPE_STUB 3

typedef enum mm_module_source_t
{
   MM_MODULE_SOURCE_INVALID=0,
   MM_MODULE_SOURCE_ACDB_BUILT_IN,
   MM_MODULE_SOURCE_ACDB_MREG_INFO,
   MM_MODULE_SOURCE_REG_API_OUT_OF_BAND,
   MM_MODULE_SOURCE_REG_API_IN_BAND,
} mm_module_source_t;

typedef struct built_in_mod_info_t
{
   uint32_t mtype;
   uint32_t id1;
   uint32_t id2;
   uint16_t needed;
   uint16_t preload;
}built_in_mod_info_t;

/**
 * ftell: For binary streams, this is the number of bytes from the beginning of the file.
 *
 * to be in range [val_min...val_max] must lie inside [range_min,...,range_max]. at boundaries 2 can be equal.
 *
 */
static inline bool_t is_val_in_range(uint32_t val_min, uint32_t val_max, uint32_t range_min, uint32_t range_max)
{
   if (val_min > val_max) //val_min must be < val_max.
   {
      return FALSE;
   }

   if (range_min > range_max)
   {
      return FALSE;
   }

   if ( (val_min < range_min) || (val_max > range_max) )
   {
      return FALSE;
   }

   return TRUE;
}

/**
 * if return ADSP_ENOTIMPL => load all modules. this section is not present.
 */
ADSPResult mm_loop_through_modules_and_reg(void *rd_ptr, mm_module_source_t src, bool_t is_reg, uint32_t client_id, void* tracker);

ADSPResult mm_register_all_built_in();

ADSPResult mm_register_built_in_module(uint32_t entry_type, built_in_mod_info_t *mod_info, void* tracker);

void mm_remove_all_static_entries();
void *mm_built_in_registration_tracker_init();
void mm_built_in_registration_tracker_finalize(void *tracker, bool_t built_in_found);

ADSPResult mm_register_all_static_modules_with_entry_type(uint32_t entry_type);

void add_prereq_module_to_client_entry(uint32_t mtype, uint32_t id1, uint32_t id2, bool_t needed);


typedef struct mm_static_capi_v2_module_t
{
   uint32_t mtype;
   uint32_t id1;
   uint32_t id2;

   capi_v2_get_static_properties_f get_static_prop_fn;
   capi_v2_init_f                  init_fn;

} mm_static_capi_v2_module_t;

typedef struct mm_dynamic_capi_module_t
{

   uint32_t mtype;
   uint32_t id1;
   uint32_t revision;

   bool_t   preload;
   const char     *filename;
   const char     *tag;

} mm_dynamic_capi_module_t;

typedef struct mm_dynamic_capi_v2_module_t
{

   uint32_t mtype;
   uint32_t id1;
   uint32_t id2;
   uint32_t revision;

   bool_t   preload;
   const char     *filename;
   const char     *tag;

} mm_dynamic_capi_v2_module_t;

typedef struct mm_private_module_t
{
   uint32_t mtype;
   uint32_t id1;
   uint32_t id2;

   //revision not required as private modules are static only and only shared modules can have revisions
} mm_private_module_t;

//if this is changed, then MAX_PREREQUISITE_MIDS in avs/build/avs_libs_config_parser.py must also be changed.
#define MM_MAX_PREREQUISITE_MIDS 2

typedef struct mm_module_id_t
{
   uint32_t mtype;
   uint32_t id1;
   uint32_t id2;
   //revision is top available one
} mm_module_id_t;

typedef struct mm_prereq_mids_t
{
   mm_module_id_t id;
   mm_module_id_t prereq_mids[MM_MAX_PREREQUISITE_MIDS]; //invalid entry = id1=0, Eg. resampler, chmixer
}mm_prereq_mids_t;


extern const mm_static_capi_v2_module_t   mm_static_capi_v2_modules[];
extern const uint32_t                     mm_num_static_capi_v2_modules;

extern const mm_dynamic_capi_v2_module_t  mm_dynamic_capi_v2_modules[];
extern const uint32_t                     mm_num_dynamic_capi_v2_modules;

extern const mm_private_module_t          mm_private_modules[];
extern const uint32_t                     mm_num_private_modules;

extern const mm_prereq_mids_t             mm_prereq_mids[];
extern const uint32_t                     mm_num_prereq_mids;

extern const mm_module_id_t               mm_virtual_stub_modules[];
extern const uint32_t                     mm_num_virtual_stub_modules;

} //extern C

#endif //_MM_MAIN_H
