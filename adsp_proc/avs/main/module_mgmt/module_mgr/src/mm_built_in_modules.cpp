/**
@file mm_iter.cpp***********************

@brief module manager AMDB iterface

 */

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/module_mgmt/module_mgr/src/mm_built_in_modules.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/12/2015  rbhatnk      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2015-2016 Qualcomm Technologies, Incorporated.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/

#include "mm_main.h"
#ifndef STAND_ALONE
#include "DALSys.h"
#endif

/* NOTE : Type 0 was used for CAPI and 1 for APPI which are no longer supported */
static const uint32_t DEV_CFG_TYPE_CAPI_V2 = 2;

#define CHECK_RET_DAL(x) \
      { \
   DALResult res = (x); \
   if (DAL_SUCCESS != res) \
   { \
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed in DAL call. Error code = %d", res); \
      return ADSP_EFAILED; \
   } \
      }

#define VERIFY_RET(x) \
      { \
   if (!(x)) \
   { \
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Condition check failed"); \
      return ADSP_EFAILED; \
   } \
      }

static bool_t check_file_exists(const char *filename)
{
   FILE *fp = fopen(filename, "rb");
   if (fp)
   {
      fclose(fp);
      return true;
   }
   return false;
}

#ifndef STAND_ALONE
static ADSPResult register_one_from_xml(DALSYSPropertyHandle hProps, uint32_t index)
{
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Adding dynamic module %lu to the AMDB", index);
   ADSPResult result = ADSP_EOK;

   const uint32_t buffer_size = sizeof ("CAPIV2TYPE_") /* The longest token */
                                 + 3 /* 3 digits for the index */
                                 + 1; /* NULL character */
   char string[buffer_size];
   DALSYSPropertyVar PropVar;

   snprintf(string, buffer_size, "URI_%lu", index);
   CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
   VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_STR_PTR);
   const char *uri = PropVar.Val.pszVal;

   snprintf(string, buffer_size, "TYPE_%lu", index);
   CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
   VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);
   const uint32_t type = PropVar.Val.dwVal;

   snprintf(string, buffer_size, "PRELOAD_%lu", index);
   CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
   VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);
   const bool_t preload = (PropVar.Val.dwVal == 0) ? FALSE : TRUE;

   switch(type)
   {
   case DEV_CFG_TYPE_CAPI_V2:
   {
      snprintf(string, buffer_size, "CAPIV2TYPE_%lu", index);
      CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
      VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);
      const uint32_t type = PropVar.Val.dwVal;

      snprintf(string, buffer_size, "ID1_%lu", index);
      CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
      VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);
      const uint32_t id1 = PropVar.Val.dwVal;

      snprintf(string, buffer_size, "ID2_%lu", index);
      CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
      VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);
      const uint32_t id2 = PropVar.Val.dwVal;

      snprintf(string, buffer_size, "TAG_%lu", index);
      CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
      VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_STR_PTR);
      const char *tag = PropVar.Val.pszVal;

      result = adsp_amdb_add_capi_v2_with_entry_type(ADSP_AMDB_ENTRY_TYPE_INIT, type, id1, id2, preload, uri, tag);
      if (ADSP_FAILED(result))
      {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to add dev cfg CAPI V2 module %lx, 0x%lx, 0x%lx. result %d. (ok when overridden)",
               type, id1, id2, result);
      }
   }
   break;
   default:
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed with invalid module type %lu.", type);
      result = ADSP_EBADPARAM;
   }

   return result;
}
#endif //STAND_ALONE

static ADSPResult register_all_from_dev_cfg_xml()
{
   ADSPResult result = ADSP_EOK;
#ifndef STAND_ALONE
   DALSYSPropertyVar PropVar;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProps);

   CHECK_RET_DAL(DALSYS_GetDALPropertyHandleStr("dynamic_modules", hProps));
   CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, "NumModules", 0, &PropVar));
   VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);

   uint64_t start_time=qurt_elite_timer_get_time_in_msec();
   uint32_t num_dev_cfg_modules = PropVar.Val.dwVal;
   uint32_t num_failed = 0;
   for (uint32_t i = 1; i < num_dev_cfg_modules + 1; i++)
   {
      result = register_one_from_xml(hProps, i);

      if (ADSP_FAILED(result))
      {
         num_failed ++;
      }
   }
   uint64_t end_time=qurt_elite_timer_get_time_in_msec();
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Loading modules from dev cfg XML: Successfully loaded %lu modules. Total failures %lu. time taken %lu ms ",
         num_dev_cfg_modules - num_failed, num_failed, (uint32_t)(end_time-start_time));

   if (num_failed > 0)
   {
      return ADSP_EFAILED;
   }
#endif
   return ADSP_EOK;
}


/**
 * similar to pick_capi_revision: check doc there.
 */
static uint32_t pick_capi_v2_revision(uint32_t curr_ind, uint32_t *picked_ind)
{
   *picked_ind=curr_ind;
   if (curr_ind+1 < mm_num_dynamic_capi_v2_modules)
   {
      if ( (mm_dynamic_capi_v2_modules[curr_ind].id1 != mm_dynamic_capi_v2_modules[curr_ind+1].id1) ||
            (mm_dynamic_capi_v2_modules[curr_ind].id2 != mm_dynamic_capi_v2_modules[curr_ind+1].id2) ||
            (mm_dynamic_capi_v2_modules[curr_ind].mtype != mm_dynamic_capi_v2_modules[curr_ind+1].mtype))
      {
         return 0; //curr_ind+1-curr_ind-1
      }
   }
   else
   {
      return 0;
   }

   bool_t found=false;
   for (uint32_t i = curr_ind; i < mm_num_dynamic_capi_v2_modules; i++)
   {
      if (i != curr_ind)
      {
         if ( (mm_dynamic_capi_v2_modules[i-1].id1 != mm_dynamic_capi_v2_modules[i].id1) ||
               (mm_dynamic_capi_v2_modules[i-1].id2 != mm_dynamic_capi_v2_modules[i].id2) ||
               (mm_dynamic_capi_v2_modules[i-1].mtype != mm_dynamic_capi_v2_modules[i].mtype))
         {
            return i-curr_ind-1;
         }
      }
      if (!found)
      {
         if (check_file_exists(mm_dynamic_capi_v2_modules[i].filename))
         {
            *picked_ind = i;
            MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Revision picked for CAPI V2 m-id (%lu,0x%lx,0x%lx) is %lu",
                  mm_dynamic_capi_v2_modules[i].mtype, mm_dynamic_capi_v2_modules[i].id1, mm_dynamic_capi_v2_modules[i].id2,
                  mm_dynamic_capi_v2_modules[i].revision);
            found=true;
         }
      }
   }
   return mm_num_dynamic_capi_v2_modules-curr_ind-1;
}

