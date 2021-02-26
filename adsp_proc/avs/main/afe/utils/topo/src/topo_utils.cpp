/** @file topo_utils.cpp

    @brief This file contains Topo Utils implmentation

 */

/*========================================================================
 $Header: //components/rel/avs.adsp/2.8.5/afe/utils/topo/src/topo_utils.cpp#1 $

 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 04/09/2014 mangeshk   Initial version
 ==========================================================================*/


/*-----------------------------------------------------------------------
   Copyright (c) 2012-2014 Qualcomm  Technologies, Inc.  All rights reserved.
   Qualcomm Technologies Proprietary and Confidential.
 -----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "topo_utils.h"
#include "audpp_util.h"
#include "Elite_CAPI_V2_types.h"
#include "adsp_amdb_static.h"
#include "Elite.h"
#include "EliteLoggingUtils.h"
#include "Elite_intf_extns_change_media_fmt.h"
#include "capi_v2_adsp_error_code_converter.h"

#define SIZE_OF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))

#define INPUT_BUF_INDEX 		0
#define OUTPUT_BUF_INDEX 		1
#define BUF_1_INDEX 			2
#define BUF_2_INDEX 			3
#define TOTAL_BUFFERS 			4
#define NUM_TEMP_BUFS 			2

#define TOPO_STACK_SIZE 	4096

typedef enum topo_intf_extn_idx_t
{
   INTF_EXTN_CHANGES_MEDIA_FORMAT = 0,
   INTF_EXTN_MAX
} topo_intf_extn_idx_t;

typedef struct topo_module_intf_extn_list_t
{
   capi_v2_interface_extns_list_t intf_extn_num;
   capi_v2_interface_extn_desc_t intf_extn_desc[INTF_EXTN_MAX];
} topo_module_intf_extn_list_t;

static QURT_ELITE_HEAP_ID topo_get_heap_id(bool_t use_lpm)
{
  QURT_ELITE_HEAP_ID heap_id = QURT_ELITE_HEAP_DEFAULT;
  if(use_lpm)
  {
    heap_id = AFE_LSM_LOW_POWER_MODULE;
  }
  return heap_id;
}

static void topo_module_query_intf_extn(topo_node_t *module_info_ptr, adsp_amdb_capi_v2_t *handle);
static bool_t topo_is_equal_media_format(const topo_media_fmt_t *f1, const topo_media_fmt_t *f2);
static void buf_convert_to_capi_v2_buf(int8_t *in_buf_ptr,
                                       capi_v2_stream_data_t *buflist_ptr,
                                       capi_v2_buf_t *buf_ptr,
                                       const uint32_t num_channels,
                                       const uint32_t buf_size_per_channel)
{
  uint32_t num_bufs;

  num_bufs = num_channels;

  int8_t *pData = in_buf_ptr;

  buflist_ptr->buf_ptr = buf_ptr;
  buflist_ptr->bufs_num = num_bufs;
  buflist_ptr->timestamp = 0; // will not be used in topo_utils

  memset(&buflist_ptr->flags, 0, sizeof(capi_v2_stream_flags_t));

  for(uint32_t i = 0; i < num_bufs; i++)
  {
    buf_ptr[i].data_ptr = pData;
    buf_ptr[i].actual_data_len = buf_size_per_channel;
    buf_ptr[i].max_data_len = buf_size_per_channel;

    pData += buf_size_per_channel;
  }
}

static ADSPResult topo_create_capi_v2_module(topo_t *topo_ptr, const uint32_t mod_idx, const uint32_t module_id,
                                             capi_v2_proplist_t *init_proplist_ptr, QURT_ELITE_HEAP_ID heap_id,
                                             adsp_amdb_capi_v2_t *handle_ptr)
{
  ADSPResult result = ADSP_EOK;
  void *module_mem = NULL;
  topo_node_t *topo_node_ptr = NULL;
  capi_v2_err_t err_code = CAPI_V2_EOK;

  if(NULL == handle_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "handle_ptr is null");
    return ADSP_EBADPARAM;
  }

  topo_node_ptr = &topo_ptr->module[mod_idx];

  // Query static properties
  capi_v2_init_memory_requirement_t memory_req = { 0 };
  capi_v2_is_inplace_t is_inplace = { FALSE };
  capi_v2_requires_data_buffering_t req_data_buf = { FALSE };
  capi_v2_num_needed_framework_extensions_t num_extensions = { 0 };

  // Is_logging_enabled need to be added here
  capi_v2_stack_size_t stack_size = { 0 };
  capi_v2_prop_t static_props[] =
  {

    { CAPI_V2_INIT_MEMORY_REQUIREMENT,
      { reinterpret_cast<int8_t *>(&memory_req), 0, sizeof(memory_req) },
      { FALSE, FALSE, 0 } },

    { CAPI_V2_IS_INPLACE,
      { reinterpret_cast<int8_t *>(&is_inplace), 0, sizeof(is_inplace) },
      { FALSE, FALSE, 0 } },

    { CAPI_V2_REQUIRES_DATA_BUFFERING,
      { reinterpret_cast<int8_t *>(&req_data_buf), 0, sizeof(req_data_buf) },
      { FALSE, FALSE, 0 } },

    { CAPI_V2_NUM_NEEDED_FRAMEWORK_EXTENSIONS,
      { reinterpret_cast<int8_t *>(&num_extensions), 0, sizeof(num_extensions) },
      { FALSE, FALSE, 0 } },

    { CAPI_V2_STACK_SIZE,
      { reinterpret_cast<int8_t *>(&stack_size), 0, sizeof(stack_size) },
      { FALSE, FALSE, 0 } },

  };
  capi_v2_proplist_t static_proplist = { SIZE_OF_ARRAY(static_props), static_props };

  err_code = adsp_amdb_capi_v2_get_static_properties_f(handle_ptr, init_proplist_ptr, &static_proplist);
  //ignoring CAPI_V2_EUNSUPPORTED returned from Module during get static property
  if(!((CAPI_V2_EOK == err_code) || (CAPI_V2_EUNSUPPORTED == err_code)))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Module with ID 0x%lX failed in getting static properties with %lu.", module_id, err_code);
    return ADSP_EFAILED;
  }

  //this conditon can occur when stub is executed
  if(memory_req.size_in_bytes <= 0)
  {
    return ADSP_EUNSUPPORTED;
  }

  topo_node_ptr->requires_data_buffering = req_data_buf.requires_data_buffering;
  if(topo_node_ptr->requires_data_buffering)
  {
    capi_v2_port_data_threshold_t input, output;

    // Set up the thresholds
    capi_v2_prop_t threshold_props[] = {
      { CAPI_V2_PORT_DATA_THRESHOLD,  { reinterpret_cast<int8_t*>(&input),   0, sizeof(input)   }, { TRUE, TRUE, 0} },
      { CAPI_V2_PORT_DATA_THRESHOLD,  { reinterpret_cast<int8_t*>(&output),   0, sizeof(output)   }, { TRUE, FALSE, 0} },
    };
    capi_v2_proplist_t threshold_proplist = {
        SIZE_OF_ARRAY(threshold_props),
        threshold_props
    };

    capi_v2_err_t err_code = adsp_amdb_capi_v2_get_static_properties_f(handle_ptr, init_proplist_ptr, &threshold_proplist);

    if (CAPI_V2_FAILED(err_code))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Module 0x%lX returned error when getting thresholds.",topo_node_ptr->module_id);
      return ADSP_EUNSUPPORTED;
    }

    if (input.threshold_in_bytes != 1)
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Module 0x%lX returned input threshold %lu. Only a threshold of 1 is supported.", topo_node_ptr->module_id, input.threshold_in_bytes);
      return ADSP_EUNSUPPORTED;
    }

    if (output.threshold_in_bytes != 1)
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Module 0x%lX returned output threshold %lu. Only a threshold of 1 is supported.", topo_node_ptr->module_id, output.threshold_in_bytes);
      return ADSP_EUNSUPPORTED;
    }
  }
  if(topo_node_ptr->requires_data_buffering)
  {
    topo_node_ptr->is_in_place = FALSE;
  }
  else
  {
    topo_node_ptr->is_in_place = is_inplace.is_inplace;
  }

  if(FALSE == topo_node_ptr->is_in_place)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Module 0x%lX is added as NON IN PLACE", topo_node_ptr->module_id);
  }
  if(topo_ptr->stack_size < stack_size.size_in_bytes)
  {
    topo_ptr->stack_size = stack_size.size_in_bytes;
  }

  if(0 != num_extensions.num_extensions)
  {
    if(NULL == (topo_node_ptr->fwk_extn_ptr = (capi_v2_framework_extension_id_t *)
                qurt_elite_memory_malloc(sizeof(capi_v2_framework_extension_id_t) * 
                                         num_extensions.num_extensions, QURT_ELITE_HEAP_DEFAULT)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory for fwk extensions for Module with ID 0x%lX", module_id);
      return ADSP_ENOMEMORY;
    }

    capi_v2_prop_t static_props_extn[] = {
      { CAPI_V2_NEEDED_FRAMEWORK_EXTENSIONS,
        { reinterpret_cast<int8_t *>(topo_node_ptr->fwk_extn_ptr), 0,
          sizeof(capi_v2_framework_extension_id_t) * num_extensions.num_extensions },
        { FALSE, FALSE, 0 } },

      { CAPI_V2_INIT_MEMORY_REQUIREMENT,
        { reinterpret_cast<int8_t *>(&memory_req), 0, sizeof(memory_req) },
        { FALSE, FALSE, 0 } },
    };

    capi_v2_proplist_t static_proplist_extn = { SIZE_OF_ARRAY(static_props_extn), static_props_extn };

    err_code = adsp_amdb_capi_v2_get_static_properties_f(handle_ptr, init_proplist_ptr, &static_proplist_extn);
    if(CAPI_V2_FAILED(err_code))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Module with ID 0x%lX failed in getting fwk extn properties with %lu.", module_id, err_code);
      return ADSP_EFAILED;
    }

  }

  topo_node_ptr->num_extensions.num_extensions = num_extensions.num_extensions;


  // Allocate memory
  if(NULL == (module_mem = (capi_v2_t *)qurt_elite_memory_malloc(memory_req.size_in_bytes, heap_id)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory creating capi_v2 module for Module with ID 0x%lX", module_id);
    return ADSP_ENOMEMORY;
  }
  else
  {
    memset(module_mem, 0, memory_req.size_in_bytes);
  }

  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "topo_utils: Module with ID 0x%lX allocated for %lu bytes of memory at location 0x%p", module_id, memory_req.size_in_bytes, module_mem);

  topo_node_ptr->module_ptr = (capi_v2_t *)(module_mem);

  // Initialize
  err_code = adsp_amdb_capi_v2_init_f(handle_ptr, topo_node_ptr->module_ptr, init_proplist_ptr);
  if(CAPI_V2_FAILED(err_code))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Module with ID 0x%lX failed in init with %lu.", module_id, err_code);
    qurt_elite_memory_free(topo_node_ptr->module_ptr);
    topo_node_ptr->module_ptr = NULL;
    return ADSP_EFAILED;
  }

  // query interface extensions supported by the module
  topo_module_query_intf_extn(topo_node_ptr, handle_ptr);

  return result; 
}


/*
  This function return module index from Event Call Back Context

  @param[in]     cb_handle: Handle returned during Event call back

  @return
  Module index

  @dependencies
  None.
*/
uint32_t topo_event_get_module_index(void *handle)
{
	return reinterpret_cast<topo_module_event_handle_t*>(handle)->module_index;
}

