/*=============================================================================
  @file sns_sensor.c

  Sole controller of the list of Sensors available on the system.  All other
  modules must query this one to create a new Sensor, or search for a specific
  Sensor.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_assert.h"
#include "sns_attribute_service.h"
#include "sns_cstruct_extn.h"
#include "sns_diag_print.h"
#include "sns_fw_attribute_service.h"
#include "sns_fw_diag_service.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_instance.h"
#include "sns_fw_service_manager.h"
#include "sns_fw_stream_service.h"
#include "sns_isafe_list.h"
#include "sns_island.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_printf_int.h"
#include "sns_rc.h"
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_suid_sensor.h"
#include "sns_types.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/

typedef struct sns_sensor_state_init
{
  sns_rc (*init)(sns_fw_sensor *sensor);
  void (*deinit)(sns_fw_sensor *sensor);
} sns_sensor_state_init;

/*=============================================================================
  Static Data Definitions
  ===========================================================================*/

/* Complete list of all registered libraries */
static sns_isafe_list libraries SNS_SECTION(".data.sns");
/* Lock to be held whenever adding or removing entries from libraries list */
static sns_osa_lock *libraries_lock SNS_SECTION(".data.sns");

/* Used for all sns_sensor_api functions */
// PEND: Return to static after sns_fw_printf is made a real sensor
sns_sensor_cb sensor_cb SNS_SECTION(".data.sns");

/* Maximum size allowed for a Sensor to request for its state buffer */
static const uint32_t state_len_max SNS_SECTION(".rodata.sns") = 1000;

/**
 * Each entry is called once per sensor init/deinit.
 * Called functions are intended to: deinitialize any state stored within the
 * sensor, and/or remove any separately managed state associated
 * with this Sensor.
 */
static sns_sensor_state_init state_init[] =
{
  { .init = NULL,                      .deinit = &sns_diag_svc_sensor_deinit },
  { .init = &sns_attr_svc_sensor_init, .deinit = &sns_attr_svc_sensor_deinit },
};

/* Temporary storage array; used by sns_sensor_foreach;
 * protected by libraries_lock */
static sns_sensor_library *tmp_libraries[10];

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* See sns_sensor_cb::get_service_manager */
SNS_SECTION(".text.sns") static struct sns_service_manager*
get_service_manager(sns_sensor const *this)
{
  UNUSED_VAR(this);
  return (sns_service_manager*)sns_service_manager_ref();
}

/* See sns_sensor_cb::get_sensor_instances */
SNS_SECTION(".text.sns") static sns_sensor_instance*
get_sensor_instance(sns_sensor const *this, bool first)
{
  sns_fw_sensor *sensor = (sns_fw_sensor*)this;
  sns_fw_sensor_instance *rv = NULL;
  sns_isafe_list_iter *iter = &sensor->instances_iter;
  sns_isafe_list_item *item = NULL;

  while(NULL == rv)
  {
    if(first)
    {
      sns_isafe_list_iter_init(iter, &sensor->sensor_instances, true);
      sns_isafe_list_iter_set_always_check_island_ptrs(iter, true);
      item = sns_isafe_list_iter_curr(iter);
      first = false;
    }
    else if(NULL != iter->list)
    {
      item = sns_isafe_list_iter_advance(iter);
    }

    if(NULL != item)
    {
      rv = (sns_fw_sensor_instance*)sns_isafe_list_item_get_data(item);
      if(rv->inst_removing != SNS_INST_REMOVING_NOT_START)
        rv = NULL;
    }
    else
      break;
  }

  if(NULL != rv && SNS_ISLAND_STATE_IN_ISLAND != rv->island_operation)
  {
    SNS_ISLAND_EXIT();
  }

  return &rv->instance;
}