static ADSPResult register_all_built_in_dynamic_modules()
{
   ADSPResult result = ADSP_EOK;

   uint64_t start_time=qurt_elite_timer_get_time_in_msec();

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "register_all_built_in_dynamic_modules adding dynamic modules to AMDB");
   // Even though module are separately listed in this file, there's no repetition from CAPI to CAPI V2 list.
   //add modules to the AMDB
   uint32_t num_capi_v2_added=0;
   uint32_t num_capi_v2_add_failed=0;

   uint32_t picked_ind=0;
   //Revision: always the latest revision is kept first by json parser.

   for (uint32_t i = 0; i < mm_num_dynamic_capi_v2_modules; i++)
   {
      i+=pick_capi_v2_revision(i, &picked_ind);

      result = adsp_amdb_add_capi_v2_with_entry_type( ADSP_AMDB_ENTRY_TYPE_INIT, mm_dynamic_capi_v2_modules[picked_ind].mtype,
            mm_dynamic_capi_v2_modules[picked_ind].id1, mm_dynamic_capi_v2_modules[picked_ind].id2, mm_dynamic_capi_v2_modules[picked_ind].preload,
            mm_dynamic_capi_v2_modules[picked_ind].filename,
            mm_dynamic_capi_v2_modules[picked_ind].tag);

      if(ADSP_FAILED(result))
      {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to add dynamic CAPI V2 module %lx, 0x%lx, 0x%lx. result %d. (ok when overridden)",
               mm_dynamic_capi_v2_modules[picked_ind].mtype, mm_dynamic_capi_v2_modules[picked_ind].id1, mm_dynamic_capi_v2_modules[picked_ind].id2, result);
         num_capi_v2_add_failed++;
      }
      else
      {
         num_capi_v2_added++;
      }
   }
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Registering built-in CAPI V2 dynamic modules to AMDB. Num added %lu, Num Failed %lu, Total %lu. (any rest are revisions)",
         num_capi_v2_added, num_capi_v2_add_failed, mm_num_dynamic_capi_v2_modules); //rest are skipped as they are revisions.
   uint64_t end_time=qurt_elite_timer_get_time_in_msec();
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "register_all_built_in_dynamic_modules done adding dynamic modules to AMDB. Time taken %lu ms",
         (uint32_t)(end_time-start_time));

   return result;
}

ADSPResult mm_register_all_static_modules_with_entry_type(uint32_t entry_type)
{
   ADSPResult result = ADSP_EOK;
   uint64_t start_time=qurt_elite_timer_get_time_in_msec();
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "mm_register_static_modules adding static modules to AMDB. entry-type %lu", entry_type);
   // Even though module are separately listed in this file, there's no repetition from CAPI to CAPi V2 list.
   //add modules to the data base
   uint32_t num_stub_added=0, num_capi_v2_added=0;
   uint32_t num_stub_add_failed=0, num_capi_v2_add_failed=0;

   for (uint32_t i = 0; i < mm_num_static_capi_v2_modules; i++)
   {
      result = adsp_amdb_add_static_capi_v2( entry_type, mm_static_capi_v2_modules[i].mtype, mm_static_capi_v2_modules[i].id1,
            mm_static_capi_v2_modules[i].id2,
            mm_static_capi_v2_modules[i].get_static_prop_fn,
            mm_static_capi_v2_modules[i].init_fn);

      if(ADSP_FAILED(result))
      {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to add static CAPI V2 module %lx, 0x%lx, 0x%lx. result %d. (ok when overridden)",
               mm_static_capi_v2_modules[i].mtype, mm_static_capi_v2_modules[i].id1, mm_static_capi_v2_modules[i].id2, result);
         num_capi_v2_add_failed++;
      }
      else
      {
         num_capi_v2_added++;
      }
   }
   for (uint32_t i = 0; i < mm_num_virtual_stub_modules; i++)
   {
      result = adsp_amdb_add_stub_with_entry_type(entry_type, mm_virtual_stub_modules[i].mtype, mm_virtual_stub_modules[i].id1, mm_virtual_stub_modules[i].id2);

      if(ADSP_FAILED(result))
      {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to add static virtual stub module %lx, 0x%lx, 0x%lx. result %d. (ok when overridden)",
               mm_virtual_stub_modules[i].mtype, mm_virtual_stub_modules[i].id1, mm_virtual_stub_modules[i].id2, result);
         num_stub_add_failed++;
      }
      else
      {
         num_stub_added++;
      }
   }

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Registering static modules to AMDB. Num added (CAPI V2, stub)=(%lu, %lu), Num Failed (%lu, %lu)).",
         num_capi_v2_added, num_stub_added, num_capi_v2_add_failed, num_stub_add_failed);
   uint64_t end_time=qurt_elite_timer_get_time_in_msec();
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "mm_register_static_modules done adding static modules to AMDB. Time taken %lu ms",(uint32_t)(end_time-start_time));

   return result;
}

/**
 * all the entries to got INIT entry.
 */
ADSPResult mm_register_all_built_in()
{
   ADSPResult result = ADSP_EOK;

   uint64_t start_time = qurt_elite_timer_get_time_in_msec();

   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "mm_register_all_built_in: dev cfg XML ");
   result = register_all_from_dev_cfg_xml();

   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "register_all_from_dev_cfg_xml failed %d", result);
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "register_all_from_dev_cfg_xml success");
   }


   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "mm_register_all_built_in: built-in dynamic");
   result = register_all_built_in_dynamic_modules();

   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "register_all_built_in_dynamic_modules failed %d", result);
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "register_all_built_in_dynamic_modules success");
   }


   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "mm_register_all_built_in: built-in static ");
   result = mm_register_all_static_modules_with_entry_type(ADSP_AMDB_ENTRY_TYPE_INIT);

   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "mm_register_static_modules failed %d", result);
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "mm_register_static_modules success");
   }

   uint64_t end_time = qurt_elite_timer_get_time_in_msec();
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "mm_register_all_built_in : time taken %lu ms", (uint32_t)(end_time-start_time));

   return result;
}