/*
  This function return Topo handle from Event Call Back Context

  @param[in]     cb_handle: Handle returned during Event call back

  @return
  Handle returned during topology creation (in unable format)

  @dependencies
  None.
*/
topo_t* topo_event_get_struct_ptr(void *handle)
{
	return reinterpret_cast<topo_module_event_handle_t*>(handle)->topo_ptr;
}


/*
  This is the function for initing the modules

  @param[in]     topology_ptr: pointer to the topology constructed
  @param[in]     mod_idx: index of the module in the topology
  @param[in/out] module_info_ptr: pointer to module info structure

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
static ADSPResult topo_init_module(topo_t *topo_ptr, const uint32_t mod_idx,
                                   capi_v2_event_cb_f event_cb_f, bool_t use_lpm,
                                   adsp_amdb_module_handle_info_t *handle_ptr)
{
  ADSPResult result = ADSP_EOK;
  topo_node_t *topo_node_ptr = NULL;

  if(NULL == handle_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "handle_ptr is null");
    return ADSP_EBADPARAM;
  }

  topo_node_ptr = &topo_ptr->module[mod_idx];

  // Set up default values
  topo_node_ptr->is_in_place = TRUE; 
  topo_node_ptr->is_enabled = TRUE;
  topo_node_ptr->module_ptr = NULL;
  topo_node_ptr->heap_id = topo_get_heap_id(use_lpm);

  topo_node_ptr->event_handle.module_index = mod_idx;
  topo_node_ptr->event_handle.topo_ptr = topo_ptr;
 
  // Set up init properties
  capi_v2_port_num_info_t port_info = { 1, 1 };
  capi_v2_heap_id_t heap_id = { topo_node_ptr->heap_id };
  capi_v2_event_callback_info_t cb_info = { event_cb_f, &topo_node_ptr->event_handle };
  capi_v2_log_code_t log_code = { QXDM_LOG_CODE_AFE_ALGO_CALIB }; 

	capi_v2_prop_t init_props[] = {
   { CAPI_V2_PORT_NUM_INFO,
   { reinterpret_cast<int8_t *>(&port_info), sizeof(port_info), sizeof(port_info) },
   { FALSE, FALSE, 0 } },

   { CAPI_V2_HEAP_ID,
   { reinterpret_cast<int8_t *>(&heap_id), sizeof(heap_id), sizeof(heap_id) },
   { FALSE, FALSE, 0 } },

   { CAPI_V2_EVENT_CALLBACK_INFO,
   { reinterpret_cast<int8_t *>(&cb_info), sizeof(cb_info), sizeof(cb_info) },
   { FALSE, FALSE, 0 } },

   { CAPI_V2_LOG_CODE,
   { reinterpret_cast<int8_t *>(&log_code), sizeof(log_code), sizeof(log_code) },
   { FALSE, FALSE, 0 } },
	};

  capi_v2_proplist_t init_proplist = { SIZE_OF_ARRAY(init_props), init_props };

       result = topo_create_capi_v2_module(topo_ptr,
                                           mod_idx,
                                      topo_node_ptr->module_id,
                                           &init_proplist,
                                      topo_node_ptr->heap_id, handle_ptr->h.capi_v2_handle);
  if(ADSP_FAILED(result))
  {
    return result;
  }
	return ADSP_EOK;
}

/*
  This function constructs the topology for the given common db
  topology definition

  @param[out]    handle: Returned for the topology constructed
  @param[in/out] topo_def_ptr: pointer to topo definition structure
  @param[in]     event_cb_f: CAPI_V2 Event callback function
  @param[in]     session_ptr: Session pointer of the client
  @param[in]     cb_info_ptr: Pointer to callback function

  @return
  Indication of success or failure.

  @dependencies
  None.
 */