/* See sns_sensor_cb::get_library_sensors */
SNS_SECTION(".text.sns") static sns_sensor*
get_library_sensor(sns_sensor const *this, bool first)
{
  sns_fw_sensor *sensor = (sns_fw_sensor*)this;
  sns_isafe_list_iter *iter = &sensor->library->sensors_iter;
  sns_sensor *rv = NULL;
  sns_isafe_list_item *item = NULL;

  if(first)
  {
    sns_isafe_list_iter_init(iter, &sensor->library->sensors, true);
    sns_isafe_list_iter_set_always_check_island_ptrs(iter, true);
    item = sns_isafe_list_iter_curr(iter);
  }
  else if(NULL != iter->list)
  {
    item = sns_isafe_list_iter_advance(iter);
  }

  if(NULL != item)
    rv = (sns_sensor*)sns_isafe_list_item_get_data(item);

  return rv;
}

/* See sns_sensor_cb::get_registration_index */
static uint32_t
get_registration_index(sns_sensor const *this)
{
  return ((sns_fw_sensor*)this)->library->registration_index;
}

/**
 * Determines whether the Sensor associated with the specified API should
 * be eligible to be placed in Island memory.
 */
static bool
is_island_sensor(struct sns_sensor_api const *sensor_api,
    struct sns_sensor_instance_api const *instance_api)
{
  return sns_island_is_island_ptr((intptr_t)sensor_api) &&
    sns_island_is_island_ptr((intptr_t)sensor_api->get_sensor_uid) &&
    (NULL == instance_api ||
      (sns_island_is_island_ptr((intptr_t)instance_api) &&
       sns_island_is_island_ptr((intptr_t)instance_api->notify_event)));
}

/**
 * Updates the island state of all instances associated with a sensor.
 *
 * @param[i] sensor Sensor to be updated
 * @param[i] enabled True if island mode will be enabled for use
 */
SNS_SECTION(".text.sns") static void
set_all_instance_island_state(sns_fw_sensor *sensor, bool enabled)
{
  sns_fw_sensor_instance *instance;

  for(instance = (sns_fw_sensor_instance*)sensor->sensor.cb->get_sensor_instance(&sensor->sensor, true);
      NULL != instance;
      instance = (sns_fw_sensor_instance*)sensor->sensor.cb->get_sensor_instance(&sensor->sensor, false))
  {
    sns_sensor_instance_set_island(instance, enabled);
  }
}

/**
 * Add a library to the temporary array.  Do not invoke this function
 * unless array has at least one empty spot.  If library is already present,
 * move to end of array (and shift-up others as necessary).
 */
static void
add_library(sns_sensor_library *library)
{
  sns_sensor_library *temp = NULL;

  for(int i = 0; i < ARR_SIZE(tmp_libraries); i++)
    if(tmp_libraries[i] == library)
      tmp_libraries[i] = NULL;

  for(int i = ARR_SIZE(tmp_libraries) - 1; i >= 0; i--)
  {
    temp = tmp_libraries[i];
    tmp_libraries[i] = library;

    library = temp;
    if(NULL == temp)
      break;
  }
}

/**
 * Utility function to be used in conjunction with sns_sensor_foreach.
 * Get the next library and lock its mutex.  May be blocking.
 *
 * @param[io] iter Initialized iterator over a list of libraries
 *
 * @return Library, or NULL when all libraries have been processed
 */