#include "mm_built_in_modules.h"

typedef struct mm_mod_built_in_info_t
{
   bool_t is_static; //dynamic otherwise
   uint32_t itype;
   union
   {
      struct //dynamic
      {
         bool_t preload;
         const char *filename; //for dynamic only
         const char *tag1; //althout built-in dynamic has only tag, dev cfg has both tags(=func names).
         const char *tag2;
      };
      struct
      {
         void *fn1; //func ptrs
         void *fn2;
      };
   };
} mm_mod_built_in_info_t;

/**
 * struct to help load needed prereq modules that are not registered already
 */
typedef struct mm_is_prereq_reg_t
{
   bool_t is_mod_needed; //true => corresponding mm_prereq_mids.id is needed module.
   bool_t is_prereq_registered[MM_MAX_PREREQUISITE_MIDS]; //true=> corresponding mm_prereq_mids.prereq_mids is in AMDB.
} mm_is_prereq_reg_t;


typedef struct mm_built_in_mod_tracker_t
{
   bool_t *mm_is_dev_cfg_modules_in_acdb;
   bool_t *mm_is_dynamic_capi_v2_modules_in_acdb;
   bool_t *mm_is_static_capi_v2_modules_in_acdb;
   bool_t *mm_is_virtual_stub_modules_in_acdb;
   mm_is_prereq_reg_t *mm_is_prereq_reg_per_acdb; //tracks: is prereq registered per ACDB & also is module requiring prereq, needed?
   bool_t *mm_is_mod_from_backward_compat_list_not_needed_per_acdb; //by default memset with 0, hence, needed.

} mm_built_in_mod_tracker_t;

bool_t is_private_module(uint32_t mtype, uint32_t id1, uint32_t id2)
{
   for (uint32_t i=0; i<mm_num_private_modules;i++)
   {
      if ( (mtype == mm_private_modules[i].mtype) &&
            (id1 == mm_private_modules[i].id1) &&
            (id2 == mm_private_modules[i].id2))
      {
         return true;
      }
   }
   return false;
}

static ADSPResult process_dev_cfg_modules_not_found_in_acdb(mm_built_in_mod_tracker_t *mm_tracker)
{
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Processing any dev_cfg_modules_not_found_in_acdb");

   if ((NULL == mm_tracker) || (NULL == mm_tracker->mm_is_dev_cfg_modules_in_acdb))
   {
      return ADSP_EOK;
   }
   //for built-in modules that didnt find a mention in ACDB file, we need to add a stub to INIT entry
#ifndef STAND_ALONE
   ADSPResult result=ADSP_EOK;

   DALSYSPropertyVar PropVar;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProps);

   CHECK_RET_DAL(DALSYS_GetDALPropertyHandleStr("dynamic_modules", hProps));
   CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, "NumModules", 0, &PropVar));
   VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);
   uint32_t num_dev_cfg_modules = PropVar.Val.dwVal;

   const uint32_t buffer_size = sizeof ("CAPIV2TYPE_") /* The longest token */
                                 + 3 /* 3 digits for the index */
                                 + 1; /* NULL character */
   char string[buffer_size];

   for (uint32_t index = 1; index < num_dev_cfg_modules + 1; index++)
   {
      //for modules which didn't get added
      if (!mm_tracker->mm_is_dev_cfg_modules_in_acdb[index-1])
      {
         DALSYSPropertyVar PropVar;

         snprintf(string, buffer_size, "TYPE_%lu", index);
         CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
         VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);
         const uint32_t type = PropVar.Val.dwVal;

         switch(type)
         {
         case DEV_CFG_TYPE_CAPI_V2:
         {
            snprintf(string, buffer_size, "CAPIV2TYPE_%lu", index);
            CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
            VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);
            const uint32_t mtype = PropVar.Val.dwVal;

            snprintf(string, buffer_size, "ID1_%lu", index);
            CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
            VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);
            const uint32_t _id1 = PropVar.Val.dwVal;

            snprintf(string, buffer_size, "ID2_%lu", index);
            CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
            VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);
            const uint32_t _id2 = PropVar.Val.dwVal;

            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Ignoring dev cfg built-in CAPI V2 module not found in ACDB. capiv2type %lu, ID1 0x%lx ID2 0x%lx.", mtype,_id1, _id2);

            //result |= adsp_amdb_add_stub_with_entry_type(ADSP_AMDB_ENTRY_TYPE_INIT, mtype, _id1, _id2);
            //mm_tracker->mm_is_dev_cfg_modules_in_acdb[index-1] = true;
         }
         break;
         default:
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed with invalid module type %lu.", type);
            result |= ADSP_EBADPARAM;
         }
      }
   }
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failure occured in registering stub. %lu",result);
   }
#endif //#ifndef STAND_ALONE
   return ADSP_EOK;

}

static void process_dynamic_modules_not_found_in_acdb(mm_built_in_mod_tracker_t *mm_tracker)
{
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Processing any dynamic_modules_not_found_in_acdb");
   ADSPResult result = ADSP_EOK;
   /** dynamic */
   //for built-in modules that didnt find a mention in ACDB file, we need to add a stub to INIT entry
   if (mm_tracker && mm_tracker->mm_is_dynamic_capi_v2_modules_in_acdb)
   {
      for (uint32_t i = 0; i < mm_num_dynamic_capi_v2_modules; i++)
      {
         if ( !mm_tracker->mm_is_dynamic_capi_v2_modules_in_acdb[i] )
         {
            MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Ignoring built-in dynamic CAPI V2 module not found in ACDB file. mtype %lu, ID1 0x%lx ID2 0x%lx. Revision %lu",
                  mm_dynamic_capi_v2_modules[i].mtype, mm_dynamic_capi_v2_modules[i].id1,
                  mm_dynamic_capi_v2_modules[i].id2, mm_dynamic_capi_v2_modules[i].revision);

            //result |= adsp_amdb_add_stub_with_entry_type(ADSP_AMDB_ENTRY_TYPE_INIT, mm_dynamic_capi_v2_modules[i].mtype, mm_dynamic_capi_v2_modules[i].id1, mm_dynamic_capi_v2_modules[i].id2);
            //mm_tracker->mm_is_dynamic_capi_v2_modules_in_acdb[i] = true;
         }
      }
   }

   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failure occured in registering stub. %d",result);
   }
}

