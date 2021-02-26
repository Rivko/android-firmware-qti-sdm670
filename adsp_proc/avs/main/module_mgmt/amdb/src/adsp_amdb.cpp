/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "Elite_CAPI_V2.h"

#ifndef ADSP_AMDB_TEST
#include "stringl.h"
#include "adsp_amdb_static.h"
#include "adsp_amdb_internal.h"
#include "shared_lib_api.h"
#include "qurt_elite_memory.h"
#include "qurt_elite_atomic.h"
#include "qurt_elite_mutex.h"
#include "qurt_elite_condvar.h"
#include "qurt_elite_nmutex.h"
#else
#include "test_main.h"
#endif

#include "dlfcn.h"
#include "hashtable.h"

#include <stdlib.h>
#include <string.h>

#include "qurt_elite_diag.h"
#include "mmdefs.h"

#include "adsp_amdb_parallel_loader.h"

/*** Utility Functions ***************************************************/
#define STD_OFFSETOF(type,member)     (((char*)(&((type*)1)->member))-((char*)1))

#define STD_RECOVER_REC(type,member,p) ((void)((p)-&(((type*)1)->member)),\
                                        (type*)(void*)(((char*)(void*)(p))-STD_OFFSETOF(type,member)))

#define ALLOC_AND_COPY_STRING(dest_str, src_str, src_str_len) \
{  \
    if(src_str_len >0) \
    {        \
       dest_str = (char*)qurt_elite_memory_malloc((src_str_len + 1), QURT_ELITE_HEAP_DEFAULT);\
       if(dest_str !=NULL) \
       { \
          /* We use memscpy instead of strlcpy since we don't need to check for '\0' every iteration. */ \
          memscpy(dest_str, src_str_len+1, src_str, src_str_len+1);\
       } \
       else \
       { \
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"failed to allocate memory for string tag"); \
       } \
    } \
    else \
    { \
       dest_str = NULL; \
    } \
}    \

static inline uint32_t align_to_8_byte(const uint32_t num)
{
   return ((num + 7) & (0xFFFFFFF8));
}

#define TRY(exception, func) \
   if (ADSP_EOK != (exception = func)) {\
      goto exception##bail; \
   }

#define THROW(exception, errno) \
   exception = errno; \
   goto exception##bail;

#define CATCH(exception) exception##bail: if (exception != ADSP_EOK)

/** ideally dlfcn.h must define these macros. but in some targets they are not defined. this is a work-around.*/
#ifndef RTLD_DI_LOAD_ADDR
#define RTLD_DI_LOAD_ADDR 3
#endif

#ifndef RTLD_DI_LOAD_SIZE
#define RTLD_DI_LOAD_SIZE 4
#endif

#ifndef ADSP_AMDB_TEST
static ADSPResult make_version_checks(const char* filename_str,
      uint16_t lib_api_maj_ver, uint32_t lib_hex_tool_ver,
      bool_t lib_is_qurt_used, uint16_t lib_qurt_ver )
{
   if ( lib_api_maj_ver != adsp_avs_build_property.api_major_version)
   {
      MSG_SPRINTF_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                             "So file %s has incompatible API version",filename_str);
      return ADSP_EFAILED;
   }

   //compare only xx and yy in xx.yy.zz versioning.
   if ((lib_hex_tool_ver>>8) != (adsp_avs_build_property.hexagon_tools_version>>8))
   {
      MSG_SPRINTF_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                             "So file %s has incompatible Hexagon tools version. Still continuing...",filename_str);
   }

   if (lib_is_qurt_used &&
         (lib_qurt_ver != adsp_avs_build_property.qurt_api_version))
   {
      MSG_SPRINTF_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                             "So file %s has incompatible Qurt API version",filename_str);
      return ADSP_EFAILED;
   }

   return ADSP_EOK;
}
#endif

static ADSPResult check_version_mismatch(void *handle, const char* filename_str)
{
   ADSPResult result = ADSP_EOK;

#ifndef ADSP_AMDB_TEST
   shared_lib_build_property_t *lib_bld_prop = (shared_lib_build_property_t*)dlsym(handle, SHARED_LIB_API__SHARED_LIB_BUILD_PROPERTY_STRING);
   if (!lib_bld_prop) {
     MSG_SPRINTF_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                 "Ignoring version mismatch checks for %s as lib doesnot contain symbol %s, err=%s",
                 filename_str, SHARED_LIB_API__SHARED_LIB_BUILD_PROPERTY_STRING, dlerror());
     return ADSP_EOK;
   }

   switch (lib_bld_prop->build_property_version)
   {
   case SHARED_LIB_API__SHARED_LIB_BUILD_PROPERTY_VERSION:
   {
      MSG_SPRINTF_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                       "Build-id of .so file %s is %s",
                       filename_str, lib_bld_prop->build_id);

      MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                             "SO: Lib version %u.%u, API version %u.%u, Hexagon tools 0x%x, is Qurt used %u, Qurt API version %u",
                             lib_bld_prop->lib_major_version, lib_bld_prop->lib_minor_version,
                             lib_bld_prop->api_major_version, lib_bld_prop->api_minor_version,lib_bld_prop->hexagon_tools_version,
                             lib_bld_prop->is_qurt_used, lib_bld_prop->qurt_api_version);
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DSP: API Version %u.%u, Hexagon tools 0x%x, Qurt API version %u",
            adsp_avs_build_property.api_major_version, adsp_avs_build_property.api_minor_version,
            adsp_avs_build_property.hexagon_tools_version, adsp_avs_build_property.qurt_api_version);

      result = make_version_checks(filename_str, lib_bld_prop->api_major_version,
            lib_bld_prop->hexagon_tools_version, lib_bld_prop->is_qurt_used, lib_bld_prop->qurt_api_version);

      break;
   }
   case SHARED_LIB_API__SHARED_LIB_BUILD_PROPERTY_VERSION_V2:
      {
         shared_lib_build_property_v2_t *lib_bld_prop_v2 = (shared_lib_build_property_v2_t*)lib_bld_prop;

         MSG_SPRINTF_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                          ".so file %s ",filename_str);

         MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                                "SO: Lib version %u.%u, API version %u.%u, Hexagon tools 0x%x, is Qurt used %u, Qurt API version %u",
                                lib_bld_prop_v2->lib_major_version, lib_bld_prop_v2->lib_minor_version,
                                lib_bld_prop_v2->api_major_version, lib_bld_prop_v2->api_minor_version,lib_bld_prop_v2->hexagon_tools_version,
                                lib_bld_prop_v2->is_qurt_used, lib_bld_prop_v2->qurt_api_version);
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DSP: API Version %u.%u, Hexagon tools 0x%x, Qurt API version %u",
               adsp_avs_build_property.api_major_version, adsp_avs_build_property.api_minor_version,
               adsp_avs_build_property.hexagon_tools_version, adsp_avs_build_property.qurt_api_version);

         result = make_version_checks(filename_str, lib_bld_prop_v2->api_major_version,
               lib_bld_prop_v2->hexagon_tools_version, lib_bld_prop_v2->is_qurt_used, lib_bld_prop_v2->qurt_api_version);
         break;
      }
   default:
   {
      MSG_SPRINTF_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                       "build properties of %s mention property version %u, which is not supported now.",
                       filename_str, lib_bld_prop->build_property_version);
      return ADSP_EFAILED;
   }
   }
#endif

   return result;
}

/*** Default Callback mechanism **********************************************************/
struct adsp_amdb_default_callback_t
{
   qurt_elite_condvar_t condition_var;
   qurt_elite_nmutex_t nmutex;
   bool_t signal_set;
};

static void adsp_amdb_default_callback(void *context)
{
   adsp_amdb_default_callback_t *obj_ptr = reinterpret_cast<adsp_amdb_default_callback_t*>(context);
   qurt_elite_nmutex_lock(&obj_ptr->nmutex);
   obj_ptr->signal_set = TRUE;
   qurt_elite_condvar_signal(&obj_ptr->condition_var);
   qurt_elite_nmutex_unlock(&obj_ptr->nmutex);
}

static void adsp_amdb_init_callback_mechanism(adsp_amdb_default_callback_t *obj_ptr)
{
   qurt_elite_nmutex_init(&obj_ptr->nmutex);
   qurt_elite_condvar_init(&obj_ptr->condition_var);
   obj_ptr->signal_set = FALSE;
}

static void adsp_amdb_wait_deinit_callback_mechanism(adsp_amdb_default_callback_t *obj_ptr)
{
   qurt_elite_nmutex_lock(&obj_ptr->nmutex);
   while (!obj_ptr->signal_set)
   {
      qurt_elite_condvar_wait(&obj_ptr->condition_var, &obj_ptr->nmutex);
   }
   qurt_elite_nmutex_unlock(&obj_ptr->nmutex);
   qurt_elite_nmutex_destroy(&obj_ptr->nmutex);
   qurt_elite_condvar_destroy(&obj_ptr->condition_var);
}

/*** Client structure for adding modules **********************************************/
struct amdb_client_t
{
   hash_node hn;

   qurt_elite_atomic_word_t refs;