static sns_sensor_library*
get_next_library(sns_isafe_list_iter *iter)
{
  sns_sensor_library *library = NULL;
  uint32_t attempts = 0;

  while(NULL == library)
  {
    uint32_t arr_len = 0;

    for(int i = 0; i < ARR_SIZE(tmp_libraries); i++)
      if(NULL != tmp_libraries[i])
        arr_len++;

    if(arr_len != ARR_SIZE(tmp_libraries) && NULL != sns_isafe_list_iter_curr(iter))
    {
      library = (sns_sensor_library*)
        sns_isafe_list_iter_get_curr_data(iter);
      sns_isafe_list_iter_advance(iter);
    }
    else
    {
      for(int i = 0; i < ARR_SIZE(tmp_libraries); i++)
      {
        if(NULL != tmp_libraries[i])
        {
          library = tmp_libraries[i];
          tmp_libraries[i] = NULL;
          break;
        }
      }
      attempts++;
    }

    if(NULL == library)
      break;

    if(SNS_RC_SUCCESS != sns_osa_lock_try_acquire(library->library_lock))
    {
      // If try-lock fails, and we either don't have any more room on the
      // temporary array, or are out of libraries from the iterator
      if(attempts >= arr_len &&
          (arr_len == ARR_SIZE(tmp_libraries) ||
           NULL == sns_isafe_list_iter_curr(iter)))
      {
        sns_osa_lock_acquire(library->library_lock);
        break;
      }
      add_library(library);
      library = NULL;
    }
  }

  return library;
}

/**
 * Check whether the SUID contained in attr_info matches the provided SUID.
 *
 * @see sns_attr_svc_sensor_foreach_func
 *
 * @return False if the two SUIDs are a match; if the SUIDs do not match,
 *         return true.
 */