static void process_static_modules_not_found_in_acdb(mm_built_in_mod_tracker_t *mm_tracker)
{
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Processing any static_modules_not_found_in_acdb");

   ADSPResult result = ADSP_EOK;
   /** static */
   //for built-in modules that didnt find a mention in ACDB file, we need to add a stub to INIT entry
   //and remove static entry (except for private)
   if (mm_tracker && mm_tracker->mm_is_static_capi_v2_modules_in_acdb)
   {
      for (uint32_t i = 0; i < mm_num_static_capi_v2_modules; i++)
      {
         if ( !mm_tracker->mm_is_static_capi_v2_modules_in_acdb[i] )
         {
            //move private modules to init entry.
            if (is_private_module(mm_static_capi_v2_modules[i].mtype, mm_static_capi_v2_modules[i].id1, mm_static_capi_v2_modules[i].id2))
            {
               MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Registering built-in private static CAPI V2 module to AMDB. mtype %lu, ID1 0x%lx ID2 0x%lx.",
                     mm_static_capi_v2_modules[i].mtype, mm_static_capi_v2_modules[i].id1, mm_static_capi_v2_modules[i].id2);

               result |= adsp_amdb_add_static_capi_v2(ADSP_AMDB_ENTRY_TYPE_INIT,
                     mm_static_capi_v2_modules[i].mtype, mm_static_capi_v2_modules[i].id1, mm_static_capi_v2_modules[i].id2,
                     mm_static_capi_v2_modules[i].get_static_prop_fn,  mm_static_capi_v2_modules[i].init_fn);
            }
            else
            {
               MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Ignoring built-in static CAPI V2 module not found in ACDB file. mtype %lu, ID1 0x%lx ID2 0x%lx.",
                     mm_static_capi_v2_modules[i].mtype, mm_static_capi_v2_modules[i].id1, mm_static_capi_v2_modules[i].id2);
               //result |= adsp_amdb_add_stub_with_entry_type(ADSP_AMDB_ENTRY_TYPE_INIT, mm_static_capi_v2_modules[i].mtype, mm_static_capi_v2_modules[i].id1, mm_static_capi_v2_modules[i].id2);
            }

            mm_tracker->mm_is_static_capi_v2_modules_in_acdb[i] = true;

         }
      }
   }


   if (mm_tracker && mm_tracker->mm_is_virtual_stub_modules_in_acdb)
   {
      for (uint32_t i = 0; i < mm_num_virtual_stub_modules; i++)
      {
         if ( !mm_tracker->mm_is_virtual_stub_modules_in_acdb[i] )
         {
            //move private modules to init entry.
            if (is_private_module(mm_virtual_stub_modules[i].mtype, mm_virtual_stub_modules[i].id1, mm_virtual_stub_modules[i].id2))
            {
               MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Registering built-in private static stub module to AMDB. mtype %lu, ID1 0x%lx ID2 0x%lx.",
                     mm_virtual_stub_modules[i].mtype, mm_virtual_stub_modules[i].id1, mm_virtual_stub_modules[i].id2);

               result |= adsp_amdb_add_stub_with_entry_type(ADSP_AMDB_ENTRY_TYPE_INIT, mm_virtual_stub_modules[i].mtype, mm_virtual_stub_modules[i].id1, mm_virtual_stub_modules[i].id2);
            }
            else
            {
               MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Ignoring built-in static stub module not found in ACDB file. mtype %lu, ID1 0x%lx ID2 0x%lx.",
                     mm_virtual_stub_modules[i].mtype, mm_virtual_stub_modules[i].id1, mm_virtual_stub_modules[i].id2);
               //result |= adsp_amdb_add_stub_with_entry_type(ADSP_AMDB_ENTRY_TYPE_INIT, mm_virtual_stub_modules[i].mtype, mm_virtual_stub_modules[i].id1, mm_virtual_stub_modules[i].id2);
            }

            mm_tracker->mm_is_virtual_stub_modules_in_acdb[i] = true;

         }
      }
   }

   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failure occured in registering private static or in adding stub. %d",result);
   }
}

static void process_backward_compat_list(uint32_t entry_type, mm_built_in_mod_tracker_t *mm_tracker)
{
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Processing backward compatibility list modules");

   bool_t error_case=false;
   if ( (mm_tracker == NULL) || (mm_tracker->mm_is_mod_from_backward_compat_list_not_needed_per_acdb == NULL))
   {
      error_case = true;
   }

   for (uint32_t i = 0; i < MM_NUM_MOD_IN_BACKWARD_COMPAT_LIST; i++)
   {
      //if error case try to add all backward compat modules. if not, then add only if not already seen in ACDB.
      if (error_case || !mm_tracker->mm_is_mod_from_backward_compat_list_not_needed_per_acdb[i])
      {
         //if needed as per mm_is_mod_from_backward_compat_list_not_needed_per_acdb
         built_in_mod_info_t mod_info;
         mod_info.mtype = mm_backward_compatibility_list[i].mtype;
         mod_info.id1 = mm_backward_compatibility_list[i].id1;
         mod_info.id2 = 0;
         mod_info.preload = AMDB_CFG_PRELOAD_FLAG_USE_FROM_BUILD;
         mod_info.needed = true;

         ADSPResult result = mm_register_built_in_module(entry_type, &mod_info, mm_tracker);
         if (ADSP_FAILED(result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error while processing backward compat list %d. (OK for modules not in built-in list)", result);
         }
      }
   }
}

static void process_prereq_modules(uint32_t entry_type, mm_built_in_mod_tracker_t *mm_tracker)
{
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Processing any pre-req modules ");
   //dependent modules which are needed per ACDB but not added to amdb are added now.
   if ( (mm_tracker == NULL) || (mm_tracker->mm_is_prereq_reg_per_acdb == NULL) ) return;

   for (uint32_t i = 0; i < mm_num_prereq_mids; i++)
   {
      if (mm_tracker->mm_is_prereq_reg_per_acdb[i].is_mod_needed)
      {
         for (uint32_t j = 0; j < MM_MAX_PREREQUISITE_MIDS; j++)
         {
            if (mm_prereq_mids[i].prereq_mids[j].id1 != 0)
            {
               if (!mm_tracker->mm_is_prereq_reg_per_acdb[i].is_prereq_registered[j])
               {
                  //now register
                  MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Prerequisite module-id (%lu, 0x%lx, 0x%lx) needed because of (%lu, 0x%lx, 0x%lx). being registered to AMDB now ",
                        mm_prereq_mids[i].prereq_mids[j].mtype, mm_prereq_mids[i].prereq_mids[j].id1, mm_prereq_mids[i].prereq_mids[j].id2,
                        mm_prereq_mids[i].id.mtype, mm_prereq_mids[i].id.id1, mm_prereq_mids[i].id.id2);


                  built_in_mod_info_t mod_info;
                  mod_info.mtype = mm_prereq_mids[i].prereq_mids[j].mtype;
                  mod_info.id1 = mm_prereq_mids[i].prereq_mids[j].id1;
                  mod_info.id2 = mm_prereq_mids[i].prereq_mids[j].id2;
                  mod_info.preload = AMDB_CFG_PRELOAD_FLAG_USE_FROM_BUILD;
                  mod_info.needed = true;

                  ADSPResult result = mm_register_built_in_module(entry_type, &mod_info, mm_tracker);
                  if (ADSP_FAILED(result))
                  {
                     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error while processing prerequisite modules %d", result);
                  }
               }
            }
         }
      }
   }
}