   uint32_t id;

   amdb_loader_handle *parallel_loader_handle;
   adsp_amdb_default_callback_t cb;
   volatile ADSPResult aggregate_error;
};

static amdb_client_t *amdb_client_create(uint32_t id)
{
   amdb_client_t *obj_ptr = (amdb_client_t*)qurt_elite_memory_malloc(sizeof(amdb_client_t), QURT_ELITE_HEAP_DEFAULT);
   if (NULL == obj_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AMDB: Failed to allocate the client structure.");
      return NULL;
   }

   memset(obj_ptr, 0, sizeof(*obj_ptr));

   obj_ptr->id = id;
   obj_ptr->aggregate_error = ADSP_EOK;
   qurt_elite_atomic_set(&obj_ptr->refs, 1);
   adsp_amdb_init_callback_mechanism(&obj_ptr->cb);

   obj_ptr->hn.key_ptr = &obj_ptr->id;
   obj_ptr->hn.key_size = sizeof(obj_ptr->id);

   return obj_ptr;
}

static void amdb_client_addref(amdb_client_t *obj_ptr)
{
   qurt_elite_atomic_increment(&obj_ptr->refs);
}

static bool_t amdb_client_is_loading_active(const amdb_client_t *obj_ptr)
{
   return (NULL != obj_ptr->parallel_loader_handle);
}

static void amdb_client_wait_for_done(amdb_client_t *obj_ptr)
{
   if (amdb_client_is_loading_active(obj_ptr))
   {
      amdb_loader_release_handle(obj_ptr->parallel_loader_handle);
      obj_ptr->parallel_loader_handle = NULL;
      adsp_amdb_wait_deinit_callback_mechanism(&obj_ptr->cb);
   }
}
static void amdb_client_release(amdb_client_t *obj_ptr)
{
   int refs = qurt_elite_atomic_decrement(&obj_ptr->refs);
   if (0 == refs)
   {
      amdb_client_wait_for_done(obj_ptr);
      qurt_elite_memory_free(obj_ptr);
   }
}

static void amdb_client_node_free(void* void_ptr, hash_node* node)
{
  amdb_client_release(STD_RECOVER_REC(amdb_client_t, hn, node));
}

static void amdb_client_set_error(amdb_client_t *obj_ptr, ADSPResult err)
{
   if (ADSP_FAILED(err))
   {
      obj_ptr->aggregate_error = ADSP_EFAILED;
   }
}

static ADSPResult amdb_client_get_aggregate_error(amdb_client_t* obj_ptr)
{
   return obj_ptr->aggregate_error;
}

/*** database management ******************************************************/

static const struct adsp_amdb_entry_config_t
{
   uint32_t entry_type;
   bool_t supports_overloading;
   char name[20];
} ADSP_AMDB_ENTRY_CONFIG[] = {
      // Note: These entries are in ascending order of value, which means descending order of priority.
      { ADSP_AMDB_ENTRY_TYPE_LEGACY, TRUE, "Legacy entry" },
      { ADSP_AMDB_ENTRY_TYPE_CLIENT, FALSE, "Client entry" },
      { ADSP_AMDB_ENTRY_TYPE_INIT, FALSE, "Init entry" },
      { ADSP_AMDB_ENTRY_TYPE_STATIC, FALSE, "Static entry"},
};

static const uint32_t ADSP_AMDB_NUM_ENTRY_TYPES = sizeof(ADSP_AMDB_ENTRY_CONFIG)/sizeof(ADSP_AMDB_ENTRY_CONFIG[0]);

struct adsp_amdb_t {
  hashtable ht[ADSP_AMDB_NUM_ENTRY_TYPES];
  bool_t is_ht_enabled[ADSP_AMDB_NUM_ENTRY_TYPES];
  qurt_elite_mutex_t mutex_node;

  amdb_parallel_loader *loader_ptr;

  hashtable client_objects_ht;
  qurt_elite_mutex_t client_objects_mutex;
};

static adsp_amdb_t adsp_amdb_;
static adsp_amdb_t* adsp_amdb_ptr_ = &adsp_amdb_;

struct adsp_amdb_node_t;

struct amdb_node_vtbl_t {
   ADSPResult (*resolve_symbols)(adsp_amdb_node_t *me, const char *f1_name_ptr, const char *f2_name_ptr); // If a function name is NULL, the internally stored names will be used
   void (*store_static_functions)(adsp_amdb_node_t *me, void *f1_ptr, void *f2_ptr);
   ADSPResult (*store_function_names)(adsp_amdb_node_t *me, const char *f1_name_ptr, const char *f2_name_ptr);
   void (*clear_symbols)(adsp_amdb_node_t *me);
   void (*end)(adsp_amdb_node_t *me);
};

struct adsp_amdb_node_t {
  hash_node hn;
  qurt_elite_atomic_word_t mem_refs; // tracks the number of entities holding a pointer to this struct.

  unsigned int dl_refs; // tracks the number of entities that can use this module.
  qurt_elite_mutex_t dl_mutex; // To prevent multiple dlopen calls for this module

  int key[3];
  char filename_str[256];
  bool_t is_static;

  uint32_t client_id;

  void* h;
  bool_t preload;

  adsp_amdb_interface_type interface_type;
  const amdb_node_vtbl_t *vtbl;
};

static ADSPResult node_wrapper_dlopen(adsp_amdb_node_t* me, const char* filename_str,
                               const char* f1_name_str = NULL,
                               const char* f2_name_str = NULL)
{
    ADSPResult result = ADSP_EOK;

    me->h = dlopen(filename_str, RTLD_NOW);
    if (0 == me->h) {
      MSG_SPRINTF_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "adsp amdb: failed to open %s, %s",
                  filename_str, dlerror());
      goto err_cleanup_1;
    }

    if (ADSP_EOK != check_version_mismatch(me->h, filename_str))
    {
       MSG_SPRINTF_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "adsp amdb: rejecting due to version mismatch %s",
                   filename_str);
       goto err_cleanup_2;
    }

    result = me->vtbl->resolve_symbols(me, f1_name_str, f2_name_str);
    if (ADSP_FAILED(result))
    {
       goto err_cleanup_2;
    }

    return result;

err_cleanup_2:
    dlclose(me->h);
    me->h=NULL;
err_cleanup_1:
    return ADSP_EFAILED;
}

static adsp_amdb_module_handle_info_t::adsp_amdb_handle_union amdb_node_to_handle(adsp_amdb_node_t *node_ptr)
{
   adsp_amdb_module_handle_info_t::adsp_amdb_handle_union h;
   h.capi_v2_handle = reinterpret_cast<adsp_amdb_capi_v2_t*>(node_ptr);
   return h;
}

static adsp_amdb_node_t *amdb_handle_to_node(adsp_amdb_module_handle_info_t::adsp_amdb_handle_union h)
{
   return reinterpret_cast<adsp_amdb_node_t*>(h.capi_v2_handle);
}

static uint32_t adsp_amdb_node_addref_mem(adsp_amdb_node_t *me)
{
  return qurt_elite_atomic_increment(&me->mem_refs);
}

static uint32_t adsp_amdb_node_addref(adsp_amdb_node_t* me)
{
  int refs;
  refs = adsp_amdb_node_addref_mem(me);
  qurt_elite_mutex_lock(&me->dl_mutex);
  me->dl_refs++;
  qurt_elite_mutex_unlock(&me->dl_mutex);

  return refs;
}

static uint32_t adsp_amdb_node_release_int(adsp_amdb_node_t* me, bool_t release_dyn_ref)
{
   int refs;

   if (release_dyn_ref) {
      // Unload the so file if no longer needed.
      qurt_elite_mutex_lock(&me->dl_mutex);
      me->dl_refs--;
      if (0 == me->dl_refs) {
         if(0 != me->h) {
            dlclose(me->h);
            me->h = 0;
         }
      }
      qurt_elite_mutex_unlock(&me->dl_mutex);
   }

   // Free the memory if no longer needed.
   refs = qurt_elite_atomic_decrement(&me->mem_refs);
   if (0 == refs) {
      me->vtbl->end(me);
      qurt_elite_memory_free(me);
      me = NULL;
   }

   return refs;
}

static uint32_t adsp_amdb_node_release(adsp_amdb_node_t* me)
{
   return adsp_amdb_node_release_int(me, TRUE);
}

static uint32_t adsp_amdb_node_destroy(adsp_amdb_node_t* me)
{
  return adsp_amdb_node_release_int(me, me->preload);
}
static void adsp_amdb_node_free(void* void_ptr, hash_node* node)
{
  adsp_amdb_node_destroy(STD_RECOVER_REC(adsp_amdb_node_t, hn, node));
}

static void node_init(adsp_amdb_node_t* me, int type, int id1, int id2)
{
   memset(me, 0, sizeof(*me));

   qurt_elite_atomic_set(&me->mem_refs, 1);
   me->dl_refs = 0;
   qurt_elite_mutex_init(&me->dl_mutex);

   me->key[0] = type;
   me->key[1] = id1;
   me->key[2] = id2;
   me->client_id = 0;

   me->hn.key_size = sizeof(me->key);
   me->hn.key_ptr = &me->key[0];
}