static bool
suid_match(struct sns_attribute_info *attr_info, void *arg)
{
  sns_sensor_uid suid;
  sns_sensor_uid *suid_arg = (sns_sensor_uid*)arg;

  suid = sns_attr_info_get_suid(attr_info);
  if(sns_sensor_uid_compare(&suid, suid_arg))
    return false;

  return true;
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_rc
sns_sensor_init_fw(void)
{
  sns_rc rc;
  sns_osa_lock_attr attr;
  sns_isafe_list_init(&libraries);

  rc = sns_osa_lock_attr_init(&attr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_ISLAND);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_create(&attr, &libraries_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  sensor_cb = (sns_sensor_cb)
  {
    .struct_len = sizeof(sensor_cb),
    .get_service_manager = &get_service_manager,
    .get_sensor_instance = &get_sensor_instance,
    .create_instance = &sns_sensor_instance_init,
    .remove_instance = &sns_sensor_instance_deinit,
    .get_library_sensor = &get_library_sensor,
    .get_registration_index = &get_registration_index,
  };

  return SNS_RC_SUCCESS;
}

sns_sensor_library*
sns_sensor_library_init(sns_register_sensors register_func,
    uint32_t registration_index)
{
  sns_isafe_list_iter iter;
  sns_osa_lock_attr attr;
  sns_rc rc;
  sns_sensor_library *library = sns_malloc(SNS_HEAP_ISLAND, sizeof(*library));
  SNS_ASSERT(NULL != library);

  SNS_PRINTF(HIGH, sns_fw_printf, "Initializing new sensor library "SNS_DIAG_PTR, library);
  sns_isafe_list_item_init(&library->list_entry, library);

  sns_isafe_list_init(&library->sensors);
  library->register_func = register_func;
  library->registration_index = registration_index;
  library->removing = SNS_LIBRARY_INIT;

  rc = sns_osa_lock_attr_init(&attr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_ISLAND);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_create(&attr, &library->library_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  sns_osa_lock_acquire(libraries_lock);
  sns_isafe_list_iter_init(&iter, &libraries, false);
  sns_isafe_list_iter_insert(&iter, &library->list_entry, true);
  sns_osa_lock_release(libraries_lock);

  return library;
}

sns_rc
sns_sensor_library_deinit(sns_sensor_library *library)
{
  sns_isafe_list_iter iter;
  SNS_PRINTF(LOW, sns_fw_printf, "sns_sensor_library_deinit "SNS_DIAG_PTR, library);

  for(sns_isafe_list_iter_init(&iter, &library->sensors, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_fw_sensor *sensor = (sns_fw_sensor*)
      sns_isafe_list_iter_get_curr_data(&iter);
    sns_sensor_deinit(sensor);
  }

  library->removing = SNS_LIBRARY_REMOVE;
  return SNS_RC_SUCCESS;
}

sns_rc
sns_sensor_init(uint32_t state_len, struct sns_sensor_api const *sensor_api,
    struct sns_sensor_instance_api const *instance_api)
{
  sns_rc rv = SNS_RC_SUCCESS;

  if(state_len > state_len_max)
  {
    rv = SNS_RC_POLICY;
  }
  else
  {
    sns_isafe_list_iter iter;
    sns_fw_sensor *sensor = NULL;
    sns_sensor_library *library;
    bool island_sensor = is_island_sensor(sensor_api, instance_api);
    uint8_t attr_idx = 0;
    uint8_t state_idx = 0;
    size_t alloc_size = sns_cstruct_extn_compute_total_size(sizeof(*sensor),2,
      ALIGN_8(sizeof(struct sns_sensor_state) + state_len) + ALIGN_8(sizeof(struct sns_attribute_info)));

    // The current library will always be at the end of the list
    sns_isafe_list_iter_init(&iter, &libraries, false);
    library = (sns_sensor_library*)
      sns_isafe_list_iter_get_curr_data(&iter);

    if(island_sensor)
    {
      sensor = sns_malloc(SNS_HEAP_ISLAND, alloc_size);
    }
    if(NULL == sensor)
    {
      sensor = sns_malloc(SNS_HEAP_MAIN, alloc_size);
      island_sensor = false;
    }
    SNS_ASSERT(NULL != sensor);
    sns_cstruct_extn_init(&sensor->extn, sensor, sizeof(*sensor), 2);
    attr_idx = sns_cstruct_extn_setup_buffer(&sensor->extn, sizeof(struct sns_attribute_info));
    state_idx = sns_cstruct_extn_setup_buffer(&sensor->extn, sizeof(struct sns_sensor_state) + state_len);

    sensor->sensor.cb = &sensor_cb;
    sensor->sensor.sensor_api = sensor_api;
    sensor->sensor.instance_api = instance_api;


    sensor->sensor.state = (struct sns_sensor_state*)
      sns_cstruct_extn_get_buffer(&sensor->extn, state_idx);
    sensor->sensor.state->state_len = state_len;

    sensor->attr_info = sns_cstruct_extn_get_buffer(&sensor->extn, attr_idx);

    for(uint8_t i = 0; i < ARR_SIZE(state_init); i++)
      if(NULL != state_init[i].init)
        state_init[i].init(sensor);

    sensor->island_operation = island_sensor ?
      SNS_ISLAND_STATE_IN_ISLAND : SNS_ISLAND_STATE_NOT_IN_ISLAND;

    sensor->removing = false;
    sensor->library = library;
    sensor->diag_config.config = default_datatype;

    sns_isafe_list_init(&sensor->data_streams);
    sns_isafe_list_init(&sensor->sensor_instances);
    sns_isafe_list_item_init(&sensor->list_entry, sensor);

    sns_osa_lock_acquire(library->library_lock);
    sns_isafe_list_iter_init(&iter, &library->sensors, false);
    sns_isafe_list_iter_insert(&iter, &sensor->list_entry, true);
    sns_osa_lock_release(library->library_lock);
  }
  return rv;
}

void
sns_sensor_library_start(sns_sensor_library *library)
{
  sns_isafe_list_iter iter;

  for(sns_isafe_list_iter_init(&iter, &library->sensors, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_fw_sensor *sensor = (sns_fw_sensor*)
        sns_isafe_list_iter_get_curr_data(&iter);

    sns_rc rc = sensor->sensor.sensor_api->init((sns_sensor*)sensor);
    if(SNS_RC_SUCCESS != rc)
    {
      SNS_PRINTF(MED, sns_fw_printf,
          "Initialization failed for sensor "SNS_DIAG_PTR, sensor);
      sns_sensor_deinit(sensor);
    }
    else
    {
      sns_sensor_uid const *sensor_uid =
        sensor->sensor.sensor_api->get_sensor_uid((sns_sensor*)sensor);

      SNS_PRINTF(MED, sns_fw_printf, "Initialized new sensor "SNS_DIAG_PTR
          " in island: %d, with SUID %"PRIsuid,
          sensor, sensor->island_operation, SNS_PRI_SUID(sensor_uid));

      if(sns_sensor_uid_compare(sensor_uid, &(sns_sensor_uid){{0}}) ||
         !sns_attr_svc_sensor_foreach(&suid_match, (void*)sensor_uid))
      {
        SNS_PRINTF(ERROR, sns_fw_printf,
            "Invalid SUID for sensor " SNS_DIAG_PTR, sensor);
        sns_sensor_deinit(sensor);
      }

      sns_attr_info_set_suid(sensor->attr_info, sensor_uid);
    }
  }
}

/**
 * As with removing Sensor Instances, removing Sensors is typically an
 * asynchronous process.  First we must start the deinitiation process
 * for every active stream, as well as every active Instance.  Upon the
 * asynchronous completion of the last deinitiation, this Sensor will
 * be removed.
 */
sns_rc
sns_sensor_deinit(sns_fw_sensor *sensor)
{
  if(sns_isafe_list_item_present(&sensor->list_entry) && !sensor->removing)
  {
    sns_sensor_instance *instance;
    sns_isafe_list_iter iter;
    bool freeable = true;
    char data_type[32];

    SNS_PRINTF(MED, sns_fw_printf, "sns_sensor_deinit "SNS_DIAG_PTR, sensor);

    sensor->removing = true;

    sns_isafe_list_iter_init(&iter, &sensor->data_streams, true);
    if(0 < sns_isafe_list_iter_len(&iter))
    {
      sns_service_manager *svc_mgr = get_service_manager(&sensor->sensor);
      sns_stream_service *stream_svc = (sns_stream_service*)
              svc_mgr->get_service(svc_mgr, SNS_STREAM_SERVICE);

      for(sns_isafe_list_iter_init(&iter, &sensor->data_streams, true);
          NULL != sns_isafe_list_iter_curr(&iter);
          sns_isafe_list_iter_advance(&iter))
      {
        struct sns_data_stream *stream =
           sns_isafe_list_iter_get_curr_data(&iter);
        stream_svc->api->remove_stream(stream_svc, stream);
      }
      freeable = false;
    }

    sns_isafe_list_item_remove(&sensor->list_entry);
    sns_attr_info_get_data_type(sensor->attr_info, data_type, sizeof(data_type));
    sns_suid_sensor_apprise(data_type);

    // PEND: de-register from power rail service

    while(NULL != (instance =
        sensor->sensor.cb->get_sensor_instance(&sensor->sensor, true)))
    {
      sns_sensor_instance_deinit(instance);
      freeable = false;
    }

    if(freeable)
      sns_sensor_delete(sensor);
  }

  return SNS_RC_SUCCESS;
}

// If sensor is located in non-island memory, this function will always be
// called in non-island mode (hence there's no need to additionally check here)
SNS_SECTION(".text.sns") bool
sns_sensor_delete(sns_fw_sensor *sensor)
{
  sns_isafe_list_iter instance_iter, stream_iter;
  if(!sns_island_is_island_ptr((intptr_t)sensor))
  {
    SNS_ISLAND_EXIT();
  }
  sns_isafe_list_iter_init(&instance_iter, &sensor->sensor_instances, true);
  sns_isafe_list_iter_init(&stream_iter, &sensor->data_streams, true);
  if(sensor->removing && 0 == sns_isafe_list_iter_len(&instance_iter) &&
     0 == sns_isafe_list_iter_len(&stream_iter))
  {
    sns_isafe_list_iter iter;
    SNS_ISLAND_EXIT();
    sns_sensor_library *library = sensor->library;
    SNS_PRINTF(MED, sns_fw_printf, "sns_sensor_delete "SNS_DIAG_PTR, sensor);

    for(uint8_t i = 0; i < ARR_SIZE(state_init); i++)
      if(NULL != state_init[i].deinit)
        state_init[i].deinit(sensor);

    sns_isafe_list_item_remove(&sensor->list_entry);
    sns_free(sensor);

    sns_isafe_list_iter_init(&iter, &library->sensors, true);
    if(0 == sns_isafe_list_iter_len(&iter))
      sns_sensor_library_deinit(library);

    return true;
  }
  return false;
}

SNS_SECTION(".text.sns") bool
sns_sensor_library_delete(sns_sensor_library *library)
{
  sns_isafe_list_iter iter;
  sns_isafe_list_iter_init(&iter, &library->sensors, true);
  sns_osa_lock_acquire(library->library_lock);

  if(0 == sns_isafe_list_iter_len(&iter) &&
     SNS_LIBRARY_REMOVE == library->removing)
  {
    SNS_ISLAND_EXIT();
    SNS_PRINTF(LOW, sns_fw_printf, "sns_sensor_library_delete "SNS_DIAG_PTR, library);
    sns_osa_lock_release(library->library_lock);
    sns_osa_lock_delete(library->library_lock);

    sns_osa_lock_acquire(libraries_lock);
    sns_isafe_list_item_remove(&library->list_entry);
    sns_osa_lock_release(libraries_lock);

    sns_free(library);

    return true;
  }
  return false;
}

bool
sns_sensor_foreach(sns_sensor_foreach_func func, void *arg)
{
  bool bail = false;
  sns_isafe_list_iter iter_library, iter_sensor;
  sns_sensor_library *library;

  sns_osa_lock_acquire(libraries_lock);
  sns_memset(tmp_libraries, 0, sizeof(tmp_libraries));

  sns_isafe_list_iter_init(&iter_library, &libraries, true);
  while(!bail && NULL != (library = get_next_library(&iter_library)))
  {
    for(sns_isafe_list_iter_init(&iter_sensor, &library->sensors, true);
        NULL != sns_isafe_list_iter_curr(&iter_sensor) && !bail;
        sns_isafe_list_iter_advance(&iter_sensor))
    {
      sns_fw_sensor *sensor = (sns_fw_sensor*)
        sns_isafe_list_iter_get_curr_data(&iter_sensor);

      if(!func(sensor, arg))
        bail = true;
    }

    sns_osa_lock_release(library->library_lock);
  }

  sns_osa_lock_release(libraries_lock);
  return !bail;
}

SNS_SECTION(".text.sns") bool
sns_sensor_set_island(sns_fw_sensor *sensor, bool enabled)
{
  bool rv = false;
  if(!enabled)
  {
    if(SNS_ISLAND_STATE_IN_ISLAND == sensor->island_operation)
    {
      sensor->island_operation = SNS_ISLAND_STATE_ISLAND_DISABLED;
      set_all_instance_island_state(sensor, enabled);
    }
    rv = true;
  }
  else
  {
    if(SNS_ISLAND_STATE_ISLAND_DISABLED == sensor->island_operation)
    {
      sns_isafe_list_iter iter;
      struct sns_fw_data_stream *data_stream;

      rv = true;
      sns_isafe_list_iter_init(&iter, &sensor->data_streams, true);
      for(data_stream = sns_isafe_list_iter_get_curr_data(&iter);
          NULL != data_stream;
          data_stream = sns_isafe_list_item_get_data(sns_isafe_list_iter_advance(&iter)))
      {
        if(!sns_island_is_island_ptr((intptr_t)data_stream))
        {
          rv = false;
          break;
        }
      }

      if(rv)
      {
        sensor->island_operation = SNS_ISLAND_STATE_IN_ISLAND;
        set_all_instance_island_state(sensor, enabled);
      }
    }
  }

  return rv;
}