void mm_remove_all_static_entries()
{
   ADSPResult result = ADSP_EOK;
   //Remove all static entries
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Removing all static entries from AMDB");
   for (uint32_t i = 0; i < mm_num_static_capi_v2_modules; i++)
   {
      result |= adsp_amdb_remove_capi_v2_with_entry_type(ADSP_AMDB_ENTRY_TYPE_STATIC, mm_static_capi_v2_modules[i].mtype, mm_static_capi_v2_modules[i].id1, mm_static_capi_v2_modules[i].id2);
   }

   for (uint32_t i = 0; i < mm_num_virtual_stub_modules; i++)
   {
      result |= adsp_amdb_remove_stub_with_entry_type(ADSP_AMDB_ENTRY_TYPE_STATIC, mm_virtual_stub_modules[i].mtype, mm_virtual_stub_modules[i].id1, mm_virtual_stub_modules[i].id2);
   }

   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failure occured in removing private static built-in %d",result);
   }
}

//called only for ACDB file processing
//this is useful for tracking which in-built modules dont get registered.
//returns a ptr, which should be passed back in finalize.
void * mm_built_in_registration_tracker_init()
{
   uint32_t num_dev_cfg_modules = 0;
   mm_built_in_mod_tracker_t *mm_tracker = NULL;

#ifndef STAND_ALONE
   DALSYSPropertyVar PropVar;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProps);

   DALResult res = DALSYS_GetDALPropertyHandleStr("dynamic_modules", hProps);
   if (DAL_SUCCESS == res)
   {
      res = DALSYS_GetPropertyValue(hProps, "NumModules", 0, &PropVar);
      if (PropVar.dwType == DALSYS_PROP_TYPE_UINT32)
      {
         num_dev_cfg_modules = PropVar.Val.dwVal;
      }
   }
#endif //STAND_ALONE

   uint32_t total=(
         num_dev_cfg_modules + mm_num_dynamic_capi_v2_modules +
         mm_num_static_capi_v2_modules + mm_num_virtual_stub_modules +
         MM_NUM_MOD_IN_BACKWARD_COMPAT_LIST
   )*sizeof(bool_t) +
         (mm_num_prereq_mids*sizeof(mm_is_prereq_reg_t)) +
         sizeof(mm_built_in_mod_tracker_t);

   uint8_t *ptr =  (uint8_t*)qurt_elite_memory_malloc(total, QURT_ELITE_HEAP_DEFAULT);
   if (ptr==NULL)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory");
      mm_tracker = NULL;
      return mm_tracker;
   }
   memset(ptr, 0, total); //initialize with false.

   mm_tracker  = (mm_built_in_mod_tracker_t*) ptr;
   ptr += sizeof(mm_built_in_mod_tracker_t);

   if (MM_NUM_MOD_IN_BACKWARD_COMPAT_LIST)   mm_tracker->mm_is_mod_from_backward_compat_list_not_needed_per_acdb = (uint8_t*)ptr;
   else                                      mm_tracker->mm_is_mod_from_backward_compat_list_not_needed_per_acdb = NULL;
   ptr+=(MM_NUM_MOD_IN_BACKWARD_COMPAT_LIST*sizeof(bool_t));

   if (num_dev_cfg_modules)               mm_tracker->mm_is_dev_cfg_modules_in_acdb = (uint8_t*)ptr;
   else                                   mm_tracker->mm_is_dev_cfg_modules_in_acdb = NULL;
   ptr+=(num_dev_cfg_modules*sizeof(bool_t));

   if (mm_num_dynamic_capi_v2_modules)    mm_tracker->mm_is_dynamic_capi_v2_modules_in_acdb = (uint8_t*)ptr;
   else                                   mm_tracker->mm_is_dynamic_capi_v2_modules_in_acdb = NULL;
   ptr+=(mm_num_dynamic_capi_v2_modules*sizeof(bool_t));

   if (mm_num_static_capi_v2_modules)     mm_tracker->mm_is_static_capi_v2_modules_in_acdb = (uint8_t*)ptr;
   else                                   mm_tracker->mm_is_static_capi_v2_modules_in_acdb = NULL;
   ptr+=(mm_num_static_capi_v2_modules*sizeof(bool_t));

   if (mm_num_virtual_stub_modules)        mm_tracker->mm_is_virtual_stub_modules_in_acdb = (uint8_t*)ptr;
   else                                    mm_tracker->mm_is_virtual_stub_modules_in_acdb = NULL;
   ptr+=(mm_num_virtual_stub_modules*sizeof(bool_t));

   if (mm_num_prereq_mids)                mm_tracker->mm_is_prereq_reg_per_acdb = (mm_is_prereq_reg_t*)ptr;
   else                                   mm_tracker->mm_is_prereq_reg_per_acdb = NULL;
   ptr+=(mm_num_prereq_mids*sizeof(mm_is_prereq_reg_t));

   return mm_tracker;
}