ADSPResult topo_create_common_db(topo_handle_t *handle,
                                 elite_cmn_topo_db_entry_t *topology_ptr,
                                 capi_v2_event_cb_f event_cb_f,
                                 void *session_ptr,
                                 topo_callback_info_t *cb_info_ptr)
{
  ADSPResult result = ADSP_EOK;
  uint32_t mod_idx = 0;
  topo_t *topo_ptr = NULL;
  topo_node_t *topo_node_ptr = NULL;
  uint32_t num_modules;
  topo_t **topo_ptr_ptr = (topo_t **)handle;
  adsp_amdb_module_handle_info_t *module_handle_info_ptr = NULL,*module_handle_info_ptr_tmp = NULL;

  if(NULL == topo_ptr_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo_def_ptr is null");
    return ADSP_EBADPARAM;
  }

  if(NULL != *topo_ptr_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Graph obj is non null");
    return ADSP_EALREADY;
  }

  num_modules = topology_ptr->topo_def.num_modules;

  avcs_module_instance_info_t  *module_list_ptr = NULL;
  uint32_t actual_num_modules = 0;

  //Find all modules
  {
    module_handle_info_ptr = (adsp_amdb_module_handle_info_t *)
    qurt_elite_memory_malloc(sizeof(adsp_amdb_module_handle_info_t) * num_modules, QURT_ELITE_HEAP_DEFAULT);

    if(NULL == module_handle_info_ptr)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo_utils: Could not allocate adsp_amdb_module_handle_info_t "
                                            "for %lu modules", num_modules);
      return ADSP_ENOMEMORY;
    }

    for(mod_idx = 0; mod_idx < num_modules; mod_idx++)
    {
      module_handle_info_ptr[mod_idx].interface_type = CAPI_V2;
      module_handle_info_ptr[mod_idx].type = AMDB_MODULE_TYPE_GENERIC;
      module_handle_info_ptr[mod_idx].id1 = topology_ptr->module_info[mod_idx].module_id;
      module_handle_info_ptr[mod_idx].id2 = 0;
      module_handle_info_ptr[mod_idx].h.capi_v2_handle = NULL;
      module_handle_info_ptr[mod_idx].result = ADSP_EFAILED;
    }

    /*
     * Note: This call will block till all modules with 'preload = 0' are loaded by the AMDB. This loading
     * happens using a thread pool using threads of very low priority. This can cause the current thread
     * to be blocked because of a low priority thread. If this is not desired, a callback function
     * should be provided that can be used by the AMDB to signal when the modules are loaded. The current
     * thread can then handle other tasks in parallel.
     */
    adsp_amdb_get_modules_request(module_handle_info_ptr, num_modules, NULL, NULL);

    module_list_ptr = (avcs_module_instance_info_t *)qurt_elite_memory_malloc(num_modules * sizeof(avcs_module_instance_info_t), QURT_ELITE_HEAP_DEFAULT);
    if(NULL == module_list_ptr)
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo_utils: Could not allocate module_list_ptr");
      qurt_elite_memory_free(module_handle_info_ptr);
      return ADSP_ENOMEMORY;
    }
    actual_num_modules = 0;
    for(mod_idx = 0; mod_idx < num_modules; mod_idx++)
    {
      if(ADSP_SUCCEEDED(module_handle_info_ptr[mod_idx].result) && 
         (CAPI_V2 == module_handle_info_ptr[mod_idx].interface_type))
      {
        module_list_ptr[actual_num_modules].module_id = topology_ptr->module_info[mod_idx].module_id;
        module_list_ptr[actual_num_modules].instance_id = topology_ptr->module_info[mod_idx].instance_id;
        actual_num_modules++;
      }
      else if(ADSP_SUCCEEDED(module_handle_info_ptr[mod_idx].result) && 
              (STUB == module_handle_info_ptr[mod_idx].interface_type))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo_utils: Module ID 0x%lX is stubbed and hence removed from topology.", 
              topology_ptr->module_info[mod_idx].module_id);
      }
      else
      {
        if(!ADSP_SUCCEEDED(module_handle_info_ptr[mod_idx].result))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo_utils: Module ID 0x%lX is invalid.", 
                topology_ptr->module_info[mod_idx].module_id);
        }
        else
        {
          MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo_utils: Module ID 0x%lX has interface type %d when %d expected.", 
                topology_ptr->module_info[mod_idx].module_id, 
                (int)module_handle_info_ptr[mod_idx].interface_type, (int)CAPI_V2);
        }
        // Release the rest of the handles
        adsp_amdb_release_handles(&module_handle_info_ptr[mod_idx], num_modules - mod_idx);
        qurt_elite_memory_free(module_handle_info_ptr);
        qurt_elite_memory_free(module_list_ptr);
        return ADSP_EUNSUPPORTED;
      }
    }

    uint32_t topo_size = sizeof(topo_t) + (actual_num_modules * sizeof(topo_node_t));

    if(NULL == (topo_ptr = (topo_t *)qurt_elite_memory_malloc(topo_size, QURT_ELITE_HEAP_DEFAULT)))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory creating topo object..");
      result = ADSP_ENOMEMORY;
      goto _topo_create_common_db_clean_up;
    }
    else
    {
      memset(topo_ptr, 0, topo_size);
    }

    topo_ptr->module = (topo_node_t *)((uint8_t *)topo_ptr + sizeof(topo_t));

    *topo_ptr_ptr = topo_ptr;

    topo_ptr->stack_size = TOPO_STACK_SIZE;
    topo_ptr->session_ptr = session_ptr;
    topo_ptr->num_modules = actual_num_modules;

    module_handle_info_ptr_tmp = module_handle_info_ptr;

    /* Set up the modules */
    for(mod_idx = 0; mod_idx < topo_ptr->num_modules; mod_idx++)
    {
      topo_node_ptr = &topo_ptr->module[mod_idx];
      topo_node_ptr->module_id = module_list_ptr[mod_idx].module_id;
      topo_node_ptr->instance_id = module_list_ptr[mod_idx].instance_id;

      while(module_handle_info_ptr_tmp->interface_type == STUB)
      {
        module_handle_info_ptr_tmp++;
      }

      if(((int32_t)(topo_node_ptr->module_id)) != module_handle_info_ptr_tmp->id1)
      {
        MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
              "topo_utils: module id in topo node 0x%lx and module handle 0x%lx is not matching",
              topo_node_ptr->module_id, module_handle_info_ptr_tmp->id1);
        result = ADSP_EFAILED;
        goto _topo_create_common_db_clean_up;
      }

      /* PRE call back */
      if(NULL != cb_info_ptr)
      {
        result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_PRE);
        if(ADSP_FAILED(result))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
                "topo_utils: Module ID 0x%lX is failed PRE callback type",
                topo_node_ptr->module_id);
          goto _topo_create_common_db_clean_up;
        }
      }

      result = topo_init_module(topo_ptr, mod_idx, event_cb_f, FALSE, module_handle_info_ptr_tmp);
      if(ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_utils: Module with ID 0x%lX failed in init.", 
              topology_ptr->module_info[mod_idx].module_id);
        goto _topo_create_common_db_clean_up;
      }

      /* POST call back */
      if(NULL != cb_info_ptr)
      {
        result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_POST);
        if(ADSP_FAILED(result))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
                "topo_utils: Module ID 0x%lX is failed PRE callback type",
                topo_node_ptr->module_id);
          goto _topo_create_common_db_clean_up;
        }
      }
      module_handle_info_ptr_tmp++;
    }

_topo_create_common_db_clean_up:
    adsp_amdb_release_handles(module_handle_info_ptr, topology_ptr->topo_def.num_modules);
    qurt_elite_memory_free(module_handle_info_ptr);
    qurt_elite_memory_free(module_list_ptr);
  }
  return result;
}