union adsp_amdb_preload_task_t
{
   uint64_t data;
   struct task_info_t
   {
      adsp_amdb_node_t *node_ptr;
      ADSPResult *result_ptr;
   } task_info;
};

static void amdb_preload_module_loader(uint64_t task_info)
{
   adsp_amdb_preload_task_t task;
   task.data = task_info;
   adsp_amdb_node_t *me = task.task_info.node_ptr;

   qurt_elite_mutex_lock(&me->dl_mutex);
   // Load only if the entry still exists in the database. If we don't check for this, dlclose will never be called.
   if (me->dl_refs > 0)
   {
      ADSPResult err = node_wrapper_dlopen(me, me->filename_str);
      if (ADSP_EOK != err)
      {
         MSG_SPRINTF_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "adsp amdb: Encountered error for dynamic module %s",
               me->filename_str);

         if (NULL != task.task_info.result_ptr)
         {
            *task.task_info.result_ptr = err;
         }

         // Update the aggregate error
         adsp_amdb_t* amdb_ptr = adsp_amdb_ptr_;
         amdb_client_t *client_ptr = NULL;
         qurt_elite_mutex_lock(&amdb_ptr->client_objects_mutex);
         hash_node *phn = hashtable_find(&amdb_ptr->client_objects_ht, &me->client_id, sizeof(me->client_id));
         if (NULL != phn)
         {
            client_ptr = STD_RECOVER_REC(amdb_client_t, hn, phn);
            amdb_client_addref(client_ptr);
         }
         qurt_elite_mutex_unlock(&amdb_ptr->client_objects_mutex);

         if(NULL != client_ptr)
         {
            amdb_client_set_error(client_ptr, err);
            amdb_client_release(client_ptr);
            client_ptr = NULL;
         }
         else
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AMDB: Cannot find the client structure for module (%d, %d, %d). Cannot fill in the error value.", me->key[0], me->key[1], me->key[2]);
         }
      }
   }
   qurt_elite_mutex_unlock(&me->dl_mutex);

   adsp_amdb_node_release_int(me, FALSE);
}

static ADSPResult node_init_symbols(adsp_amdb_node_t* me, void *f1, void *f2, bool_t preload, bool_t is_static,
                               const char* filename_str,
                               const char* f1_str,
                               const char* f2_str)
{
   uint32_t err;

   if (is_static) {
      me->is_static = TRUE;
      me->vtbl->store_static_functions(me, f1, f2);
      me->dl_refs = 1;
      me->preload = TRUE;
   }else {
      me->preload = preload;
      if (NULL == filename_str) {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "adsp amdb: Null filename encountered for dynamic module");
         THROW(err, ADSP_EFAILED);
      }
      strlcpy(me->filename_str, filename_str, sizeof(me->filename_str));

      if (ADSP_EOK != me->vtbl->store_function_names(me, f1_str, f2_str))
      {
         THROW(err, ADSP_EFAILED);
      }
      if (me->preload)
      {
      me->dl_refs = 1;
      }
      else
      {
         me->dl_refs = 0;
      }
   }
   THROW(err, ADSP_EOK);

   CATCH(err) {
   }
   return err;
}

static void amdb_push_preload_task(adsp_amdb_node_t *me, ADSPResult *dyn_loading_error_ptr)
{
   adsp_amdb_t* amdb_ptr = adsp_amdb_ptr_;
   amdb_client_t *client_ptr = NULL;
   int err = ADSP_EOK;

   qurt_elite_mutex_lock(&amdb_ptr->client_objects_mutex);
   hash_node *phn = hashtable_find(&amdb_ptr->client_objects_ht, &me->client_id, sizeof(me->client_id));
   if (NULL != phn)
   {
      client_ptr = STD_RECOVER_REC(amdb_client_t, hn, phn);
      amdb_client_addref(client_ptr);
   }
   qurt_elite_mutex_unlock(&amdb_ptr->client_objects_mutex);

   // Check if the client object exists
   // If not, create the client object and insert it. Since clients have
   // to be single threaded, no other can thread will be creating an object
   // with the same id.
   if (NULL == client_ptr)
   {
      client_ptr = amdb_client_create(me->client_id);
      if (NULL == client_ptr)
      {
         return;
      }

      client_ptr->parallel_loader_handle = amdb_loader_get_handle(amdb_ptr->loader_ptr, adsp_amdb_default_callback, &client_ptr->cb, amdb_preload_module_loader);
      QURT_ELITE_ASSERT(NULL != client_ptr->parallel_loader_handle);

      qurt_elite_mutex_lock(&amdb_ptr->client_objects_mutex);
      amdb_client_addref(client_ptr);
      err = hashtable_insert(&amdb_ptr->client_objects_ht, &client_ptr->hn);
      qurt_elite_mutex_unlock(&amdb_ptr->client_objects_mutex);

      if (ADSP_FAILED(err))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "amdb: Failed to insert client node into hash table. Loading cannot proceed.");
         amdb_client_release(client_ptr);
         client_ptr = NULL;
         return;
      }
   }

   // Push the task
   adsp_amdb_node_addref_mem(me);
   adsp_amdb_preload_task_t task;
   task.data = 0;
   task.task_info.node_ptr = me;
   task.task_info.result_ptr = dyn_loading_error_ptr;
   amdb_loader_push_task(client_ptr->parallel_loader_handle, task.data);

   amdb_client_release(client_ptr);
}

adsp_amdb_node_t *amdb_node_hashtable_find(hashtable *ht, const void *key_ptr, int key_size)
{
   adsp_amdb_node_t *node_ptr = NULL;
   node_ptr = NULL;
   /*
     NOTE:
     Previous behaviour: Previously the preload node used to be removed if the mem_refs == 1 and node_ptr->h == NULL
                         This was done to remove any failed-to-dl_load preload modules when get_modules_request is done.
     Current behaviour: This was modified to not remove any preload nodes. Because even if the preload module fails at loading
                        at preload stage, there might be a possibility that it will succeed if you retry during use-case stage
   */

   hash_node *phn = hashtable_find(ht, key_ptr, key_size);
   if (!phn)
   {
      return NULL;
   }

   node_ptr = STD_RECOVER_REC(adsp_amdb_node_t, hn, phn);
   return node_ptr;
}

static ADSPResult adsp_amdb_get_node(adsp_amdb_module_handle_info_t *module_info_ptr, bool_t *to_be_loaded)
{
  adsp_amdb_t* me = adsp_amdb_ptr_;
  adsp_amdb_node_t* node_ptr = NULL;
  int key[3];
  ADSPResult err = ADSP_EOK;
  *to_be_loaded = FALSE;

  key[0] = module_info_ptr->type;
  key[1] = module_info_ptr->id1;
  key[2] = module_info_ptr->id2;

  uint32_t entry_type=0xff;
  qurt_elite_mutex_lock(&me->mutex_node);
  for (uint32_t i = 0; i < ADSP_AMDB_NUM_ENTRY_TYPES; i++)
  {
     if (me->is_ht_enabled[i])
     {
        node_ptr = amdb_node_hashtable_find(&me->ht[i], &key, sizeof(key));
        if (node_ptr)
        {
           entry_type=ADSP_AMDB_ENTRY_CONFIG[i].entry_type;

           qurt_elite_mutex_lock(&node_ptr->dl_mutex);
           if (!node_ptr->is_static)
           {
              if (NULL == node_ptr->h)
              {
                 if (node_ptr->preload)
                 {
                    // Case where module load has failed when preloading, we try to reload the moduale again
                    if(1 == qurt_elite_atomic_get(&node_ptr->mem_refs))
                    {
                       *to_be_loaded = TRUE;
                    }
                    else
                    {
                       // The module is still being loaded. We can try to get a module from another hash table.
                       qurt_elite_mutex_unlock(&node_ptr->dl_mutex);
                       node_ptr = NULL;
                       continue;
                    }
                 }
                 else
                 {
                    *to_be_loaded = TRUE;
                 }
              }
              else
              {
                 node_ptr->dl_refs++;
              }
           }
           else
           {
              node_ptr->dl_refs++;
           }
           qurt_elite_mutex_unlock(&node_ptr->dl_mutex);

           module_info_ptr->interface_type = node_ptr->interface_type;
           module_info_ptr->h = amdb_node_to_handle(node_ptr);

           adsp_amdb_node_addref_mem(node_ptr);
        break;
     }
  }
  }
  qurt_elite_mutex_unlock(&me->mutex_node);

  if (NULL == node_ptr) {
     THROW(err, ADSP_EFAILED);
  }


  MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AMDB_GET: Module (%lu, 0x%x, 0x%lx) from entry-type 0x%x, is_static %u, to_be_loaded %u, interface %d",
        module_info_ptr->type, module_info_ptr->id1, module_info_ptr->id2, entry_type, node_ptr->is_static, *to_be_loaded, module_info_ptr->interface_type);

  CATCH(err) {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AMDB_GET: Failed to get module (%lu, 0x%x, 0x%lx)",
          module_info_ptr->type, module_info_ptr->id1, module_info_ptr->id2);
  }
  return err;
}