//called only for ACDB file processing
void mm_built_in_registration_tracker_finalize(void *tracker, bool_t built_in_sec_found)
{
   mm_built_in_mod_tracker_t *mm_tracker = (mm_built_in_mod_tracker_t*)tracker;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Finalizing the registration. built_in_sec_found %u", built_in_sec_found);

   if (built_in_sec_found)
   {
      //register any module in the backward compatibility list unless they are explicitly marked as not needed in ACDB file.
      //this is to be done only if built_in_sec_found as otherwise, all backward compatibility modules are needed by default & they'll be added if they are in built-in lists.
      //a module in backward compatibility list can be added to AMDB only if it's mentioned in the built-in lists.
      //do this before the processing of prereq modules, dev-cfg, dynamic, static modules because this processing could affect other lists.
      process_backward_compat_list(ADSP_AMDB_ENTRY_TYPE_INIT, mm_tracker);

      // process prereq modules: register them to AMDB if they are not already added.
      process_prereq_modules(ADSP_AMDB_ENTRY_TYPE_INIT, mm_tracker);

      //process(print) all built-in modules that didn't get loaded to AMDB.
      //built-in modules that didnt find a mention in ACDB file (& in backward compatibility list), we just ignore.
      //earlier the idea was to add as stub, but then if a topo is created with those modules, a stub would be silent error but
      //failing is better as it immediately notifies.
      //finally all static entries are also removed. this way, modules not found in ACDB file never get used/consume any memory.

      //if modules mentioned in dev-cfg/dynamic are overridden on top of modules mentioned in dynamic/static, then
      // AMDB will fail (but failure is ignored).
      process_dev_cfg_modules_not_found_in_acdb(mm_tracker);
      process_dynamic_modules_not_found_in_acdb(mm_tracker);
      process_static_modules_not_found_in_acdb(mm_tracker);

   }

   if (NULL != mm_tracker)
   {
      qurt_elite_memory_free(mm_tracker);
      mm_tracker = NULL;
   }

}

static void mark_backward_compat_list(mm_built_in_mod_tracker_t *mm_tracker, uint32_t mtype, uint32_t id1, uint32_t id2, bool_t needed)
{
   if ( (mm_tracker == NULL) || (mm_tracker->mm_is_mod_from_backward_compat_list_not_needed_per_acdb == NULL)) return;

   if (needed)
   {
      return; //since default value = needed.
   }

   //update if ACDB file explicitly marks the module as not-needed
   for (uint32_t i = 0; i < MM_NUM_MOD_IN_BACKWARD_COMPAT_LIST; i++)
   {
      if ( (mtype == mm_backward_compatibility_list[i].mtype) &&
            (id1 == mm_backward_compatibility_list[i].id1) &&
            (id2 == 0))
      {
         mm_tracker->mm_is_mod_from_backward_compat_list_not_needed_per_acdb[i] = true; //not needed
         return;
      }
   }
}

void add_prereq_module_to_client_entry(uint32_t mtype, uint32_t id1, uint32_t id2, bool_t needed)
{
   if (!needed) return;

   for (uint32_t i = 0; i < mm_num_prereq_mids; i++)
   {
      if ( (mtype == mm_prereq_mids[i].id.mtype) &&
            (id1 == mm_prereq_mids[i].id.id1) &&
            (id2 == mm_prereq_mids[i].id.id2)) //if the module has a pre-req mod
      {
         for (uint32_t j = 0; j < MM_MAX_PREREQUISITE_MIDS; j++)
         {
            if (mm_prereq_mids[i].prereq_mids[j].id1 != 0)
            {
               //now register
               MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Prerequisite module-id (%lu, 0x%lx, 0x%lx) needed because of (%lu, 0x%lx, 0x%lx). being registered to AMDB now ",
                     mm_prereq_mids[i].prereq_mids[j].mtype, mm_prereq_mids[i].prereq_mids[j].id1, mm_prereq_mids[i].prereq_mids[j].id2,
                     mm_prereq_mids[i].id.mtype, mm_prereq_mids[i].id.id1, mm_prereq_mids[i].id.id2);

               built_in_mod_info_t mod_info;
               mod_info.mtype = mm_prereq_mids[i].prereq_mids[j].mtype;
               mod_info.id1 = mm_prereq_mids[i].prereq_mids[j].id1;
               mod_info.id2 = mm_prereq_mids[i].prereq_mids[j].id2;
               mod_info.preload = AMDB_CFG_PRELOAD_FLAG_USE_FROM_BUILD;
               mod_info.needed = true;

               ADSPResult result = mm_register_built_in_module(ADSP_AMDB_ENTRY_TYPE_CLIENT, &mod_info, NULL);
               if (ADSP_FAILED(result))
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error while processing prerequisite modules %d (OK if adding again)", result);
               }
            }
         }
      }
   }
}

static void mark_prereq_modules(mm_built_in_mod_tracker_t *mm_tracker, uint32_t mtype, uint32_t id1, uint32_t id2, bool_t needed)
{
   if ((mm_tracker == NULL) || (mm_tracker->mm_is_prereq_reg_per_acdb == NULL))
   {
      return;
   }
   if (!needed)
   {
      return; //since default value = not needed.
   }

   //check if input module matches either the  mm_prereq_mids_t.id or one of the dependent modules.
   //since we cannot predict the order.
   for (uint32_t i = 0; i < mm_num_prereq_mids; i++)
   {
      if ( (mtype == mm_prereq_mids[i].id.mtype) &&
            (id1 == mm_prereq_mids[i].id.id1) &&
            (id2 == mm_prereq_mids[i].id.id2))
      {
         mm_tracker->mm_is_prereq_reg_per_acdb[i].is_mod_needed = true; //eg. ds1
      }

      for (uint32_t j = 0; j < MM_MAX_PREREQUISITE_MIDS; j++)
      {
         if (mm_prereq_mids[i].prereq_mids[j].id1 != 0)
         {
            if ( (mtype == mm_prereq_mids[i].prereq_mids[j].mtype) &&
                  (id1 == mm_prereq_mids[i].prereq_mids[j].id1) &&
                  (id2 == mm_prereq_mids[i].prereq_mids[j].id2)) //eg. resampler, chmixer,
            {
               //a prereq module could be added from MREGINFO (overriding).
               //that's not tracked here. AMDB would return errors for overridden modules anyway.
               mm_tracker->mm_is_prereq_reg_per_acdb[i].is_prereq_registered[j] = true;
               break;
            }
         }
      }
   }
}