/*
  This function constructs the topology for the given topology definition

  @param[out]    handle: Returned for the topology constructed
  @param[in/out] topo_def_ptr: pointer to topo definition structure
  @param[in]     event_cb_f: CAPI_V2 Event callback function
  @param[in]     session_ptr: Session pointer of the client
  @param[in]     cb_info_ptr: Pointer to callback function

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult topo_create(topo_handle_t *handle,
                       topology_definition_t *topo_def_ptr,
                       capi_v2_event_cb_f event_cb_f,
                       void *session_ptr,
                       topo_callback_info_t *cb_info_ptr)
{ 
  ADSPResult result = ADSP_EOK;
  uint32_t mod_idx = 0;
  topo_t *topo_ptr = NULL;
  topo_node_t *topo_node_ptr = NULL;
  uint32_t num_modules;
  topo_t **topo_ptr_ptr = (topo_t **)handle;
  adsp_amdb_module_handle_info_t *module_handle_info_ptr = NULL,*module_handle_info_ptr_tmp = NULL;

  if(NULL == topo_ptr_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo_def_ptr is null");
    return ADSP_EBADPARAM;
  }

  if(NULL != *topo_ptr_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Graph obj is non null");
    return ADSP_EALREADY;
  }

  if(topo_def_ptr->num_modules > AFE_MAX_MODULES_PER_TOPOLOGY)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Graph definition has more number of modules %ld than supported "
                                          "modules in topo", topo_def_ptr->num_modules);
    return ADSP_EBADPARAM;
  }

  num_modules = topo_def_ptr->num_modules;

  avcs_module_instance_info_t *module_list_ptr = NULL;
  uint32_t actual_num_modules = 0;

  //Find all modules
  {
    module_handle_info_ptr = (adsp_amdb_module_handle_info_t *)
    qurt_elite_memory_malloc(sizeof(adsp_amdb_module_handle_info_t) * num_modules, QURT_ELITE_HEAP_DEFAULT);
    if(NULL == module_handle_info_ptr)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo_utils: Could not allocate adsp_amdb_module_handle_info_t "
                                            "for %lu modules", num_modules);
      return ADSP_ENOMEMORY;
    }

    for(mod_idx = 0; mod_idx < num_modules; mod_idx++)
    {
      module_handle_info_ptr[mod_idx].interface_type = CAPI_V2;
      module_handle_info_ptr[mod_idx].type = AMDB_MODULE_TYPE_GENERIC;
      module_handle_info_ptr[mod_idx].id1 = topo_def_ptr->module_info[mod_idx].module_id;
      module_handle_info_ptr[mod_idx].id2 = 0;
      module_handle_info_ptr[mod_idx].h.capi_v2_handle = NULL;
      module_handle_info_ptr[mod_idx].result = ADSP_EFAILED;
    }

    /*
     * Note: This call will block till all modules with 'preload = 0' are loaded by the AMDB. This loading
     * happens using a thread pool using threads of very low priority. This can cause the current thread
     * to be blocked because of a low priority thread. If this is not desired, a callback function
     * should be provided that can be used by the AMDB to signal when the modules are loaded. The current
     * thread can then handle other tasks in parallel.
     */
    adsp_amdb_get_modules_request(module_handle_info_ptr, num_modules, NULL, NULL);

    module_list_ptr = (avcs_module_instance_info_t *)qurt_elite_memory_malloc(num_modules * sizeof(avcs_module_instance_info_t), QURT_ELITE_HEAP_DEFAULT);
    if(NULL == module_list_ptr)
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo_utils: Could not allocate module_list_ptr");
      qurt_elite_memory_free(module_handle_info_ptr);
      return ADSP_ENOMEMORY;
    }

    actual_num_modules = 0;
    for(mod_idx = 0; mod_idx < num_modules; mod_idx++)
    {
      if(ADSP_SUCCEEDED(module_handle_info_ptr[mod_idx].result) && 
         (CAPI_V2 == module_handle_info_ptr[mod_idx].interface_type))
      {
        module_list_ptr[actual_num_modules].module_id = topo_def_ptr->module_info[mod_idx].module_id;
        module_list_ptr[actual_num_modules].instance_id = 0;
        actual_num_modules++;
      }
      else if(ADSP_SUCCEEDED(module_handle_info_ptr[mod_idx].result) && 
              (STUB == module_handle_info_ptr[mod_idx].interface_type))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo_utils: Module ID 0x%lX is stubbed and hence removed "
                                              "from topology.", topo_def_ptr->module_info[mod_idx].module_id);
      }
      else
      {
        if(!ADSP_SUCCEEDED(module_handle_info_ptr[mod_idx].result))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo_utils: Module ID 0x%lX is invalid.", 
                topo_def_ptr->module_info[mod_idx].module_id);
        }
        else
        {
          MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo_utils: Module ID 0x%lX has interface type %d when %d expected.", 
                topo_def_ptr->module_info[mod_idx].module_id, (int)module_handle_info_ptr[mod_idx].interface_type, (int)CAPI_V2);
        }
        // Release the rest of the handles
        adsp_amdb_release_handles(&module_handle_info_ptr[mod_idx], num_modules - mod_idx);
        qurt_elite_memory_free(module_handle_info_ptr);
        qurt_elite_memory_free(module_list_ptr);
        return ADSP_EUNSUPPORTED;
      }
    }

    uint32_t topo_size = sizeof(topo_t) + (actual_num_modules * sizeof(topo_node_t));

    if(NULL == (topo_ptr = (topo_t *)qurt_elite_memory_malloc(topo_size, QURT_ELITE_HEAP_DEFAULT)))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory creating topo object..");
      result = ADSP_ENOMEMORY;
      goto _topo_create_clean_up;
    }
    else
    {
      memset(topo_ptr, 0, topo_size);
    }

    topo_ptr->module = (topo_node_t *)((uint8_t *)topo_ptr + sizeof(topo_t));
    *topo_ptr_ptr = topo_ptr;

    topo_ptr->stack_size = TOPO_STACK_SIZE;
    topo_ptr->session_ptr = session_ptr;
    topo_ptr->num_modules = actual_num_modules;

    module_handle_info_ptr_tmp = module_handle_info_ptr;

    /* Set up the modules */
    for(mod_idx = 0; mod_idx < topo_ptr->num_modules; mod_idx++)
    {
      topo_node_ptr = &topo_ptr->module[mod_idx];
      topo_node_ptr->module_id = module_list_ptr[mod_idx].module_id;
      topo_node_ptr->instance_id = module_list_ptr[mod_idx].instance_id;

      while(module_handle_info_ptr_tmp->interface_type == STUB)
      {
        module_handle_info_ptr_tmp++;
      }

      if(((int32_t)(topo_node_ptr->module_id)) != module_handle_info_ptr_tmp->id1)
      {
        MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
              "topo_utils: module id in topo node 0x%lx and module handle 0x%lx is not matching",
              topo_node_ptr->module_id, module_handle_info_ptr_tmp->id1);
        result = ADSP_EFAILED;
        goto _topo_create_clean_up;
      }

      /* PRE call back */
      if(NULL != cb_info_ptr)
      {
        result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_PRE);
        if(ADSP_FAILED(result))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
                "topo_utils: Module ID 0x%lX is failed PRE callback type",
                topo_node_ptr->module_id);
          goto _topo_create_clean_up;
        }
      }

      result = topo_init_module(topo_ptr, mod_idx, event_cb_f, FALSE, module_handle_info_ptr_tmp);
      if(ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_utils: Module with ID 0x%lX failed in init.", 
              topo_def_ptr->module_info[mod_idx].module_id);
        goto _topo_create_clean_up;
      }

      /* POST call back */
      if(NULL != cb_info_ptr)
      {
        result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_POST);
        if(ADSP_FAILED(result))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
                "topo_utils: Module ID 0x%lX is failed PRE callback type",
                topo_node_ptr->module_id);
          goto _topo_create_clean_up;
        }
      }
      module_handle_info_ptr_tmp++;
    }

_topo_create_clean_up:
    adsp_amdb_release_handles(module_handle_info_ptr, topo_def_ptr->num_modules);
    qurt_elite_memory_free(module_handle_info_ptr);
    qurt_elite_memory_free(module_list_ptr);
  }

  return result;
}

/*
  This function destroys the topology 

  @param[in/out]  handle: Handle returned during topology creation
  @param[in]     cb_info_ptr: Pointer to callback function

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult topo_destroy(topo_handle_t handle,
                        topo_callback_info_t *cb_info_ptr)
{
  uint32_t mod_idx = 0;
  topo_node_t *topo_node_ptr = NULL;
  topo_t *topo_ptr = (topo_t *)handle;
  ADSPResult result = ADSP_EOK;

  if(NULL != topo_ptr)
  {
    if(TOPO_STATE_STOP != topo_ptr->topo_state)
    {
      topo_stop((void *)topo_ptr, NULL);
    }

    for(mod_idx = 0; mod_idx < topo_ptr->num_modules; mod_idx++)
    {
      topo_node_ptr = &topo_ptr->module[mod_idx];

      if(NULL != topo_node_ptr->module_ptr)
      {
        /* PRE call back */
        if(NULL != cb_info_ptr)
        {
          result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_PRE);
          if(ADSP_FAILED(result))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
                  "topo_utils: Module ID 0x%lX is failed PRE callback type",
                  topo_node_ptr->module_id);
            /* Continue on error */
          }
        }
        if(NULL != topo_node_ptr->module_ptr->vtbl_ptr)
        {
          // Deiniting the algo module
          topo_node_ptr->module_ptr->vtbl_ptr->end(topo_node_ptr->module_ptr);
        }

        /* POST call back */
        if(NULL != cb_info_ptr)
        {
          result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_POST);
          if(ADSP_FAILED(result))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
                  "topo_utils: Module ID 0x%lX is failed PRE callback type",
                  topo_node_ptr->module_id);
            /* Continue on error */
          }
        }

        if(NULL != topo_node_ptr->fwk_extn_ptr)
        {
          qurt_elite_memory_free(topo_node_ptr->fwk_extn_ptr);
          topo_node_ptr->fwk_extn_ptr = NULL;
        }
        qurt_elite_memory_free(topo_node_ptr->module_ptr);
        topo_node_ptr->module_ptr = NULL;
      }
    }

    qurt_elite_memory_free(topo_ptr);
  }
  return ADSP_EOK;
}

ADSPResult topo_get_node(topo_t *topo_ptr, uint32_t module_id, uint16_t instance_id, topo_node_t **topo_node_ptr_ptr)
{
  uint32_t mod_idx = 0;

  if(NULL == topo_ptr)
  {
    return ADSP_EUNSUPPORTED;
  }

  *topo_node_ptr_ptr = NULL;

  for(mod_idx = 0; mod_idx < topo_ptr->num_modules; mod_idx++)
  {
    if(module_id == topo_ptr->module[mod_idx].module_id && (instance_id == topo_ptr->module[mod_idx].instance_id))
    {
      *topo_node_ptr_ptr = &topo_ptr->module[mod_idx];
      return ADSP_EOK;
    }
  }

  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "The module with id 0x%lx - 0x%hx is not found in the topo ", module_id, instance_id);
  return ADSP_EUNSUPPORTED;

}