static hashtable *adsp_amdb_get_hashtable(uint32_t entry_type)
{
   adsp_amdb_t* me = adsp_amdb_ptr_;
   hashtable *ht = NULL;
   for (uint32_t i = 0; i < ADSP_AMDB_NUM_ENTRY_TYPES; i++)
   {
      if (entry_type == ADSP_AMDB_ENTRY_CONFIG[i].entry_type)
      {
         ht = &me->ht[i];
         break;
      }
   }

   return ht;
}

static ADSPResult adsp_amdb_remove_node_with_entry_type(uint32_t entry_type, int type, int id1, int id2)
{
   adsp_amdb_t* me = adsp_amdb_ptr_;
   adsp_amdb_node_t* node_ptr;
   ADSPResult err;

   MSG_4(MSG_SSID_QDSP6, DBG_LOW_PRIO,"AMDB_REMOVE: remove module (%lu, 0x%x, 0x%lx) from entry-type 0x%x", type,id1,id2,entry_type);
   hashtable *ht = adsp_amdb_get_hashtable(entry_type);
   if (NULL == ht)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "adsp_amdb: failed to find entry type %lx", entry_type);
      THROW(err, ADSP_EFAILED);
   }

   int key[3];
   key[0] = type;
   key[1] = id1;
   key[2] = id2;
   qurt_elite_mutex_lock(&me->mutex_node);
   node_ptr = amdb_node_hashtable_find(ht, &key, sizeof(key));
   if (node_ptr) {
      err = hashtable_remove(ht, &node_ptr->key, sizeof(node_ptr->key),
            &node_ptr->hn);
      if (ADSP_EOK == err) {
         qurt_elite_mutex_unlock(&me->mutex_node);
         THROW(err, ADSP_EOK);
      }
   }
   qurt_elite_mutex_unlock(&me->mutex_node);
   THROW(err, ADSP_EFAILED);

   CATCH(err) 
   {
	   MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AMDB_REMOVE: remove failed for module (%lu, 0x%x, 0x%lx) from entry-type 0x%x", type,id1,id2,entry_type);
   }
   return err;
}

static ADSPResult adsp_amdb_remove_node(int type, int id1, int id2,
                                    const char* filename_str)
{
  adsp_amdb_t* me = adsp_amdb_ptr_;
  adsp_amdb_node_t* node_ptr;
  hash_node* phn = 0;
  ADSPResult err = ADSP_EOK;

  if ((NULL != filename_str) && ('\0' != filename_str[0])) {
    // remove all contained in this file
    uint32_t n = 0;
    MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO,"AMDB_REMOVE: remove node (%lu, 0x%x, 0x%lx) ", type,id1,id2);

    hashtable *ht = adsp_amdb_get_hashtable(ADSP_AMDB_ENTRY_TYPE_LEGACY);
    if (NULL == ht)
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "adsp amdb: failed to find legacy hashtable");
       THROW(err, ADSP_EFAILED);
    }

    qurt_elite_mutex_lock(&me->mutex_node);
    while (0 != (phn = hashtable_getnext(ht, &n))) {
      node_ptr = STD_RECOVER_REC(adsp_amdb_node_t, hn, phn);
      if (!node_ptr->is_static && !strncmp(node_ptr->filename_str,
                                              filename_str,
                                              sizeof(node_ptr->filename_str))) {
        if (ADSP_EOK == hashtable_remove(ht, &node_ptr->key,
                                         sizeof(node_ptr->key), phn)) {
          n--;
        }
      }
    }
    qurt_elite_mutex_unlock(&me->mutex_node);
    if(err!=ADSP_EOK)
    {
       MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AMDB_REMOVE: remove failed for node (%lu, 0x%x, 0x%lx) ", type,id1,id2);
    }
    THROW(err, ADSP_EOK);
  } else {
     return adsp_amdb_remove_node_with_entry_type(ADSP_AMDB_ENTRY_TYPE_LEGACY, type, id1, id2);
  }

  CATCH(err) {
  }
  return err;
}

static ADSPResult adsp_amdb_remove_all_nodes(void)
{
  adsp_amdb_t* me = adsp_amdb_ptr_;
  adsp_amdb_node_t* capi_v2_ptr;
  ADSPResult err = ADSP_EOK;
  uint32_t n = 0;
  hash_node* phn = 0;

  qurt_elite_mutex_lock(&me->mutex_node);
  hashtable *ht = adsp_amdb_get_hashtable(ADSP_AMDB_ENTRY_TYPE_LEGACY);
  if (NULL == ht)
  {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "adsp amdb: failed to find legacy hashtable");
     qurt_elite_mutex_unlock(&me->mutex_node);
     THROW(err, ADSP_EFAILED);
  }

  while (0 != (phn = hashtable_getnext(ht, &n))) {
    capi_v2_ptr = STD_RECOVER_REC(adsp_amdb_node_t, hn, phn);
    if (!capi_v2_ptr->is_static) {
        if (ADSP_EOK == hashtable_remove(ht, &capi_v2_ptr->key,
                                       sizeof(capi_v2_ptr->key), phn)) {
        n--;
      }
    }
  }
  qurt_elite_mutex_unlock(&me->mutex_node);

  CATCH(err) {
  }
  return err;
}

static ADSPResult adsp_amdb_print_all_nodes(void)
{
  adsp_amdb_t* me = adsp_amdb_ptr_;
  adsp_amdb_node_t* node_ptr;

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "adsp_amdb: print common db");
  qurt_elite_mutex_lock(&me->mutex_node);
  for (uint32_t i = 0; i < ADSP_AMDB_NUM_ENTRY_TYPES; i++)
  {
     uint32_t n = 0;
     hash_node* phn = 0;
     MSG_SPRINTF_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
           "adsp_amdb: entry type %s", ADSP_AMDB_ENTRY_CONFIG[i].name);
     while (0 != (phn = hashtable_getnext(&me->ht[i], &n))) {
        node_ptr = STD_RECOVER_REC(adsp_amdb_node_t, hn, phn);

        MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
              "adsp_amdb: interface_type %d, module-id (%d, 0x%x, 0x%x). is static %u",
              node_ptr->interface_type,
              node_ptr->key[0], node_ptr->key[1], node_ptr->key[2],
              node_ptr->is_static);
        MSG_SPRINTF_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
              "adsp_amdb: file name  %s", node_ptr->filename_str);
     }
  }
  qurt_elite_mutex_unlock(&me->mutex_node);

  return ADSP_EOK;
}

static bool_t amdb_is_module_conflict(uint32_t entry_type, int type, int id1, int id2)
{
   uint32_t err = ADSP_EOK;
   adsp_amdb_t* me = adsp_amdb_ptr_;
   hashtable *ht = NULL;
   bool_t supports_overloading = TRUE;
   const char *name = NULL;
   int key[3] = { type, id1, id2 };

   for (uint32_t i = 0; i < ADSP_AMDB_NUM_ENTRY_TYPES; i++)
   {
      if (entry_type == ADSP_AMDB_ENTRY_CONFIG[i].entry_type)
      {
         ht = &me->ht[i];
         supports_overloading = ADSP_AMDB_ENTRY_CONFIG[i].supports_overloading;
         name = ADSP_AMDB_ENTRY_CONFIG[i].name;
         break;
      }
   }
   if (NULL == ht)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "adsp_amdb: failed to find entry type %lx", entry_type);
      THROW(err, ADSP_EFAILED);
   }

   if (!supports_overloading)
   {
      qurt_elite_mutex_lock(&me->mutex_node);
      hash_node* phn = 0;
      phn = hashtable_find(ht, key, sizeof(key));
      if (phn)
      {
         qurt_elite_mutex_unlock(&me->mutex_node);
         MSG_SPRINTF_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "adsp_amdb: AMDB entry type %s does not support overloading. Entry already exists.", name);
         THROW(err, ADSP_EALREADY);
      }
      qurt_elite_mutex_unlock(&me->mutex_node);
   }

   CATCH(err) {
   }

   if (ADSP_FAILED(err))
   {
      return TRUE;
   }

   return FALSE;
}

static ADSPResult amdb_insert_node(uint32_t entry_type, adsp_amdb_node_t *node_ptr)
{
   uint32_t err = ADSP_EOK;
   adsp_amdb_t* me = adsp_amdb_ptr_;
   hashtable *ht = NULL;
   bool_t supports_overloading = TRUE;
   const char *name = NULL;

   for (uint32_t i = 0; i < ADSP_AMDB_NUM_ENTRY_TYPES; i++)
   {
      if (entry_type == ADSP_AMDB_ENTRY_CONFIG[i].entry_type)
      {
         ht = &me->ht[i];
         supports_overloading = ADSP_AMDB_ENTRY_CONFIG[i].supports_overloading;
         name = ADSP_AMDB_ENTRY_CONFIG[i].name;
         break;
      }
   }
   if (NULL == ht)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "adsp_amdb: failed to find entry type %lx", entry_type);
      THROW(err, ADSP_EFAILED);
   }

   qurt_elite_mutex_lock(&me->mutex_node);
   if (!supports_overloading)
   {
      adsp_amdb_node_t *existing_node = amdb_node_hashtable_find(ht, node_ptr->hn.key_ptr, node_ptr->hn.key_size);
      if (existing_node)
      {
         qurt_elite_mutex_unlock(&me->mutex_node);
         MSG_SPRINTF_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "adsp_amdb: AMDB entry type %s does not support overloading. Entry already exists.", name);
         THROW(err, ADSP_EALREADY);
      }
   }
   err = hashtable_insert(ht, &node_ptr->hn);
   if(ADSP_EOK == err)
   {
      adsp_amdb_node_addref_mem(node_ptr);
   }
   qurt_elite_mutex_unlock(&me->mutex_node);
   if (ADSP_EOK != err) {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "adsp amdb: hashtable_insert() failed %lu", err);
      THROW(err, err);
   }


   CATCH(err) {
   }

   return err;
}

