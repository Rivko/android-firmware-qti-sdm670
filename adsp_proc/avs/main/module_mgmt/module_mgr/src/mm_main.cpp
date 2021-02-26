/**
@file mm_iter.cpp***********************

@brief module manager main

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/module_mgmt/module_mgr/src/mm_main.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/12/2015  rbhatnk      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2015 Qualcomm Technologies, Incorporated.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/

#include "mm_main.h"
#include "mm_acdb_file_reader.h"
#include "mm_mem_blob_reader.h"
#include "adsp_core_api.h"
#ifndef STAND_ALONE
#include "adsp_amdb_static.h"
#include "adsp_amdb_internal.h"
#endif

static ADSPResult handle_reg_dereg_api(bool_t is_register, void *addr, uint32_t size, bool_t is_out_of_band, uint32_t client_id)
{
   ADSPResult result = ADSP_EOK;

   //create reader for reading the memory
   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"Calling mem_blob_reader_create");
   mem_blob_reader_t *mem_blob_rdr = NULL;
   result = mem_blob_reader_create(&mem_blob_rdr, addr, size);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"mem_blob_reader_create failed %d. ", result);
      return result;
   }

   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"Done mem_blob_reader_create");

   MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"Looping through REG_API is_register %u", is_register);

   mm_module_source_t src=is_out_of_band ? MM_MODULE_SOURCE_REG_API_OUT_OF_BAND : MM_MODULE_SOURCE_REG_API_IN_BAND;
   result = mm_loop_through_modules_and_reg((void*)mem_blob_rdr, src, is_register, client_id, NULL);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"Looping through REG_API failed 0x%x", result);
      goto _bailout;
   }

_bailout:
   (void)mem_blob_reader_destroy(&mem_blob_rdr);

   if (is_register)
   {
      ADSPResult dyn_loading_result = adsp_amdb_wait_for_module_additions(client_id);
      if (ADSP_FAILED(dyn_loading_result))
      {
         result = dyn_loading_result;
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Module_mgr: Addition of modules in REG_API returned with error code %d", result);
      }
   }

   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"Done mem_blob_reader_destroy");

   return result;
}

/**
 * all the entries go to STATIC entry.
 */
ADSPResult mm_register_all_static_modules()
{
   return mm_register_all_static_modules_with_entry_type(ADSP_AMDB_ENTRY_TYPE_STATIC);
}

ADSPResult mm_register_modules_based_on_acdb()
{
   ADSPResult result = ADSP_EOK;
   bool_t built_in_found = true;
   bool_t init_entry_populated = false;

   uint64_t start_time = qurt_elite_timer_get_time_in_msec();

   adsp_amdb_disable_entry_type(ADSP_AMDB_ENTRY_TYPE_INIT);
   //create reader for reading meta-info-file (ACDB file)
   //if creation failed due to absent meta-info-file, then load all modules.
   //if it failed due to any other reason, error-out
   //if it succeeded, load as per this file.
   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"Calling acdb_file_reader_create");
   acdb_file_reader_t *acdb_file_rdr = NULL;
   result = acdb_file_reader_create(&acdb_file_rdr);
   if (result == ADSP_ENOTIMPL)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"acdb_file_reader_create failed due to absent meta-info-file or absent sections. Registering all built-in modules.");
      result = mm_register_all_built_in();
      ADSPResult result1 = adsp_amdb_wait_for_module_additions();
      if (ADSP_FAILED(result1))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Dynamic loading of some BUILT_IN modules failed %d", result1);
         result = result1;
      }
      init_entry_populated = true;
      goto _bailout;
   }
   else if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"acdb_file_reader_create failed %d. Not loading any module", result);
      //if acdb file open failed, then let static entry stay, so dont mark init_entry_populated
      goto _bailout;
   }

   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"Done acdb_file_reader_create");

   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"Looping through MREG_INFO");

   //do mreginfo first because it can have overrides.
   result = mm_loop_through_modules_and_reg((void*)acdb_file_rdr, MM_MODULE_SOURCE_ACDB_MREG_INFO, TRUE, AVCS_AMDB_CLIENT_ID_NONE, NULL);
   if (result == ADSP_ENOTIMPL)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"MREG_INFO not present");
      result = ADSP_EOK;
   }
   else if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Looping through MREG_INFO failed %d. Continuing to built-in.", result);
   }


   {
      //to create memory for logging which module got loaded etc.
      void *tracker = mm_built_in_registration_tracker_init();

      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"Looping through BUILT_IN");

      result = mm_loop_through_modules_and_reg((void*)acdb_file_rdr, MM_MODULE_SOURCE_ACDB_BUILT_IN, TRUE, AVCS_AMDB_CLIENT_ID_NONE, tracker);
      if (result == ADSP_ENOTIMPL)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"built-in not present. Registering all built-in");
         built_in_found = false;
         result = mm_register_all_built_in();
      }
      else if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Looping through BUILT_IN failed %d", result);
         //if ACDB file is present & built-in section exists, but built-in section reading failed, then assume init_entry_populated is true.
      }


      ADSPResult result1 = adsp_amdb_wait_for_module_additions();
      if (ADSP_FAILED(result1))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Dynamic loading of some BUILT_IN modules failed %d", result1);
         result = result1;
         //init_entry_populated -> true even if module addition failed, because acdb file was present & built-in section also present.
      }

      //to log error for modules not present in ACDB.Also loads private, dependent modules. adds stubs for modules not found in ACDB.
      mm_built_in_registration_tracker_finalize(tracker, built_in_found);

      (void)acdb_file_reader_destroy(&acdb_file_rdr);

      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"Done acdb_file_reader_destroy");
      init_entry_populated = true;

   }

_bailout:
   adsp_amdb_enable_entry_type(ADSP_AMDB_ENTRY_TYPE_INIT);
   if (init_entry_populated)
   {
      mm_remove_all_static_entries(); //once INIT entry is enabled, remove static entry.
   }
   uint64_t end_time = qurt_elite_timer_get_time_in_msec();
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "mm_register_modules_based_on_acdb : time taken %lu ms", (uint32_t)(end_time-start_time));

   return result;
}

ADSPResult mm_register_modules_from_reg_api(void *addr, uint32_t size, bool_t is_out_of_band, uint32_t client_id)
{
   ADSPResult result = ADSP_EOK;

   result = handle_reg_dereg_api(TRUE, addr, size, is_out_of_band, client_id);

   return result;
}

ADSPResult mm_remove_modules_from_dereg_api(void *addr, uint32_t size, bool_t is_out_of_band, uint32_t client_id)
{
   ADSPResult result = ADSP_EOK;

   result = handle_reg_dereg_api(FALSE, addr, size, is_out_of_band, client_id);

   return result;
}