ADSPResult topo_dealloc_temp_buffers(topo_t *topo_ptr)
{
  ADSPResult result = ADSP_EOK;

  if(NULL != topo_ptr->internal_buf_ptr)
  {
    qurt_elite_memory_free(topo_ptr->internal_buf_ptr);
    topo_ptr->internal_buf_ptr = NULL;
    topo_ptr->internal_buf_size = 0;
    memset(&topo_ptr->buffer1,0,sizeof(capi_v2_buf_t)*AFE_PORT_MAX_CHANNEL_CNT);
    memset(&topo_ptr->buffer2,0,sizeof(capi_v2_buf_t)*AFE_PORT_MAX_CHANNEL_CNT);
    memset(&topo_ptr->buflist1,0,sizeof(capi_v2_stream_data_t));
    memset(&topo_ptr->buflist2,0,sizeof(capi_v2_stream_data_t));
  }
  else
  {
    result = ADSP_EALREADY;
  }

  return result;
}
ADSPResult topo_allocate_temp_buffers(topo_t *topo_ptr,uint32_t buf_size)
{
  ADSPResult result = ADSP_EOK;

  uint32_t num_channels = topo_ptr->num_channels;
  uint32_t bufSize = buf_size * num_channels;
  uint32_t buf_size_per_channel = buf_size;

  uint32_t temp_bufs_size = bufSize * NUM_TEMP_BUFS;

  if(topo_ptr->internal_buf_size != temp_bufs_size)
  {
    if(NULL != topo_ptr->internal_buf_ptr)
    {
      qurt_elite_memory_free(topo_ptr->internal_buf_ptr);
      topo_ptr->internal_buf_ptr = NULL;
    }
    // Allocate memory
    /* Memory allocation for temporary buffers */
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Requesting %lu bytes default memory for re-allocating temp buffers", temp_bufs_size);
    topo_ptr->internal_buf_ptr = (uint8_t *)qurt_elite_memory_malloc(temp_bufs_size, QURT_ELITE_HEAP_DEFAULT);
    topo_ptr->internal_buf_size = temp_bufs_size;
  }
  if(NULL == topo_ptr->internal_buf_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_utils: Out of Memory while creating temp buffers !");
    return ADSP_ENOMEMORY;
  }


  uint8_t *ptr = topo_ptr->internal_buf_ptr;

  /* Set up the buffers */
  topo_ptr->buflist1.buf_ptr = topo_ptr->buffer1;
  topo_ptr->buflist2.buf_ptr = topo_ptr->buffer2;

  topo_ptr->buflist1.bufs_num = num_channels;
  topo_ptr->buflist2.bufs_num = num_channels;

  topo_ptr->buflist1.timestamp = 0;
  topo_ptr->buflist2.timestamp = 0;

  topo_ptr->buflist1.flags.is_timestamp_valid = FALSE;
  topo_ptr->buflist2.flags.is_timestamp_valid = FALSE;

  capi_v2_buf_t *pList1 = topo_ptr->buffer1;
  capi_v2_buf_t *pList2 = topo_ptr->buffer2;

  pList1[0].data_ptr = (int8_t *)ptr;

  for(uint32_t i = 0; i < num_channels; i++)
  {
    pList1[i].data_ptr = (int8_t *)ptr;
    pList1[i].actual_data_len = 0;
    pList1[i].max_data_len = buf_size_per_channel;
    ptr += buf_size_per_channel;

    pList2[i].data_ptr = (int8_t *)ptr;
    pList2[i].actual_data_len = 0;
    pList2[i].max_data_len = buf_size_per_channel;
    ptr += buf_size_per_channel;
  }

  return result;
}

/**
 * Sets up the input and output buffer indices for each module
 * in the topology.
 *
 * The logic for determinining the input and output buffers is
 * as follows:
 *
 * The input to the first module will be the input buffer. If
 * the module is inplace, its output will also be the same
 * buffer. The input and output will be set to the same value
 * till a non-inplace module is found.
 *
 * For the first non-inplace module, the output buffer will be
 * set to the tmp1 buffer. This buffer will be used as input and
 * output till the next non-inplace module.
 *
 * The output buffer for the next non-inplace module will be set
 * to tmp2. This procedure will continue, with the buffers being
 * switched between tmp1 and tmp2 at each non-inplace module.
 *
 * After this pass ends, the output of the last module is set to
 * the output buffer. If it is an inplace module, its input is
 * also set to the output buffer, and the output of the previous
 * module is set to the output buffer. This procedure is
 * repeated, going backwards and setting the input and output
 * buffer pointers of all modules to the output buffer, till the
 * first non-inplace module is found.
 *
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] modules
 *   The array of modules in the topology.
 * @param[in] num_modules
 *   Number of modules in the array.
 */
static void topo_set_buffer_indices(topo_node_t modules[], const uint32_t num_modules)
{
  uint32_t i;
  topo_node_t *module_info_ptr = &modules[0];
  uint32_t currentIdx = INPUT_BUF_INDEX;

  // Loop through the modules and set the input and output buffers.
  for(i = 0; i < num_modules; i++)
  {
    module_info_ptr->in_buf_index = currentIdx;

    if(!module_info_ptr->is_in_place)
    {
      if(BUF_1_INDEX == currentIdx)
      {
        currentIdx = BUF_2_INDEX;
      }
      else
      {
        currentIdx = BUF_1_INDEX;
      }
    }

    module_info_ptr->out_buf_index = currentIdx;

    module_info_ptr++;
  }

  // Now go backwards, filling up the last index with the output buffer index.
  //module_info_ptr = &modules[num_modules - 1];
  module_info_ptr--;
  uint32_t lastIdx = module_info_ptr->out_buf_index;
  module_info_ptr->out_buf_index = OUTPUT_BUF_INDEX;

  for(i = 0; i < num_modules - 1; i++)
  {
    if(lastIdx == module_info_ptr->in_buf_index)
    {
      module_info_ptr->in_buf_index = OUTPUT_BUF_INDEX;
      module_info_ptr--;
      module_info_ptr->out_buf_index = OUTPUT_BUF_INDEX;
    }
    else
    {
      break;
    }
  }
}


/*
  This function starts the topology and sends the necessary initial information for the topology 

  @param[in]  handle: Handle returned during topology creation
  @param[in]  media_fmt_ptr: Pointer to the media fmt describing data stream
  @param[in]  frame_size_in_sample: Frame size in samples
  @param[in]  cb_info_ptr: Pointer to a list of call back function informations
  @param[in]  num_cb_fn: Number fo call back function in the above list
  @param[in]  cb_info_ptr: Pointer to callback function
  
  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult topo_start(topo_handle_t handle,
                      topo_media_fmt_t *media_fmt_ptr,
                      uint32_t frame_size_in_samples,
                      topo_callback_info_t *cb_info_ptr)
{
  ADSPResult result = ADSP_EOK;
  capi_v2_err_t err_code = CAPI_V2_EOK;
  uint32_t mod_idx = 0;
  topo_node_t *topo_node_ptr = NULL;
  topo_node_t *topo_prev_node_ptr = NULL;
  capi_v2_t *module_ptr = NULL;
  capi_v2_proplist_t props_list;
  capi_v2_prop_t prop;
  topo_t *topo_ptr = (topo_t *)handle;
  uint32_t temp_buf_size = 0, buf_size = 0;
  uint32_t bytes_per_channel = 0;
  uint32_t sampling_rate = 0;
  uint32_t frame_size_in_ms = 0;
  if(NULL == topo_ptr)
  {
    return ADSP_EOK;
  }

  //Extracting topo need values from media format
  topo_ptr->bit_width = media_fmt_ptr->f.bits_per_sample;
  topo_ptr->bytes_per_channel = (topo_ptr->bit_width > 16)? 4: 2;
  topo_ptr->num_channels = media_fmt_ptr->f.num_channels;
  topo_ptr->sampling_rate = media_fmt_ptr->f.sampling_rate;
  topo_ptr->topo_input_buf_size = topo_ptr->bytes_per_channel * frame_size_in_samples;
  buf_size = topo_ptr->topo_input_buf_size;
  prop.id = CAPI_V2_INPUT_MEDIA_FORMAT;
  prop.payload.data_ptr = (int8_t *)media_fmt_ptr;
  prop.payload.max_data_len = sizeof(topo_media_fmt_t);
  prop.payload.actual_data_len = sizeof(topo_media_fmt_t);


  // send media fmt only to port 0
  prop.port_info.is_valid = TRUE;
  prop.port_info.port_index = 0;
  prop.port_info.is_input_port = TRUE;

  props_list.prop_ptr = &prop;
  props_list.props_num = 1;

  frame_size_in_ms = (frame_size_in_samples * 1000)/topo_ptr->sampling_rate;
  for(mod_idx = 0; mod_idx < topo_ptr->num_modules; mod_idx++)
  {
    topo_node_ptr = &topo_ptr->module[mod_idx];
    module_ptr = topo_node_ptr->module_ptr;

    topo_node_ptr->module_input_buf_size = topo_ptr->topo_input_buf_size;
    topo_node_ptr->module_ouput_buf_size = topo_ptr->topo_input_buf_size;
    /* For First Module in topology, topology media format is going to be its input media format
     * For Modules going forward if at all any MFC module is there then the media format
     * for modules after MFC is going to be output media format of MFC module till there is another
     * MFC module in topology.*/
    if(mod_idx>0)
    {
      topo_prev_node_ptr = &topo_ptr->module[mod_idx-1];
      topo_node_ptr->module_input_buf_size = topo_prev_node_ptr->module_input_buf_size;
      topo_node_ptr->module_ouput_buf_size = topo_node_ptr->module_input_buf_size;
      if(TRUE == topo_prev_node_ptr->is_output_media_event_raised)
      {
        if(!topo_is_equal_media_format(&(topo_prev_node_ptr->output_media_fmt), (topo_media_fmt_t *)prop.payload.data_ptr))
        {
          /* update the payload pointer to point to new media format information
           * This payload will eventually be sent to module as input media format set property*/
          prop.payload.data_ptr = (int8_t *)&(topo_prev_node_ptr->output_media_fmt);
          sampling_rate = topo_prev_node_ptr->output_media_fmt.f.sampling_rate;
          bytes_per_channel = (topo_prev_node_ptr->output_media_fmt.f.bits_per_sample > 16)? 4: 2;
          /* If at all any previous module output media format has changed to its input then
           * its output buffer and present module's input buffer size is going to be changes
           * So based on previous modules sampling rate and bytes per channel this has to be updated*/
          temp_buf_size = (sampling_rate * frame_size_in_ms * bytes_per_channel)/1000;
          topo_node_ptr->module_input_buf_size = temp_buf_size;
          /* Previous module's output buffer size is same as current module's input buffer size*/
          topo_prev_node_ptr->module_ouput_buf_size = temp_buf_size;
          if(temp_buf_size > buf_size)
          {
            buf_size = temp_buf_size;
          }
        }
      }
    }

    /* PRE call back */
    if(NULL != cb_info_ptr)
    {
      result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_PRE);
      if(ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
              "topo_utils: Module ID 0x%lX is failed PRE callback type",
              topo_node_ptr->module_id);
        return result;
      }
    }
    // Algo modules will be inited during this media format update
    err_code = module_ptr->vtbl_ptr->set_properties(module_ptr, &props_list);
    if(CAPI_V2_FAILED(err_code))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to initialize the module, 0x%lx", topo_node_ptr->module_id);

      return capi_v2_err_to_adsp_result(err_code);;
    }

    /* POST call back */
    if(NULL != cb_info_ptr)
    {
      result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_POST);
      if(ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
              "topo_utils: Module ID 0x%lX is failed POST callback type",
              topo_node_ptr->module_id);
        return result;
      }
    }
  }

  // Allocate and Set up the buffers
  result = topo_allocate_temp_buffers(topo_ptr,buf_size);
  if(ADSP_FAILED(result))
  {
    MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_utils: Error while creating temp buffers !");
    return result;
  }

  // Set the indices for intermediate buffers
  topo_set_buffer_indices(topo_ptr->module, topo_ptr->num_modules);

  topo_ptr->topo_state = TOPO_STATE_START;

  return result; 
}