static ADSPResult amdb_add_node(uint32_t entry_type, adsp_amdb_node_t *node_ptr, void *f1, void *f2, bool_t preload, bool_t is_static, const char* filename_str, const char* f1_str, const char* f2_str, ADSPResult *dyn_loading_error_ptr, uint32_t client_id)
{
   ADSPResult err = ADSP_EOK;

   if (NULL != dyn_loading_error_ptr)
   {
      *dyn_loading_error_ptr = ADSP_EOK;
   }

   err = node_init_symbols(node_ptr, f1, f2, preload,is_static, filename_str, f1_str, f2_str);
   if (ADSP_EOK != err) {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "adsp amdb: init symbols failed %d", err);
      THROW(err, err);
   }

   node_ptr->client_id = client_id;

   err = amdb_insert_node(entry_type, node_ptr);
   if (ADSP_EOK != err) {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "adsp amdb: node insertion failed %d", err);
      THROW(err, err);
   }

   if (node_ptr->preload && !node_ptr->is_static)
   {
      amdb_push_preload_task(node_ptr, dyn_loading_error_ptr);
   }

   // We can release the current ref count since the hashtable and the preload task code each add their own ref counts.
   adsp_amdb_node_release_int(node_ptr, FALSE);

   CATCH(err) {
   }
   return err;
}

/*** init/deinit **************************************************************/
union adsp_amdb_non_preload_task_t
{
   uint64_t data;
   adsp_amdb_module_handle_info_t *handle_info_ptr;
};
extern "C" {
static void adsp_amdb_non_preload_module_loader(uint64_t task);
}

ADSPResult adsp_amdb_init(void)
{
  adsp_amdb_t* me = adsp_amdb_ptr_;
  ADSPResult err = ADSP_EOK;

  me->loader_ptr = amdb_loader_create();
  if (NULL == me->loader_ptr) {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "amdb: amdb loader init failed");
    THROW(err, ADSP_EFAILED);
  }

  for (uint32_t i = 0; i < ADSP_AMDB_NUM_ENTRY_TYPES; i++)
  {
     err = hashtable_init(&me->ht[i], 16, 2, adsp_amdb_node_free, me);
     if (ADSP_EOK != err) {
        MSG_SPRINTF_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "amdb: hashtable of type %s init failed %d", ADSP_AMDB_ENTRY_CONFIG[i].name, err);
        THROW(err, err);
     }
     me->is_ht_enabled[i] = TRUE;
  }
  qurt_elite_mutex_init(&me->mutex_node);

  err = hashtable_init(&me->client_objects_ht, 4, 2, amdb_client_node_free, me);
  if (ADSP_EOK != err) {
     MSG_SPRINTF_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "amdb: hashtable for client ids init failed %d", err);
     THROW(err, err);
  }
  qurt_elite_mutex_init(&me->client_objects_mutex);

  THROW(err, ADSP_EOK);

  CATCH(err) {
  }
  return err;
}

void adsp_amdb_deinit(void)
{
  adsp_amdb_t* me = adsp_amdb_ptr_;

  qurt_elite_mutex_lock(&me->client_objects_mutex);
  (void) hashtable_remove_all(&me->client_objects_ht);
  qurt_elite_mutex_unlock(&me->client_objects_mutex);
  qurt_elite_mutex_destroy(&me->client_objects_mutex);
  hashtable_deinit(&me->client_objects_ht);

  qurt_elite_mutex_lock(&me->mutex_node);
  for (uint32_t i = 0; i < ADSP_AMDB_NUM_ENTRY_TYPES; i++)
  {
     (void)hashtable_remove_all(&me->ht[i]);
     hashtable_deinit(&me->ht[i]);
  }
  qurt_elite_mutex_unlock(&me->mutex_node);
  qurt_elite_mutex_destroy(&me->mutex_node);

  amdb_loader_destroy(me->loader_ptr);
  me->loader_ptr = NULL;
}


/*** capi_v2 ******************************************************************/
struct adsp_amdb_capi_v2_t
{
   adsp_amdb_node_t node; // Must be the first element of the structure.

   capi_v2_get_static_properties_f get_static_properties_f;
   capi_v2_init_f init_f;
   char* get_static_properties_str;
   char* init_str;
};

/*** capi_v2 - wrapper ********************************************************/

typedef ADSPResult(* capi_v2_wrapper_orig_end_f)(capi_v2_t* capi_v2_ptr);

// capi_v2_wrapper hijacks the objects vtbl adds a wrapper to the end function
// this method of tracking module lifetime is not the cleaneset due to the
// assumption that we can point the object to the wrappers vtbl without any
// ill side effect.  But, it does provide for the least overhead since the rest
// of the capi_v2 functions are direct calls
typedef struct capi_v2_wrapper_t
{
  const capi_v2_vtbl_t* pvtbl;
  capi_v2_t* icapi_v2;
  adsp_amdb_capi_v2_t* capi_v2_ptr;
}
capi_v2_wrapper_t;

static capi_v2_err_t capi_v2_wrapper_process(capi_v2_t* icapi_v2,
                                             capi_v2_stream_data_t* input[],
                                             capi_v2_stream_data_t* output[])
{
  capi_v2_wrapper_t* me = (capi_v2_wrapper_t*)icapi_v2;
  return me->icapi_v2->vtbl_ptr->process(me->icapi_v2, input, output);
}

static capi_v2_err_t capi_v2_wrapper_end(capi_v2_t* icapi_v2)
{
  capi_v2_wrapper_t* me = (capi_v2_wrapper_t*)icapi_v2;

  me->icapi_v2->vtbl_ptr->end(me->icapi_v2);
  adsp_amdb_node_release(&me->capi_v2_ptr->node);

  return CAPI_V2_EOK;
}

static capi_v2_err_t capi_v2_wrapper_set_param(capi_v2_t* icapi_v2,
                                               uint32_t param_id,
                                               const capi_v2_port_info_t* port_info_ptr,
                                               capi_v2_buf_t* params_ptr)
{
  capi_v2_wrapper_t* me = (capi_v2_wrapper_t*)icapi_v2;
  return me->icapi_v2->vtbl_ptr->set_param(me->icapi_v2, param_id, port_info_ptr, params_ptr);
}

static capi_v2_err_t capi_v2_wrapper_get_param(capi_v2_t* icapi_v2,
                                               uint32_t param_id,
                                               const capi_v2_port_info_t* port_info_ptr,
                                               capi_v2_buf_t* params_ptr)
{
  capi_v2_wrapper_t* me = (capi_v2_wrapper_t*)icapi_v2;
  return me->icapi_v2->vtbl_ptr->get_param(me->icapi_v2, param_id, port_info_ptr, params_ptr);
}

static capi_v2_err_t capi_v2_wrapper_set_properties(capi_v2_t* icapi_v2,
                                                    capi_v2_proplist_t* props_ptr)
{
  capi_v2_wrapper_t* me = (capi_v2_wrapper_t*)icapi_v2;
  return me->icapi_v2->vtbl_ptr->set_properties(me->icapi_v2, props_ptr);
}

static capi_v2_err_t capi_v2_wrapper_get_properties(capi_v2_t* icapi_v2,
                                                    capi_v2_proplist_t* props_ptr)
{
  capi_v2_wrapper_t* me = (capi_v2_wrapper_t*)icapi_v2;
  return me->icapi_v2->vtbl_ptr->get_properties(me->icapi_v2, props_ptr);
}

static const capi_v2_vtbl_t capi_v2_vtbl_wrapper = {
  capi_v2_wrapper_process,
  capi_v2_wrapper_end,
  capi_v2_wrapper_set_param,
  capi_v2_wrapper_get_param,
  capi_v2_wrapper_set_properties,
  capi_v2_wrapper_get_properties,
};

static ADSPResult capi_v2_wrapper_init(capi_v2_wrapper_t* me, capi_v2_t* icapi_v2,
                                adsp_amdb_capi_v2_t* capi_v2_ptr)
{
  me->pvtbl = &capi_v2_vtbl_wrapper;
  me->icapi_v2 = icapi_v2;
  me->capi_v2_ptr = capi_v2_ptr;

  (void)adsp_amdb_node_addref(&capi_v2_ptr->node);
  return ADSP_EOK;
}