static ADSPResult look_up_module_in_dev_cfg(uint32_t mtype, uint32_t id1, uint32_t id2, mm_mod_built_in_info_t *binfo, mm_built_in_mod_tracker_t *mm_tracker)
{
   ADSPResult result = ADSP_EFAILED; //look-up failed. didn't find.

#ifndef STAND_ALONE
   DALSYSPropertyVar PropVar;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProps);

   CHECK_RET_DAL(DALSYS_GetDALPropertyHandleStr("dynamic_modules", hProps));
   CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, "NumModules", 0, &PropVar));
   VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);
   uint32_t num_dev_cfg_modules = PropVar.Val.dwVal;

   const uint32_t buffer_size = sizeof ("CAPIV2TYPE_") /* The longest token */
                                 + 3 /* 3 digits for the index */
                                 + 1; /* NULL character */
   char string[buffer_size];

   for (uint32_t index = 1; index < num_dev_cfg_modules + 1; index++)
   {
      DALSYSPropertyVar PropVar;

      snprintf(string, buffer_size, "TYPE_%lu", index);
      CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
      VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);
      const uint32_t type = PropVar.Val.dwVal;

      bool_t found=false;
      switch(type)
      {
      case DEV_CFG_TYPE_CAPI_V2:
      {
         snprintf(string, buffer_size, "CAPIV2TYPE_%lu", index);
         CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
         VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);
         const uint32_t type = PropVar.Val.dwVal;

         snprintf(string, buffer_size, "ID1_%lu", index);
         CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
         VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);
         const uint32_t _id1 = PropVar.Val.dwVal;

         snprintf(string, buffer_size, "ID2_%lu", index);
         CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
         VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);
         const uint32_t _id2 = PropVar.Val.dwVal;

         if ( (type==mtype) && (id1==_id1) && (id2==_id2) )
         {
            snprintf(string, buffer_size, "TAG_%lu", index);
            CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
            VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_STR_PTR);
            const char *tag = PropVar.Val.pszVal;

            found = true;
            binfo->tag1 = tag;
            binfo->is_static = false;
            binfo->itype = AMDB_INTERFACE_TYPE_CAPI_V2;

#ifdef MEDIUM_LOG
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Found module mtype %lu, ID1 0x%lx ID2 0x%lx as CAPI V2 in dev cfg.", mtype,id1, id2);
#endif
         }
      }
      break;
      default:
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed with invalid module type %lu.", type);
         result = ADSP_EBADPARAM;
      }

      if (found)
      {
         snprintf(string, buffer_size, "URI_%lu", index);
         CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
         VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_STR_PTR);
         const char *uri = PropVar.Val.pszVal;

         snprintf(string, buffer_size, "PRELOAD_%lu", index);
         CHECK_RET_DAL(DALSYS_GetPropertyValue(hProps, string, 0, &PropVar));
         VERIFY_RET(PropVar.dwType == DALSYS_PROP_TYPE_UINT32);
         const bool_t preload = (PropVar.Val.dwVal == 0) ? FALSE : TRUE;

         binfo->filename = uri;
         binfo->preload = preload;

         //put a tick mark against module. so that remaining can be errored out.
         if (mm_tracker && mm_tracker->mm_is_dev_cfg_modules_in_acdb) mm_tracker->mm_is_dev_cfg_modules_in_acdb[index-1] = true;

         return ADSP_EOK;
      }
   }
#endif //#ifndef STAND_ALONE
   return result;
}


static ADSPResult look_up_module_in_dynamic_mod_lists(uint32_t mtype, uint32_t id1, uint32_t id2, mm_mod_built_in_info_t *binfo, mm_built_in_mod_tracker_t *mm_tracker)
{
   //ADSPResult result = ADSP_EFAILED; //look-up failed. didn't find.

   uint32_t picked_ind=0;

   for (uint32_t i = 0; i < mm_num_dynamic_capi_v2_modules; i++)
   {
      if ( (mtype == mm_dynamic_capi_v2_modules[i].mtype) &&
            (id1 == mm_dynamic_capi_v2_modules[i].id1) &&
            (id2 == mm_dynamic_capi_v2_modules[i].id2))
      {
         uint32_t temp = i;
         i+=pick_capi_v2_revision(i, &picked_ind);

         binfo->is_static = false;
         binfo->preload = mm_dynamic_capi_v2_modules[picked_ind].preload;
         binfo->filename = mm_dynamic_capi_v2_modules[picked_ind].filename;
         binfo->tag1 = mm_dynamic_capi_v2_modules[picked_ind].tag;
         binfo->itype = AMDB_INTERFACE_TYPE_CAPI_V2;

         //put a tick mark against module. so that remaining can be errored out.
         if (mm_tracker && mm_tracker->mm_is_dynamic_capi_v2_modules_in_acdb)
         {
            for (uint32_t j=temp;j<=i;j++)
            {
               mm_tracker->mm_is_dynamic_capi_v2_modules_in_acdb[j] = true;
            }
         }

#ifdef MEDIUM_LOG
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Found module mtype %lu, ID1 0x%lx ID2 0x%lx as CAPI V2 in dynamic mod list. Revision %lu",
               mm_dynamic_capi_v2_modules[picked_ind].mtype, mm_dynamic_capi_v2_modules[picked_ind].id1, mm_dynamic_capi_v2_modules[picked_ind].id2,
               mm_dynamic_capi_v2_modules[picked_ind].revision);
#endif
         return ADSP_EOK;
      }
   }

   return ADSP_EFAILED;
}