/*
  This function sets paramers for a given module in the topology

  @param[in]  handle: Handle returned during topology creation
  @param[in]  module_id: module id
  @param[in]  param_id: Param id
  @param[in]  param_buf_len: Size of the parameter
  @param[in]  param_buf_ptr: pointer to calibration data payload
  @param[in]  cb_info_ptr: Pointer to callback function

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult topo_set_param(topo_handle_t handle,
                          uint32_t module_id,
                          uint16_t instance_id,
                          uint32_t param_id,
                          uint32_t param_buf_len,
                          int8_t *param_buf_ptr,
                          topo_callback_info_t *cb_info_ptr)
{
  ADSPResult result = ADSP_EOK;
  capi_v2_buf_t buf;
  capi_v2_t *module_ptr = NULL;
  topo_t *topo_ptr = (topo_t *)handle;
  topo_node_t *topo_node_ptr = NULL;

  capi_v2_port_info_t port_info;

  if(NULL == handle)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo pointer is NULL");
    return ADSP_EBADPARAM;
  }

  if(ADSP_EOK != (result = topo_get_node(topo_ptr, module_id, instance_id, &topo_node_ptr)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "getting node from topology is failed with result %d", result);
  }
  else
  {
    /* PRE call back */
    if(NULL != cb_info_ptr)
    {
      result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_PRE);
      if(ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
              "topo_utils: Module ID 0x%lX is failed PRE callback type",
              topo_node_ptr->module_id);
        return result;
      }
    }

    buf.data_ptr = (int8_t *)param_buf_ptr;
    buf.max_data_len = param_buf_len;
    buf.actual_data_len = param_buf_len;

    port_info.is_valid = FALSE;

    module_ptr = topo_node_ptr->module_ptr;
    if(!(module_ptr && module_ptr->vtbl_ptr))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
            "topo_utils: Module ID 0x%lX passed NULL pointer Failed",
            topo_node_ptr->module_id);
      return ADSP_EFAILED;
    }

    result = module_ptr->vtbl_ptr->set_param(module_ptr, param_id, &port_info, &buf);
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Set param for module id 0x%lx with param id 0x%lx result %d", module_id, param_id, result);

    /* POST call back */
    if(NULL != cb_info_ptr)
    {
      result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_POST);
      if(ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
              "topo_utils: Module ID 0x%lX is failed POST callback type",
              topo_node_ptr->module_id);
        return result;
      }
    }
  }

  return result; 
}

/*
  This function gets paramers for a given module in the topology

  @param[in]  handle: Handle returned during topology creation
  @param[in]  module_id: module id
  @param[in]  param_id: Param id
  @param[in]  param_buf_len: Size of the parameter buffer
  @param[in]  param_buf_ptr: pointer to calibration data payload
  @param[out] actual_param_buf_len: ptr to the parameter buffer length
  @param[in]  cb_info_ptr: Pointer to callback function

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult topo_get_param(topo_handle_t handle,
                          uint32_t module_id,
                          uint16_t instance_id,
                          uint32_t param_id,
                          int32_t param_buf_len,
                          int8_t *param_buf_ptr,
                          uint32_t *actual_param_buf_len,
                          topo_callback_info_t *cb_info_ptr)
{
  ADSPResult result = ADSP_EOK;
  capi_v2_buf_t buf;
  capi_v2_t *module_ptr = NULL;
  capi_v2_port_info_t port_info;
  topo_t *topo_ptr = (topo_t *)handle;
  topo_node_t *topo_node_ptr = NULL;

  if(NULL == topo_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo pointer is NULL");
    return ADSP_EBADPARAM;
  }

  if(ADSP_EOK != (result = topo_get_node(topo_ptr, module_id, instance_id, &topo_node_ptr)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "getting node from topology is failed with result %d", result);
  }
  else
  {
    /* PRE call back */
    if(NULL != cb_info_ptr)
    {
      result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_PRE);
      if(ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
              "topo_utils: Module ID 0x%lX is failed PRE callback type",
              topo_node_ptr->module_id);
        return result;
      }
    }

    buf.data_ptr = (int8_t *)param_buf_ptr;
    buf.max_data_len = param_buf_len;
    port_info.is_valid = false;
    module_ptr = topo_node_ptr->module_ptr;

    result = module_ptr->vtbl_ptr->get_param(module_ptr, param_id, &port_info, &buf);
    *actual_param_buf_len = buf.actual_data_len;
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Get param for module id 0x%lx with param id 0x%lx result %d", module_id, param_id, result);

    /* POST call back */
    if(NULL != cb_info_ptr)
    {
      result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_POST);
      if(ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
              "topo_utils: Module ID 0x%lX is failed POST callback type",
              topo_node_ptr->module_id);
        return result;
      }
    }
  }

	return result;
}

/*
  This function processes the topology

  @param[in]  handle: Handle returned during topology creation
  @param[in]  input_buf_ptr: pointer to the input buffer
  @param[in]  output_buf_ptr: pointer to the output buffer
  @param[in]  cb_info_ptr: Pointer to callback function
  @param[in]  timestamp: timestamp that need to be propagated to Modules
  @param[in]  is_timestamp_valid: Indicates if timestamp is valid or not

  @return
  Indication of success or failure.

  @dependencies
  None.
*/