capi_v2_err_t adsp_amdb_capi_v2_get_static_properties_f(adsp_amdb_capi_v2_t* me,
                                                        capi_v2_proplist_t* init_set_properties,
                                                        capi_v2_proplist_t* static_properties)
{
   ADSPResult err = CAPI_V2_EOK;
   capi_v2_prop_t* prop_ptr = 0;
   uint32_t size = 0;
   uint32_t i;

   TRY(err, me->get_static_properties_f(init_set_properties, static_properties));

   for (i = 0; i < static_properties->props_num; i++) {
      if (static_properties->prop_ptr[i].id == CAPI_V2_INIT_MEMORY_REQUIREMENT) {
         prop_ptr = &static_properties->prop_ptr[i];
      }
   }
   if (0 != prop_ptr) {
      size = ((capi_v2_init_memory_requirement_t*)prop_ptr->payload.data_ptr)->size_in_bytes;
      size = align_to_8_byte(size);
      size += align_to_8_byte(sizeof(capi_v2_wrapper_t));
      ((capi_v2_init_memory_requirement_t*)prop_ptr->payload.data_ptr)->size_in_bytes = size;
   }

   CATCH(err) {
   }
   return err;
}

capi_v2_err_t adsp_amdb_capi_v2_init_f(adsp_amdb_capi_v2_t* me,
                                       capi_v2_t* icapi_v2,
                                       capi_v2_proplist_t* init_set_properties)
{
  capi_v2_wrapper_t* wrapper_ptr = (capi_v2_wrapper_t*)icapi_v2;
  ADSPResult err = CAPI_V2_EOK;

  icapi_v2 = (capi_v2_t*)((char*)wrapper_ptr +
                          align_to_8_byte(sizeof(capi_v2_wrapper_t)));

  TRY(err, me->init_f(icapi_v2, init_set_properties));
  TRY(err, capi_v2_wrapper_init(wrapper_ptr, icapi_v2, me));

  CATCH(err) {
  }
  
  return err;
}

static ADSPResult capi_v2_resolve_symbols(adsp_amdb_node_t *node_ptr, const char *f1_name_ptr = NULL, const char *f2_name_ptr = NULL) // If a function name is NULL, the internally stored names will be used
{
   adsp_amdb_capi_v2_t *me = reinterpret_cast<adsp_amdb_capi_v2_t*>(node_ptr);

   if (NULL == f1_name_ptr)
   {
      f1_name_ptr = me->get_static_properties_str;
   }
   if (NULL == f2_name_ptr)
   {
      f2_name_ptr = me->init_str;
   }

   me->get_static_properties_f = (capi_v2_get_static_properties_f)dlsym(me->node.h, f1_name_ptr);
   if (!me->get_static_properties_f) {
      MSG_SPRINTF_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "capi_v2: dlsym failed %s, %s",
            f1_name_ptr, dlerror());
      return ADSP_EFAILED;
   }

   me->init_f = (capi_v2_init_f)dlsym(me->node.h, f2_name_ptr);
   if (!me->init_f) {
      MSG_SPRINTF_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "capi_v2: dlsym failed %s , %s",
            f2_name_ptr, dlerror());
      return ADSP_EFAILED;
   }

   return ADSP_EOK;
}

static ADSPResult capi_v2_store_function_names(adsp_amdb_node_t *node_ptr, const char *f1_name_ptr, const char *f2_name_ptr)
{
   adsp_amdb_capi_v2_t *me = reinterpret_cast<adsp_amdb_capi_v2_t*>(node_ptr);

   ALLOC_AND_COPY_STRING(me->get_static_properties_str, f1_name_ptr, strlen(f1_name_ptr));
   ALLOC_AND_COPY_STRING(me->init_str, f2_name_ptr, strlen(f2_name_ptr));

   if ((NULL == me->get_static_properties_str) || (NULL == me->init_str))
   {
      return ADSP_ENOMEMORY;
   }

   return ADSP_EOK;
}

static void capi_v2_store_static_functions(adsp_amdb_node_t *node_ptr, void *f1_ptr, void *f2_ptr)
{
   adsp_amdb_capi_v2_t *me = reinterpret_cast<adsp_amdb_capi_v2_t*>(node_ptr);

   me->get_static_properties_f = (capi_v2_get_static_properties_f)(f1_ptr);
   me->init_f = (capi_v2_init_f)(f2_ptr);
}

static void capi_v2_clear_symbols(adsp_amdb_node_t *node_ptr)
{
   adsp_amdb_capi_v2_t *me = reinterpret_cast<adsp_amdb_capi_v2_t*>(node_ptr);

   me->get_static_properties_f = NULL;
   me->init_f = NULL;
}

static void capi_v2_end(adsp_amdb_node_t *node_ptr)
{
   adsp_amdb_capi_v2_t *me = reinterpret_cast<adsp_amdb_capi_v2_t*>(node_ptr);

   me->node.vtbl->clear_symbols(&me->node);

   if (NULL != me->get_static_properties_str)
   {
      qurt_elite_memory_free(me->get_static_properties_str);
      me->get_static_properties_str = NULL;
   }
   if (NULL != me->init_str)
   {
      qurt_elite_memory_free(me->init_str);
      me->init_str = NULL;
   }
}

static const amdb_node_vtbl_t capi_v2_node_vtbl = {
      capi_v2_resolve_symbols,
      capi_v2_store_static_functions,
      capi_v2_store_function_names,
      capi_v2_clear_symbols,
      capi_v2_end
};

static ADSPResult adsp_amdb_addInternal_capi_v2(uint32_t entry_type, bool_t preload, int type, int id1, int id2,
                                                capi_v2_get_static_properties_f get_static_properties_f,
                                                capi_v2_init_f init_f,
                                                const char* filename_str,
                                                const char* get_static_properties_str,
                                                const char* init_str,
                                                ADSPResult *dyn_loading_error_ptr,
                                                uint32_t client_id)
{
  adsp_amdb_capi_v2_t* capi_v2_ptr = NULL;
  ADSPResult err = ADSP_EOK;
  bool_t hashnode_initialized = FALSE;
  bool_t is_static = FALSE;

  MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AMDB_ADD: CAPI_V2: (%lu, 0x%x, 0x%lx) to entry-type 0x%x, preload %u", type,id1,id2,entry_type,preload);

  if (amdb_is_module_conflict(entry_type, type, id1, id2))
  {
     THROW(err, ADSP_EALREADY);
  }

  capi_v2_ptr = (adsp_amdb_capi_v2_t*)qurt_elite_memory_malloc(sizeof(adsp_amdb_capi_v2_t), QURT_ELITE_HEAP_DEFAULT);
  if (0 == capi_v2_ptr) {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "capi_v2: failed to allocate list entry");
    THROW(err, ADSP_ENOMEMORY);
  }
  hashnode_initialized = TRUE;

  node_init(&capi_v2_ptr->node, type, id1, id2);

  capi_v2_ptr->node.vtbl = &capi_v2_node_vtbl;
  capi_v2_ptr->node.interface_type = CAPI_V2;
  capi_v2_ptr->get_static_properties_f = NULL;
  capi_v2_ptr->init_f = NULL;
  capi_v2_ptr->get_static_properties_str = NULL;
  capi_v2_ptr->init_str = NULL;
  is_static = (get_static_properties_f!=NULL);

  err = amdb_add_node(entry_type, &capi_v2_ptr->node, (void*)get_static_properties_f, (void*)init_f, preload,is_static,
                     filename_str, get_static_properties_str, init_str, dyn_loading_error_ptr, client_id);
  if (ADSP_EOK != err) {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "capi_v2: adsp_amdb_capi_v2_t node addition failed %d", err);
    THROW(err, err);
  }

  CATCH(err) {
    if (capi_v2_ptr) {
       if (hashnode_initialized) {
          (void)adsp_amdb_node_destroy(&capi_v2_ptr->node);
       } else {
          qurt_elite_memory_free(capi_v2_ptr);
       }
    }

    if (ADSP_FAILED(err))
    {
       if (NULL != dyn_loading_error_ptr)
       {
          *dyn_loading_error_ptr = err;
       }
    }
  }
  return err;
}

/*** stub ******************************************************************/
struct adsp_amdb_stub_t
{
   adsp_amdb_node_t node; // Must be the first element of the structure.
};


static ADSPResult stub_resolve_symbols(adsp_amdb_node_t *node_ptr, const char *f1_name_ptr = NULL, const char *f2_name_ptr = NULL) // If a function name is NULL, the internally stored names will be used
{

   return ADSP_EOK;
}

static ADSPResult stub_store_function_names(adsp_amdb_node_t *node_ptr, const char *f1_name_ptr, const char *f2_name_ptr)
{

   return ADSP_EOK;
}

static void stub_store_static_functions(adsp_amdb_node_t *node_ptr, void *f1_ptr, void *f2_ptr)
{

}

static void stub_clear_symbols(adsp_amdb_node_t *node_ptr)
{

}