static ADSPResult look_up_module_in_static_mod_lists(uint32_t mtype, uint32_t id1, uint32_t id2, mm_mod_built_in_info_t *binfo, mm_built_in_mod_tracker_t *mm_tracker)
{
   //ADSPResult result = ADSP_EFAILED;//look-up failed. didn't find.

   for (uint32_t i = 0; i < mm_num_static_capi_v2_modules; i++)
   {
      if ( (mtype == mm_static_capi_v2_modules[i].mtype) &&
            (id1 == mm_static_capi_v2_modules[i].id1) &&
            (id2 == mm_static_capi_v2_modules[i].id2))
      {
         binfo->is_static = true;
         binfo->fn1 = (void*) mm_static_capi_v2_modules[i].get_static_prop_fn;
         binfo->fn2 = (void*) mm_static_capi_v2_modules[i].init_fn;
         binfo->itype = AMDB_INTERFACE_TYPE_CAPI_V2;

         //put a tick mark against module. so that remaining can be errored out.
         if (mm_tracker && mm_tracker->mm_is_static_capi_v2_modules_in_acdb) mm_tracker->mm_is_static_capi_v2_modules_in_acdb[i] = true;

#ifdef MEDIUM_LOG
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Found module mtype %lu, ID1 0x%lx ID2 0x%lx as CAPI_V2 in static module list..",
               mm_static_capi_v2_modules[i].mtype, mm_static_capi_v2_modules[i].id1, mm_static_capi_v2_modules[i].id2);
#endif
         return ADSP_EOK;
      }
   }

   for (uint32_t i = 0; i < mm_num_virtual_stub_modules; i++)
   {
      if ( (mtype == mm_virtual_stub_modules[i].mtype) &&
            (id1 == mm_virtual_stub_modules[i].id1) &&
            (id2 == mm_virtual_stub_modules[i].id2))
      {
         binfo->is_static = true;
         binfo->fn1 = NULL;
         binfo->fn2 = NULL;
         binfo->itype = AMDB_INTERFACE_TYPE_STUB;

         //put a tick mark against module. so that remaining can be errored out.
         if (mm_tracker && mm_tracker->mm_is_virtual_stub_modules_in_acdb) mm_tracker->mm_is_virtual_stub_modules_in_acdb[i] = true;

#ifdef MEDIUM_LOG
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Found module mtype %lu, ID1 0x%lx ID2 0x%lx as stub in virtual-stub module list..",
               mm_virtual_stub_modules[i].mtype, mm_virtual_stub_modules[i].id1, mm_virtual_stub_modules[i].id2);
#endif
         return ADSP_EOK;
      }
   }

   return ADSP_EFAILED;
}

static ADSPResult mm_query_built_in_info_per_module(mm_built_in_mod_tracker_t *mm_tracker, uint32_t mtype, uint32_t id1, uint32_t id2, mm_mod_built_in_info_t *binfo)
{
   ADSPResult result = ADSP_EFAILED; //by default module is not found

   //order of lookup is important. precedence.

   //first look into dev cfg xml
   result = look_up_module_in_dev_cfg(mtype, id1, id2, binfo, mm_tracker);
   if (ADSP_SUCCEEDED(result))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Module (%lu, 0x%lx, 0x%lx) found in dev cfg", mtype, id1, id2);
      return result;
   }
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Module (%lu, 0x%lx, 0x%lx) not found in dev cfg. Looking into list of dynamic modules", mtype, id1, id2);

   //look into dynamic modules
   result = look_up_module_in_dynamic_mod_lists(mtype, id1, id2, binfo, mm_tracker);
   if (ADSP_SUCCEEDED(result))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Module (%lu, 0x%lx, 0x%lx) found in dynamic mod list", mtype, id1, id2);
      return result;
   }
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Module (%lu, 0x%lx, 0x%lx) not found in dynamic mod list. Looking into list of static modules", mtype, id1, id2);

   //look into static modules list
   result = look_up_module_in_static_mod_lists(mtype, id1, id2, binfo, mm_tracker);
   if (ADSP_SUCCEEDED(result))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Module (%lu, 0x%lx, 0x%lx) found in static mod list", mtype, id1, id2);
      return result;
   }
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Module (%lu, 0x%lx, 0x%lx) not found in any list", mtype, id1, id2);

   return result;
}

ADSPResult mm_register_built_in_module(uint32_t entry_type, built_in_mod_info_t *mod_info, void* tracker)
{
   ADSPResult result = ADSP_EOK;
   mm_built_in_mod_tracker_t *mm_tracker = (mm_built_in_mod_tracker_t*)tracker;
   mm_mod_built_in_info_t binfo;
   memset(&binfo,0,sizeof(binfo));
   //need to call this outside mod_info->needed condition because, even if its not needed, we need to know whether ACDB knows about module.
   //module not known to ACDB are not added to AMDB unlike not-needed modules which are added as stubs.
   result = mm_query_built_in_info_per_module(mm_tracker, mod_info->mtype, mod_info->id1, mod_info->id2, &binfo);
   if (ADSP_FAILED(result))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Built-in information not found for module-id (%lu, 0x%lx, 0x%lx). Ignoring this module.", mod_info->mtype, mod_info->id1, mod_info->id2);
      return result;
   }

   //since module is now found in built-in lists, mark the pre-req modules as needed if the module is needed.
   mark_prereq_modules(mm_tracker, mod_info->mtype, mod_info->id1, mod_info->id2, mod_info->needed);

   //since module is now found in built-in lists, if ACDB file explicitly marks the module as not-needed, then need to update tracker.
   mark_backward_compat_list(mm_tracker, mod_info->mtype, mod_info->id1, mod_info->id2, mod_info->needed);

   if (mod_info->preload == AMDB_CFG_PRELOAD_FLAG_USE_FROM_BUILD)
   {
      mod_info->preload = binfo.preload;
   }

   if (mod_info->needed)
   {
      switch(binfo.itype)
      {
      case AMDB_INTERFACE_TYPE_CAPI_V2:
      {
         if (binfo.is_static)
         {
            result = adsp_amdb_add_static_capi_v2(entry_type,
                  mod_info->mtype, mod_info->id1, mod_info->id2,
                  (capi_v2_get_static_properties_f)binfo.fn1, (capi_v2_init_f)binfo.fn2);
         }
         else
         {
            result = adsp_amdb_add_capi_v2_with_entry_type(entry_type,
                  mod_info->mtype, mod_info->id1, mod_info->id2,
                  mod_info->preload, (const char*)binfo.filename,
                  (const char*)binfo.tag1);
         }
         break;
      }
      case AMDB_INTERFACE_TYPE_STUB:
      {
         result = adsp_amdb_add_stub_with_entry_type(entry_type, mod_info->mtype, mod_info->id1, mod_info->id2);
      }
      default:
      {
         result = ADSP_EUNSUPPORTED;
         break;
      }
      }
   }
   else
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO," Module is marked as not needed module-id (%lu, 0x%lx, 0x%lx). Adding as stub. ",
            mod_info->mtype, mod_info->id1, mod_info->id2);
      result = adsp_amdb_add_stub_with_entry_type(entry_type, mod_info->mtype, mod_info->id1, mod_info->id2);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"adsp_amdb_add_stub_with_entry_type returned %d", result);
   }

   return result;
}