ADSPResult topo_process(topo_handle_t handle,
                        int8_t *input_buf_ptr,
                        int8_t *output_buf_ptr,
                        topo_callback_info_t *cb_info_ptr,
                        uint64_t timestamp,
                        bool_t is_timestamp_valid)
{
  ADSPResult result = ADSP_EOK;
  uint32_t mod_idx = 0;
  capi_v2_t *module_ptr = NULL;
  capi_v2_buf_t in_buf[AFE_PORT_MAX_CHANNEL_CNT];
  capi_v2_buf_t out_buf[AFE_PORT_MAX_CHANNEL_CNT];
  capi_v2_stream_data_t in_buf_list, out_buf_list;
  uint32_t inp_buf_size_per_channel = 0, out_buf_size_per_channel = 0;
  capi_v2_stream_data_t *pProcessBuffers[TOTAL_BUFFERS];
  uint32_t in_buf_index;
  uint32_t out_buf_index;
  topo_t *topo_ptr = (topo_t *)handle;
  topo_node_t *topo_node_ptr = NULL;
  uint64_t module_timestamp = timestamp;
  uint8_t i = 0;
  if(NULL == topo_ptr)
  {
    return ADSP_EBADPARAM;
  }

  if(TOPO_STATE_START != topo_ptr->topo_state)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Topo is not in start state %d", topo_ptr->topo_state);
    return ADSP_EBADPARAM;
  }

  /* Topology's input buffer size per channel is going to be same as topo_ptr->topo_input_buf_size */
  inp_buf_size_per_channel = topo_ptr->topo_input_buf_size;

  /* Topologie's Output buffer size per channel is going to same as last module's module_ouput_buf_size*/
  topo_node_ptr = &topo_ptr->module[topo_ptr->num_modules-1];
  out_buf_size_per_channel = topo_node_ptr->module_ouput_buf_size;

  buf_convert_to_capi_v2_buf(input_buf_ptr,
                             &in_buf_list,
                             &in_buf[0],
                             topo_ptr->num_channels,
                             inp_buf_size_per_channel);
  buf_convert_to_capi_v2_buf(output_buf_ptr,
                             &out_buf_list,
                             &out_buf[0],
                             topo_ptr->num_channels,
                             out_buf_size_per_channel);


  // Set up the buffer pointers. These are used to send in
  // the appropriate input and output buffers in each process
  // call.
  pProcessBuffers[INPUT_BUF_INDEX] = &in_buf_list;
  pProcessBuffers[OUTPUT_BUF_INDEX] = &out_buf_list;
  pProcessBuffers[BUF_1_INDEX] = &topo_ptr->buflist1;
  pProcessBuffers[BUF_2_INDEX] = &topo_ptr->buflist2;

  //Loop through all the modules in topology and send common setparams like media fmt and cust info
  for(mod_idx = 0; mod_idx < topo_ptr->num_modules; mod_idx++)
  {
    topo_node_ptr = &topo_ptr->module[mod_idx];
    module_ptr = topo_node_ptr->module_ptr;

    in_buf_index = topo_node_ptr->in_buf_index;
    out_buf_index = topo_node_ptr->out_buf_index;

    for(i = 0; i<topo_ptr->num_channels; i++)
    {
      /* Filling timestamp, buffer sizes for input buffer*/
      pProcessBuffers[in_buf_index]->buf_ptr[i].actual_data_len = topo_node_ptr->module_input_buf_size;
      pProcessBuffers[in_buf_index]->buf_ptr[i].max_data_len = topo_node_ptr->module_input_buf_size;
      /* Filling buffer sizes for output buffer*/
      pProcessBuffers[out_buf_index]->buf_ptr[i].max_data_len = topo_node_ptr->module_ouput_buf_size;
    }
    pProcessBuffers[in_buf_index]->timestamp = module_timestamp;
    pProcessBuffers[in_buf_index]->flags.is_timestamp_valid = is_timestamp_valid;
    /* PRE call back */
    if(NULL != cb_info_ptr)
    {
      result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_PRE);
      if(ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
              "topo_utils: Module ID 0x%lX is failed PRE callback type",
              topo_node_ptr->module_id);
        return result;
      }
    }
    if(TRUE == topo_node_ptr->is_enabled)
    {
      module_ptr->vtbl_ptr->process(module_ptr, &pProcessBuffers[in_buf_index], &pProcessBuffers[out_buf_index]);
      if((TRUE == is_timestamp_valid) && ((int64_t)module_timestamp == pProcessBuffers[out_buf_index]->timestamp))
      {
        module_timestamp = module_timestamp - topo_node_ptr->delay_in_us;
      }
      else if((TRUE == is_timestamp_valid) && ((int64_t)module_timestamp != pProcessBuffers[out_buf_index]->timestamp))
      {
        module_timestamp = pProcessBuffers[out_buf_index]->timestamp;
      }
    }
    else
    {
      for(i = 0; i<topo_ptr->num_channels; i++)
      {
      memscpy(pProcessBuffers[out_buf_index]->buf_ptr[i].data_ptr,
              pProcessBuffers[out_buf_index]->buf_ptr[i].max_data_len,
              pProcessBuffers[in_buf_index]->buf_ptr[i].data_ptr,
              pProcessBuffers[in_buf_index]->buf_ptr[i].actual_data_len);
      }
    }
    /* POST call back */
    if(NULL != cb_info_ptr)
    {
      result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_POST);
      if(ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
              "topo_utils: Module ID 0x%lX is failed POST callback type",
              topo_node_ptr->module_id);
        return result;
      }
    }
  }
  return result; 
}

/*
  This function stops the topology

  @param[in]  handle: Handle returned during topology creation
  @param[in]  cb_info_ptr: Pointer to callback function
  
  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult topo_stop(topo_handle_t handle,
                     topo_callback_info_t *cb_info_ptr)
{
  ADSPResult result = ADSP_EOK;
  topo_t *topo_ptr = NULL;
  topo_node_t *topo_node_ptr = NULL;
  uint32_t mod_idx;

  if(NULL == handle)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null topology pointer");
    return ADSP_EBADPARAM;
  }

  topo_ptr = (topo_t *)handle;

  if(NULL != cb_info_ptr)
  {
    for(mod_idx = 0; mod_idx < topo_ptr->num_modules; mod_idx++)
    {
      topo_node_ptr = &topo_ptr->module[mod_idx];
      topo_node_ptr->is_output_media_event_raised = FALSE;
      /* PRE call back */
      cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_PRE);
      if(ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
              "topo_utils: Module ID 0x%lX is failed PRE callback type",
              topo_node_ptr->module_id);
        return result;
      }
    }
  }

  topo_dealloc_temp_buffers(topo_ptr);

  if(NULL != cb_info_ptr)
  {
    for(mod_idx = 0; mod_idx < topo_ptr->num_modules; mod_idx++)
    {
      topo_node_ptr = &topo_ptr->module[mod_idx];

      /* POST call back */
      result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_POST);
      if(ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
              "topo_utils: Module ID 0x%lX is failed POST callback type",
              topo_node_ptr->module_id);
        return result;
      }
    }
  }

  ((topo_t *)topo_ptr)->topo_state = TOPO_STATE_STOP;

  return result; 
}


/*
  This function set media type of input data

  @param[in]  handle: Handle returned during topology creation
  @param[in]  media_type_ptr: pointer to data media type
  @param[in]  cb_info_ptr: Pointer to callback function

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult topo_set_input_media_type(topo_handle_t handle,
                                     topo_media_fmt_t *media_fmt_ptr,
                                     topo_callback_info_t *cb_info_ptr)
{
  uint32_t iter = 0;
  capi_v2_t *module_ptr = NULL;
  capi_v2_prop_t prop;
  capi_v2_proplist_t props_list;
  ADSPResult result = ADSP_EOK;
  ADSPResult temp_result = ADSP_EOK;
  topo_t *topo_ptr = (topo_t *)handle;
  topo_node_t *topo_node_ptr = NULL;
  topo_node_t *topo_prev_node_ptr = NULL;
  if(NULL == topo_ptr || NULL == media_fmt_ptr)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "Set input media type, Null pointer args, "
          "topo_ptr=%p, media_type_ptr=%p",
          topo_ptr,media_fmt_ptr);
    return ADSP_EBADPARAM;
  }

  prop.id                      = CAPI_V2_INPUT_MEDIA_FORMAT;
  prop.payload.data_ptr        = (int8_t *)media_fmt_ptr;
  prop.payload.max_data_len    = sizeof(topo_media_fmt_t);
  prop.payload.actual_data_len = sizeof(topo_media_fmt_t);

  /* send media fmt only to port 0 */
  prop.port_info.is_valid = TRUE;
  prop.port_info.port_index = 0;
  prop.port_info.is_input_port = TRUE;

  props_list.prop_ptr  = &prop;
  props_list.props_num = 1;

  /* Loop in all modules in topology */
  for(iter = 0; iter < topo_ptr->num_modules; iter++)
  {
    topo_node_ptr = &topo_ptr->module[iter];
    module_ptr = topo_node_ptr->module_ptr;

    if(NULL == module_ptr)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "Null capi ptr for module index[%lu] in topology",
            iter);
      continue;
    }
    /* For First Module in topology, topology media format is going to be its input media format
     * For Modules going forward if at all any MFC module is there then the media format
     * for modules after MFC is going to be output media format of MFC module till there is another
     * MFC module in topology.*/
    if(iter>0)
    {
      topo_prev_node_ptr = &topo_ptr->module[iter-1];
      if(TRUE == topo_prev_node_ptr->is_output_media_event_raised)
      {
        if(!topo_is_equal_media_format(&(topo_prev_node_ptr->output_media_fmt), (topo_media_fmt_t *)prop.payload.data_ptr))
        {
          prop.payload.data_ptr = (int8_t *)&(topo_prev_node_ptr->output_media_fmt);
        }
      }
    }

    /* PRE call back */
    if(NULL != cb_info_ptr)
    {
      result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_PRE);
      if (ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
              "topo_utils: Module ID 0x%lX is failed PRE callback type",
              topo_node_ptr->module_id);
        return result;
      }
    }

    temp_result = module_ptr->vtbl_ptr->set_properties(module_ptr,
                                                       &props_list);
    if(ADSP_FAILED(temp_result))
    {
      result = ADSP_FAILED(result) ? result : temp_result;
      /* Log and continue */
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "Failed set media type for module_id[%lu] in topology",
            topo_ptr->module[0].module_id);
    }

    /* POST call back */
    if(NULL != cb_info_ptr)
    {
      result = cb_info_ptr->cb_f(topo_node_ptr, cb_info_ptr->cb_arg, TOPO_CB_TYPE_POST);
      if (ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
              "topo_utils: Module ID 0x%lX is failed POST callback type",
              topo_node_ptr->module_id);
        return result;
      }
    }
  }

  return result;
}