static void stub_end(adsp_amdb_node_t *node_ptr)
{
   adsp_amdb_stub_t *me = reinterpret_cast<adsp_amdb_stub_t*>(node_ptr);

   me->node.vtbl->clear_symbols(&me->node);
}

static const amdb_node_vtbl_t stub_node_vtbl = {
      stub_resolve_symbols,
      stub_store_static_functions,
      stub_store_function_names,
      stub_clear_symbols,
      stub_end
};

static ADSPResult adsp_amdb_addInternal_stub(uint32_t entry_type, bool_t preload, int type, int id1, int id2,
                                             const char* filename_str, ADSPResult *dyn_loading_error_ptr, uint32_t client_id)
{
  adsp_amdb_stub_t* stub_ptr = NULL;
  ADSPResult err = ADSP_EOK;
  bool_t hashnode_initialized = FALSE;
  bool_t is_static = FALSE;

  MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AMDB_ADD: STUB: (%lu, 0x%x, 0x%lx) to entry-type 0x%x, preload %u", type,id1,id2,entry_type,preload);

  if (amdb_is_module_conflict(entry_type, type, id1, id2))
  {
     THROW(err, ADSP_EALREADY);
  }

  stub_ptr = (adsp_amdb_stub_t*)qurt_elite_memory_malloc(sizeof(adsp_amdb_stub_t), QURT_ELITE_HEAP_DEFAULT);
  if (0 == stub_ptr) {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "STUB: failed to allocate list entry");
    THROW(err, ADSP_ENOMEMORY);
  }
  hashnode_initialized = TRUE;

  node_init(&stub_ptr->node, type, id1, id2);

  stub_ptr->node.vtbl = &stub_node_vtbl;
  stub_ptr->node.interface_type = STUB;
  //stub_ptr->node.is_static = TRUE;
  is_static = TRUE;

  err = amdb_add_node(entry_type, &stub_ptr->node, NULL, NULL, preload,is_static, filename_str, NULL ,NULL, dyn_loading_error_ptr, client_id);
  if (ADSP_EOK != err) {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "STUB: adsp_amdb_stub_t node addition failed %d", err);
    THROW(err, err);
  }

  CATCH(err) {
    if (stub_ptr) {
       if (hashnode_initialized) {
          (void)adsp_amdb_node_destroy(&stub_ptr->node);
       } else {
          qurt_elite_memory_free(stub_ptr);
       }
    }
    if (ADSP_FAILED(err))
    {
       if (NULL != dyn_loading_error_ptr)
       {
          *dyn_loading_error_ptr = err;
       }
    }
  }
  return err;
}



/*** adsp_amdb - external api *************************************************/
extern "C" {

ADSPResult adsp_amdb_add_stub_with_entry_type(uint32_t entry_type, int type, int id1, int id2, ADSPResult *dyn_loading_error_ptr, uint32_t client_id )
{
   ADSPResult result=ADSP_EOK;

   result = adsp_amdb_addInternal_stub(entry_type, FALSE, type, id1, id2, NULL, dyn_loading_error_ptr, client_id);
   return result;
}

ADSPResult adsp_amdb_remove_stub_with_entry_type(uint32_t entry_type, int type, int id1, int id2)
{
  return adsp_amdb_remove_node_with_entry_type(entry_type, type, id1, id2);
}

ADSPResult adsp_amdb_remove_with_entry_type(uint32_t entry_type, int type, int id1, int id2)
{
   return adsp_amdb_remove_node_with_entry_type(entry_type, type, id1, id2);
}

ADSPResult adsp_amdb_add_capi_v2_with_entry_type(uint32_t entry_type, int type, int id1, int id2, bool_t preload,
                                 const char* filename_str,
                                 const char* tag,
                                 ADSPResult *dyn_loading_error_ptr,
                                 uint32_t client_id)
{
  ADSPResult result=ADSP_EOK;
  const char *stat_prop="_get_static_properties";
  const char *init = "_init";
  uint16_t len1=strlen(stat_prop);
  uint16_t len2=strlen(init);
  uint16_t len3=strlen(tag);

  char *ptr = (char*) qurt_elite_memory_malloc(len1+len2+(len3+1)*2, QURT_ELITE_HEAP_DEFAULT);

  if (ptr==NULL)
  {
	  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "amdb: out of memory when allocating memory for get static properties and init function strings");
	  return ADSP_ENOMEMORY;
  }

  char* get_static_properties_str = ptr;
  strlcpy(get_static_properties_str, tag, len3+1);
  strlcpy(get_static_properties_str+len3, stat_prop, len1+1);

  char* init_str = ptr+len1+len3+1;
  strlcpy(init_str, tag, len3+1);
  strlcpy(init_str+len3, init, len2+1);

  result = adsp_amdb_addInternal_capi_v2(entry_type, preload, type, id1, id2, 0, 0, filename_str,
                                       get_static_properties_str, init_str, dyn_loading_error_ptr, client_id);

  qurt_elite_memory_free(ptr);

  return result;
}

ADSPResult adsp_amdb_remove_capi_v2_with_entry_type(uint32_t entry_type, int type, int id1, int id2)
{
   return adsp_amdb_remove_node_with_entry_type(entry_type, type, id1, id2);
}

ADSPResult adsp_amdb_add_static_capi_v2(uint32_t entry_type, int type, int id1, int id2,
                                        capi_v2_get_static_properties_f get_static_properties_f,
                                        capi_v2_init_f init_f)
{
   return adsp_amdb_addInternal_capi_v2(entry_type, TRUE, type, id1, id2, get_static_properties_f, init_f, 0, 0, 0, NULL, 0);
}

ADSPResult adsp_amdb_add_capi_v2(int type, int id1, int id2, bool_t preload,
                                 const char* filename_str,
                                 const char* get_static_properties_str,
                                 const char* init_str)
{
  ADSPResult dyn_loading_error = ADSP_EOK;
  ADSPResult err = adsp_amdb_addInternal_capi_v2(ADSP_AMDB_ENTRY_TYPE_LEGACY, preload, type, id1, id2, 0, 0, filename_str,
                                       get_static_properties_str, init_str, &dyn_loading_error, 0);
  if (ADSP_FAILED(err))
  {
     return err;
  }

  adsp_amdb_wait_for_module_additions();

  return dyn_loading_error;
}

ADSPResult adsp_amdb_remove_capi_v2(int type, int id1, int id2, const char* filename_str)
{
   return adsp_amdb_remove_node(type, id1, id2, filename_str);
}

ADSPResult adsp_amdb_remove_all_capi_v2(void)
{
   return adsp_amdb_remove_all_nodes();
}

ADSPResult adsp_amdb_print_all_capi_v2(void)
{
   return adsp_amdb_print_all_nodes();
}

void adsp_amdb_get_modules_request(
      adsp_amdb_module_handle_info_t module_handle_info[],
      uint32_t num_modules,
      adsp_amdb_get_modules_callback_f callback_function,
      void *callback_context)
{
   amdb_loader_handle *preload_loader_handle_ptr = NULL;
   amdb_loader_handle *non_preload_loader_handle_ptr = NULL;
   adsp_amdb_default_callback_t default_cb_obj;
   adsp_amdb_t* me = adsp_amdb_ptr_;
   adsp_amdb_non_preload_task_t non_preload_task;
   adsp_amdb_preload_task_t preload_task;
   non_preload_task.data = 0;
   preload_task.data = 0;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AMDB_GET: get modules called (num_modules : %lu) ", num_modules);

   for (uint32_t i = 0; i < num_modules; i++)
   {
      bool_t to_be_loaded = FALSE;
      bool_t is_preload = FALSE;
      module_handle_info[i].result = adsp_amdb_get_node(&module_handle_info[i], &to_be_loaded);
      if (ADSP_SUCCEEDED(module_handle_info[i].result) && to_be_loaded)
      {
         is_preload = amdb_handle_to_node(module_handle_info[i].h)->preload;
         if(TRUE == is_preload)
         {
            // Preload node case, get preload handle once and reuse it if needed.
            // Do different get_handle  based on callback function validity
            if(NULL == preload_loader_handle_ptr)
            {
               if (NULL == callback_function)
               {
                  adsp_amdb_init_callback_mechanism(&default_cb_obj);
                  preload_loader_handle_ptr = amdb_loader_get_handle(me->loader_ptr,\
                                                                     adsp_amdb_default_callback, \
                                                                     &default_cb_obj, \
                                                                     amdb_preload_module_loader);
               }
               else
               {
                  preload_loader_handle_ptr = amdb_loader_get_handle(me->loader_ptr,\
                                                                     callback_function, \
                                                                     callback_context, \
                                                                     amdb_preload_module_loader);
               }

               if (NULL != preload_loader_handle_ptr)
               {
                  preload_task.data = 0;
                  preload_task.task_info.node_ptr = amdb_handle_to_node(module_handle_info[i].h);
                  preload_task.task_info.result_ptr = &(module_handle_info[i].result);
                  amdb_loader_push_task(preload_loader_handle_ptr, preload_task.data);
               }
               else
               {
                  MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "amdb: Couldn't push dlopen task for module %d, 0x%x, 0x%x, 0x%x",\
                        module_handle_info[i].interface_type,\
                        module_handle_info[i].type,\
                        module_handle_info[i].id1,\
                        module_handle_info[i].id2);
                  adsp_amdb_node_release_int(amdb_handle_to_node(module_handle_info[i].h), FALSE);
                  module_handle_info[i].h.capi_v2_handle = NULL;
                  module_handle_info[i].result = ADSP_EFAILED;
               }
            }
            else
            {
               preload_task.data = 0;
               preload_task.task_info.node_ptr = amdb_handle_to_node(module_handle_info[i].h);
               preload_task.task_info.result_ptr = &(module_handle_info[i].result);
               amdb_loader_push_task(preload_loader_handle_ptr, preload_task.data);
            }
         }
         else
         {
            // Non-Preload node case, get non-preload handle once and reuse it if needed.
            // Do different get_handle  based on callback function validity
            if(NULL == non_preload_loader_handle_ptr)
            {
               if (NULL == callback_function)
               {
                  adsp_amdb_init_callback_mechanism(&default_cb_obj);
                  non_preload_loader_handle_ptr = amdb_loader_get_handle(me->loader_ptr,\
                                                                         adsp_amdb_default_callback, \
                                                                         &default_cb_obj, \
                                                                         adsp_amdb_non_preload_module_loader);
               }
               else
               {
                  non_preload_loader_handle_ptr = amdb_loader_get_handle(me->loader_ptr,\
                                                                         callback_function, \
                                                                         callback_context, \
                                                                         adsp_amdb_non_preload_module_loader);
               }

               if (NULL != non_preload_loader_handle_ptr)
               {
                  non_preload_task.handle_info_ptr = &module_handle_info[i];
                  amdb_loader_push_task(non_preload_loader_handle_ptr, non_preload_task.data);
               }
               else
               {
                  MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "amdb: Couldn't push dlopen task for module %d, 0x%x, 0x%x, 0x%x",\
                        module_handle_info[i].interface_type,\
                        module_handle_info[i].type,\
                        module_handle_info[i].id1,\
                        module_handle_info[i].id2);
                  adsp_amdb_node_release_int(amdb_handle_to_node(module_handle_info[i].h), FALSE);
                  module_handle_info[i].h.capi_v2_handle = NULL;
                  module_handle_info[i].result = ADSP_EFAILED;
               }
            }
            else
            {
               non_preload_task.handle_info_ptr = &module_handle_info[i];
               amdb_loader_push_task(non_preload_loader_handle_ptr, non_preload_task.data);

            }
         }
      }
   }

   if ((NULL == non_preload_loader_handle_ptr) && \
       (NULL == preload_loader_handle_ptr))
   {
      if (NULL != callback_function)
      {
         callback_function(callback_context);
      }
   }
   else
   {
      if(NULL != non_preload_loader_handle_ptr)
      {
         amdb_loader_release_handle(non_preload_loader_handle_ptr);
      }

      if(NULL != preload_loader_handle_ptr)
      {
         amdb_loader_release_handle(preload_loader_handle_ptr);
      }

      if (NULL == callback_function)
      {
         adsp_amdb_init_callback_mechanism(&default_cb_obj);
         adsp_amdb_wait_deinit_callback_mechanism(&default_cb_obj);
      }
   }
}

void adsp_amdb_release_handles(
      adsp_amdb_module_handle_info_t module_handle_info[],
      uint32_t num_modules
      )
{
   for (uint32_t i = 0; i < num_modules; i++)
   {
      if (ADSP_FAILED(module_handle_info[i].result))
      {
         continue;
      }

      if (NULL != module_handle_info[i].h.capi_v2_handle)
      {
         adsp_amdb_node_release(amdb_handle_to_node(module_handle_info[i].h));
         module_handle_info[i].h.capi_v2_handle = NULL;
      }
   }
}

void adsp_amdb_get_dl_info(adsp_amdb_module_handle_info_t *module_handle_info, //handle to the amdb.
      bool_t *is_dl, //is module dynamically loaded.
      uint32_t **start_addr, //start address (virtual) where the lib is loaded.
      uint32_t *so_size) //size of the loaded library.
{
   *is_dl = FALSE;
   *start_addr = NULL;
   *so_size = 0;
   if (ADSP_FAILED(module_handle_info->result))
   {
      return;
   }
   adsp_amdb_node_t *node = amdb_handle_to_node(module_handle_info->h);

   if (NULL == node) return;

   if (node->is_static) return;

   *is_dl = TRUE;

   int p = 0xABCD;
   #ifndef MDSPMODE
	   int rc = dlinfo(node->h, RTLD_DI_LOAD_ADDR, &p);
   #else
	   int rc = 0;//dlinfo(node->h, RTLD_DI_LOAD_ADDR, &p);
   #endif
   
   if (rc) return;
   *start_addr = (uint32_t*)p;
   
   #ifndef MDSPMODE
      rc = dlinfo(node->h, RTLD_DI_LOAD_SIZE, &p);
   #endif

   if (rc)
   {
      *start_addr = NULL;
      return;
   }
   *so_size = p;
}


static void adsp_amdb_non_preload_module_loader(uint64_t task_info)
{
   adsp_amdb_non_preload_task_t task;
   task.data = task_info;
   adsp_amdb_module_handle_info_t *module_handle_info = task.handle_info_ptr;
   adsp_amdb_node_t *node_ptr = amdb_handle_to_node(module_handle_info->h);

   qurt_elite_mutex_lock(&node_ptr->dl_mutex);
   if( 0 == node_ptr->dl_refs)
   {

      ADSPResult res = node_wrapper_dlopen(node_ptr, node_ptr->filename_str);
      if (ADSP_EOK != res)
      {
         MSG_SPRINTF_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "adsp amdb: Encountered error while loading dynamic module %s",
               node_ptr->filename_str);
         qurt_elite_mutex_unlock(&node_ptr->dl_mutex);
         adsp_amdb_node_release_int(node_ptr, FALSE);
         module_handle_info->h.capi_v2_handle = NULL;
         module_handle_info->result = res;
      }
      else
      {
         node_ptr->dl_refs++;
         qurt_elite_mutex_unlock(&node_ptr->dl_mutex);
      }
   }
   else
   {
      node_ptr->dl_refs++;
      qurt_elite_mutex_unlock(&node_ptr->dl_mutex);
   }
}

ADSPResult adsp_amdb_wait_for_module_additions(uint32_t client_id)
{
   adsp_amdb_t* amdb_ptr = adsp_amdb_ptr_;
   amdb_client_t *client_ptr = NULL;
   ADSPResult aggregate_error = ADSP_EOK;

   qurt_elite_mutex_lock(&amdb_ptr->client_objects_mutex);
   hash_node *phn = hashtable_find(&amdb_ptr->client_objects_ht, &client_id, sizeof(client_id));
   if (NULL != phn)
   {
      client_ptr = STD_RECOVER_REC(amdb_client_t, hn, phn);
      amdb_client_addref(client_ptr);
   }
   qurt_elite_mutex_unlock(&amdb_ptr->client_objects_mutex);

   if (NULL != client_ptr)
   {
      amdb_client_wait_for_done(client_ptr);

      aggregate_error = amdb_client_get_aggregate_error(client_ptr);

      qurt_elite_mutex_lock(&amdb_ptr->client_objects_mutex);
      (void) hashtable_remove(&amdb_ptr->client_objects_ht, &client_id, sizeof(client_id), &client_ptr->hn);
      qurt_elite_mutex_unlock(&amdb_ptr->client_objects_mutex);

      // Since client code is single thread for a particular id, the only ref left should be the current one.
      QURT_ELITE_ASSERT(1 == qurt_elite_atomic_get(&client_ptr->refs));

      // This will wait for all loading to complete and destroy the client object.
      amdb_client_release(client_ptr);
      client_ptr = NULL;

   }

   return aggregate_error;
}

void adsp_amdb_disable_entry_type(uint32_t entry_type)
{
   adsp_amdb_t* me = adsp_amdb_ptr_;
   qurt_elite_mutex_lock(&me->mutex_node);
   for (uint32_t i = 0; i < ADSP_AMDB_NUM_ENTRY_TYPES; i++)
   {
      if (entry_type == ADSP_AMDB_ENTRY_CONFIG[i].entry_type)
      {
         me->is_ht_enabled[i] = FALSE;
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "amdb: Disabling entry type %lx", entry_type);
         break;
      }
   }
   qurt_elite_mutex_unlock(&me->mutex_node);
}

void adsp_amdb_enable_entry_type(uint32_t entry_type)
{
   adsp_amdb_t* me = adsp_amdb_ptr_;
   qurt_elite_mutex_lock(&me->mutex_node);
   for (uint32_t i = 0; i < ADSP_AMDB_NUM_ENTRY_TYPES; i++)
   {
      if (entry_type == ADSP_AMDB_ENTRY_CONFIG[i].entry_type)
      {
         me->is_ht_enabled[i] = TRUE;
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "amdb: Enabling entry type %lx", entry_type);
         break;
      }
   }
   qurt_elite_mutex_unlock(&me->mutex_node);
}

}
// End of external API