/*
  This function sets codec interrupt and relavant information to the modules which are
  registered for particular interrupts in the topology

  @param[in]  handle: pointer to the topology constructed
  @param[in]  int_id : Interrupt id
  @param[out]  param_size : Size of the codec interrupt related information
  @param[out]  data_buf_ptr : pointer to buffer 

  @return
  ADSPResult.

  @dependencies
  None.
*/
ADSPResult topo_cdc_int_set_param(topo_handle_t handle, uint32_t int_id, uint32_t param_size, int8_t *data_buf_ptr)
{
  topo_t *topo_ptr = (topo_t *)handle;
  topo_node_t *topo_node_ptr = NULL;
  ADSPResult result = ADSP_EOK, local_result;

  if(NULL == topo_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Topo pointer is NULL");
		return ADSP_EFAILED;
  }

  for(uint32_t mod_idx = 0; mod_idx < topo_ptr->num_modules; mod_idx++)
  {
    topo_node_ptr = &topo_ptr->module[mod_idx];
    for(uint32_t int_cnt = 0; int_cnt < topo_node_ptr->cdc_int_info.num_cdc_int.num_cdc_int; int_cnt++)
    {
      if(int_id == topo_node_ptr->cdc_int_info.cdc_int_ptr[int_cnt].cdc_int)
      {
        if(ADSP_EOK
           != (local_result = topo_set_param((void*)topo_ptr,
                                       topo_node_ptr->module_id, topo_node_ptr->instance_id, int_id,
                                       param_size,
                                       data_buf_ptr, NULL)))
        { 
          MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Topo Utils: codec interrupt 0x%lx  module id 0x%lx failed with result: "
                                                "0x%x", int_id, topo_node_ptr->module_id, local_result);
        }
        result |= local_result;
      }
    }
  }

 return result;
}

/*
  This function acks codec interrupt of the modules which are
  registered for particular interrupts in the topology

  @param[in]  handle: pointer to the topology constructed
  @param[in]  int_index : Interrupt id
  @param[in]  cb_f : call back function to do client specific action

  @return
  ADSPResult.

  @dependencies
  None.
*/
ADSPResult topo_ack_cdc_int_client_info(topo_handle_t handle, uint16_t int_index, topo_ack_cdc_int_cb_f cb_f)
{
  topo_t *topo_ptr = (topo_t *)handle;
  topo_node_t *topo_node_ptr = NULL;
  ADSPResult result = ADSP_EOK;

  if(NULL == topo_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Topo pointer is NULL");
    return ADSP_EFAILED;
  }

  if(NULL == cb_f)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Call back pointer is NULL");
    return ADSP_EFAILED;
  }

  for(uint32_t mod_idx = 0; mod_idx < topo_ptr->num_modules; mod_idx++)
  {
    topo_node_ptr = &topo_ptr->module[mod_idx];
    /* Search the client matching the interrupt index from codec svc msg payload */
    for(uint32_t idx = 0; idx < topo_node_ptr->cdc_int_info.num_cdc_int.num_cdc_int; idx++)
    {
      if(int_index == (uint32_t)topo_node_ptr->cdc_client_ptr[idx].client_info.int_index)
      {
        result = cb_f(&topo_node_ptr->cdc_client_ptr[idx].client_info);
        if(ADSP_FAILED(result))
        {
          MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
                "topo_utils: Module ID 0x%lX is failed for acking interrupt 0x%lx",
                topo_node_ptr->module_id, int_index);
          return result;
        }
      }
    }
  }
  return result; 
}

static void topo_module_query_intf_extn(topo_node_t *module_info_ptr, adsp_amdb_capi_v2_t *handle)
{
   capi_v2_err_t capi_result = CAPI_V2_EOK;

   if((NULL == module_info_ptr) || (NULL == handle))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"topo_module_query_intf_extn: received bad pointer");
      return;
   }

   intf_extn_chg_mf_t payload = {0};

   topo_module_intf_extn_list_t intf_ext_list = {
         {   INTF_EXTN_MAX   },
         {
            {INTF_EXTN_CHG_MF, FALSE, { (int8_t*)&payload, sizeof(intf_extn_chg_mf_t), sizeof(intf_extn_chg_mf_t) } },
         }
   };
   capi_v2_port_info_t port_info = {TRUE, FALSE, 0};

   capi_v2_prop_t static_props[] = {
         { CAPI_V2_INTERFACE_EXTENSIONS,    { reinterpret_cast<int8_t *>(&intf_ext_list), sizeof(intf_ext_list), sizeof(intf_ext_list) },   port_info },
   };

   capi_v2_proplist_t static_proplist = { 1, static_props };
   capi_result = adsp_amdb_capi_v2_get_static_properties_f(handle, NULL, &static_proplist);

   if(CAPI_V2_EOK == capi_result)
   {
      for(uint32_t i = 0; i < intf_ext_list.intf_extn_num.num_extensions; i++)
      {
         if(intf_ext_list.intf_extn_desc[i].is_supported)
         {
            // switch case used to make it easier to add future interface extensions.
            switch(intf_ext_list.intf_extn_desc[i].id)
            {
               case INTF_EXTN_CHG_MF:
               {
                  module_info_ptr->is_media_fmt_conv_capability = payload.bits_per_sample || payload.channel_info || payload.sampling_rate;
                  break;
               }
               // we should never reach the default case.
               default:
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"unsupported intf_extn 0x%lx for module 0x%lx", intf_ext_list.intf_extn_desc[i].id, module_info_ptr->module_id);
                  break;
               }
            }
         }
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"topo_module_query_intf_extn not supported for the module 0x%lx.", module_info_ptr->module_id);
   }
}

/*
  This function Sends output media format to modules which support media format covertion

  @param[in]  handle: pointer to the topology constructed
  @param[in]  pParams : output media format parameters

  @return
  ADSPResult.

  @dependencies
  None.
*/
ADSPResult topo_send_capiv2_output_media_format (topo_handle_t handle, const topo_send_output_media_fmt_t *pParams)
{
  topo_t *topo_ptr = (topo_t *)handle;

  if(NULL == topo_ptr)
  {
    return ADSP_EOK;
  }
  capi_v2_err_t err_code = CAPI_V2_EOK;

  intf_extn_param_id_chg_mf_propose_op_mf_t proposed_media_format;
  proposed_media_format.default_bits_per_sample = FALSE;
  proposed_media_format.bits_per_sample = pParams->bits_per_sample;
  proposed_media_format.default_sampling_rate = FALSE;
  proposed_media_format.sampling_rate = pParams->sampling_rate;
  proposed_media_format.default_channel_info = FALSE;
  if(pParams->num_channels == 0 || pParams->num_channels > SIZE_OF_ARRAY(pParams->channel_mapping))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid number of channels %lu", pParams->num_channels);
    return ADSP_EBADPARAM;
  }
  proposed_media_format.num_channels = pParams->num_channels;
  for(uint32_t i = 0; i < pParams->num_channels; i++)
  {
    proposed_media_format.channel_map[i] = (uint16_t)pParams->channel_mapping[i];
  }

  capi_v2_buf_t proposed_media_format_payload = {
      reinterpret_cast<int8_t*>(&proposed_media_format),
      sizeof(proposed_media_format),
      sizeof(proposed_media_format)
  };
  capi_v2_port_info_t port_info = {
      TRUE,
      FALSE,
         0
  };

  for(uint32_t i = 0; i < topo_ptr->num_modules; i++)
  {
      if(topo_ptr->module[i].is_media_fmt_conv_capability)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Proposing output media format to Module with ID 0x%lX.", topo_ptr->module[i].module_id);
      //reset the actual data length for every module
      proposed_media_format_payload.actual_data_len = sizeof(proposed_media_format);
      capi_v2_t *module_ptr = topo_ptr->module[i].module_ptr;

      err_code = module_ptr->vtbl_ptr->set_param(module_ptr, INTF_EXTN_PARAM_ID_CHG_MF_PROPOSE_OP_MF, &port_info, &proposed_media_format_payload);
      if (CAPI_V2_FAILED(err_code))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Module with ID %lX failed to set param for propose output media format.", topo_ptr->module[i].module_id);
      }
    }
  }
  return ADSP_EOK;
}

static bool_t topo_is_equal_media_format(const topo_media_fmt_t *f1, const topo_media_fmt_t *f2)
{
  bool_t is_equal = TRUE;

  is_equal = is_equal && (f1->h.data_format == f2->h.data_format);
  is_equal = is_equal && (f1->f.bits_per_sample == f2->f.bits_per_sample);
  is_equal = is_equal && (f1->f.bitstream_format == f2->f.bitstream_format);
  is_equal = is_equal && (f1->f.data_interleaving == f2->f.data_interleaving);
  is_equal = is_equal && (f1->f.data_is_signed == f2->f.data_is_signed);
  is_equal = is_equal && (f1->f.num_channels == f2->f.num_channels);
  is_equal = is_equal && (f1->f.q_factor == f2->f.q_factor);
  is_equal = is_equal && (f1->f.sampling_rate == f2->f.sampling_rate);

  if (is_equal)
  {
    if (CAPI_V2_DATA_FORMAT_INVALID_VAL != f1->f.num_channels)
    {
      for (uint32_t i = 0; i < f1->f.num_channels; i++)
      {
        is_equal = is_equal && (f1->f.channel_type[i] == f2->f.channel_type[i]);
      }
    }
  }

  return is_equal;
}
