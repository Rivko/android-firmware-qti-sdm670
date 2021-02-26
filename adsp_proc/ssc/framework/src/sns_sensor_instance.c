/*=============================================================================
  @file sns_sensor_instance.c

  All functionality related to the creation, deletion, and modification of
  Sensor Instances.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_assert.h"
#include "sns_diag_print.h"
#include "sns_fw_data_stream.h"
#include "sns_fw_event_service.h"
#include "sns_fw_log.h"
#include "sns_fw_request.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_instance.h"
#include "sns_fw_service_manager.h"
#include "sns_fw_stream_service.h"
#include "sns_isafe_list.h"
#include "sns_island.h"
#include "sns_memmgr.h"
#include "sns_printf_int.h"
#include "sns_pwr_sleep_mgr.h"
#include "sns_rc.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_types.h"

#include "sns_fw_diag_service.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/

typedef struct sns_sensor_instance_state_init
{
  sns_rc (*init)(sns_fw_sensor_instance *instance, sns_fw_sensor *sensor);
  void (*deinit)(sns_fw_sensor_instance *instance);
} sns_sensor_instance_state_init;

/*=============================================================================
  Forward Declarations
  ===========================================================================*/
extern bool sns_instance_streams_in_island(sns_sensor_instance *instance);

/*=============================================================================
  Static Data Definitions
  ===========================================================================*/

static sns_sensor_instance_cb instance_cb SNS_SECTION(".data.sns");

/**
 * Each entry is called once per sensor instance init/deinit.
 * Called functions are intended to: deinitialize any state stored within the
 * sensor instance, and/or remove any separately managed state associated
 * with this Sensor Instance.
 */
static sns_sensor_instance_state_init state_init[] SNS_SECTION(".data.sns") =
{
  { .init = &sns_diag_sensor_instance_init, .deinit = &sns_diag_sensor_instance_deinit },
  { .init = NULL, .deinit = &sns_pwr_sleep_mgr_sensor_instance_deinit },
  { .init = NULL, .deinit = &sns_event_service_deinit_instance },
};

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* See sns_sensor_instance_cb::get_service_manager */
SNS_SECTION(".text.sns") static sns_service_manager*
get_service_manager(sns_sensor_instance *this)
{
  UNUSED_VAR(this);
  return (sns_service_manager*)sns_service_manager_ref();
}

/**
 * Find the client request list of a Sensor Instance associated with a
 * particular SUID.
 *
 * @return Found request list; or NULL if not found
 */
SNS_SECTION(".text.sns") static sns_client_req_list*
find_req_list(sns_fw_sensor_instance *this, sns_sensor_uid const *suid)
{
  sns_isafe_list_iter iter;
  sns_client_req_list *rv = NULL;

  if (NULL != this && NULL != suid)
  {
    for(sns_isafe_list_iter_init(&iter, &this->client_req_lists, true);
        NULL != sns_isafe_list_iter_curr(&iter);
        sns_isafe_list_iter_advance(&iter))
    {
      sns_client_req_list *req_list = (sns_client_req_list*)
        sns_isafe_list_iter_get_curr_data(&iter);

      if(sns_sensor_uid_compare(suid, req_list->suid))
      {
        rv = req_list;
        break;
      }
    }
  }

  return rv;
}

/**
 * See sns_sensor_instance_cb::get_client_request
 */
SNS_SECTION(".text.sns") static sns_request const*
get_client_request(sns_sensor_instance *this,
    sns_sensor_uid const *suid, bool first)
{
  sns_request *rv = NULL;
  sns_fw_sensor_instance *instance = (sns_fw_sensor_instance*)this;
  sns_client_req_list *req_list = NULL;
  if(NULL == suid)
  {
    suid = instance->sensor->sensor.sensor_api->get_sensor_uid(&(instance->sensor->sensor));
  }
  req_list = find_req_list(instance, suid);

  if(NULL != req_list)
  {
    sns_isafe_list_iter *iter = &req_list->iter;
    sns_isafe_list_item *item = NULL;

    if(first)
    {
      sns_isafe_list_iter_init(iter, &req_list->client_requests, true);
      sns_isafe_list_iter_set_always_check_island_ptrs(iter, true);
      item = sns_isafe_list_iter_curr(iter);
    }
    else if(NULL != iter->list)
    {
      item = sns_isafe_list_iter_advance(iter);
    }

    if(NULL != item)
    {
      rv = (sns_request*)sns_isafe_list_item_get_data(item);
      if (NULL != rv)
      {
        //SNS_ASSERT(sns_data_stream_validate_request((sns_fw_request*)rv));
      }
    }
  }

  return rv;
}

/**
 *  See sns_sensor_instance_cb::remove_client_request
 */
SNS_SECTION(".text.sns") static void
remove_client_request(sns_sensor_instance *this,
    struct sns_request const *request)
{
  UNUSED_VAR(this);
  //SNS_ASSERT(sns_data_stream_validate_request((sns_fw_request*)request));
  sns_isafe_list_item_remove(&((sns_fw_request*)request)->list_entry);
  sns_fw_log_inst_map(this);
}

/**
 *  See sns_sensor_instance_cb::add_client_request
 */
SNS_SECTION(".text.sns") static void
add_client_request(sns_sensor_instance *this,
    struct sns_request const *request)
{
  sns_isafe_list_iter iter;
  sns_fw_sensor_instance *instance = (sns_fw_sensor_instance*)this;
  sns_fw_request *fw_request = (sns_fw_request*)request;

  //SNS_VERBOSE_ASSERT(sns_data_stream_validate_request((sns_fw_request*)fw_request),
  //                   "add_client_request() called with invalid request");

  //If the stream has been removed on which the request came in then assert.
  SNS_ASSERT(SNS_DATA_STREAM_WAIT_RECEIPT != fw_request->stream->removing);

  if(!sns_island_is_island_ptr((intptr_t)fw_request->stream) ||
     SNS_ISLAND_STATE_IN_ISLAND != fw_request->stream->island_operation)
  {
    SNS_ISLAND_EXIT();
  }

  SNS_VERBOSE_ASSERT((SNS_INST_REMOVING_NOT_START == instance->inst_removing), 
      "Adding request to removed instance");

  sns_sensor *src_sensor = &fw_request->stream->data_source->sensor;
  sns_sensor_uid const *suid = src_sensor->sensor_api->get_sensor_uid(src_sensor);
  sns_client_req_list *req_list = find_req_list(instance, suid);
  bool island_operation = (SNS_ISLAND_STATE_NOT_IN_ISLAND != instance->island_operation);

  if(NULL == req_list)
  {
    if(island_operation)
    {
      req_list = sns_malloc(SNS_HEAP_ISLAND, sizeof(*req_list));
    }
    if(NULL == req_list)
    {
      SNS_ISLAND_EXIT();
      req_list = sns_malloc(SNS_HEAP_MAIN, sizeof(*req_list));
      instance->island_operation = SNS_ISLAND_STATE_NOT_IN_ISLAND;
    }
    SNS_ASSERT(NULL != req_list);

    req_list->suid = suid;
    sns_isafe_list_init(&req_list->client_requests);

    sns_isafe_list_item_init(&req_list->list_entry, req_list);
    sns_isafe_list_iter_init(&iter, &instance->client_req_lists, true);
    sns_isafe_list_iter_insert(&iter, &req_list->list_entry, true);
  }

  sns_isafe_list_iter_init(&iter, &req_list->client_requests, false);
  sns_isafe_list_iter_insert(&iter, &fw_request->list_entry, true);
}

/**
 * Alloc a sensor instance
 */
SNS_SECTION(".text.sns") static sns_fw_sensor_instance *
sns_sensor_instance_alloc(sns_fw_sensor *fw_sensor, size_t state_size, size_t diag_size)
{
  sns_fw_sensor_instance *instance = NULL;
  size_t alloc_size = sns_cstruct_extn_compute_total_size(sizeof(*instance), 2, state_size + diag_size);
  bool island_instance = (SNS_ISLAND_STATE_NOT_IN_ISLAND != fw_sensor->island_operation);

  if(island_instance)
  {
    instance = sns_malloc(SNS_HEAP_ISLAND, alloc_size );
  }

  if(NULL == instance)
  {
    SNS_ISLAND_EXIT();
    instance = sns_malloc(SNS_HEAP_MAIN, alloc_size);
  }
  SNS_ASSERT(NULL != instance);
  return instance;
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_rc
sns_sensor_instance_init_fw(void)
{
  instance_cb = (sns_sensor_instance_cb)
  {
    .struct_len = sizeof(instance_cb),
    .get_service_manager = &get_service_manager,
    .get_client_request = &get_client_request,
    .remove_client_request = &remove_client_request,
    .add_client_request = &add_client_request
  };

  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_sensor_instance*
sns_sensor_instance_init(sns_sensor *sensor, uint32_t state_len)
{
  sns_fw_sensor *fw_sensor = (sns_fw_sensor*)sensor;
  sns_fw_sensor_instance *instance = NULL;
  sns_isafe_list_iter iter;
  sns_rc rv = SNS_RC_SUCCESS;
  bool island_instance = SNS_ISLAND_STATE_NOT_IN_ISLAND != fw_sensor->island_operation;
  uint32_t sensors_cnt = fw_sensor->library->sensors.cnt;
  size_t state_size = ALIGN_8(state_len) + sizeof(struct sns_sensor_instance_state);
  size_t diag_size = ALIGN_8(sizeof(sns_diag_src_config)) * sensors_cnt;
  uint8_t state_idx = 0;

  instance = sns_sensor_instance_alloc(fw_sensor, state_size, diag_size);
  island_instance = island_instance && sns_island_is_island_ptr((intptr_t)instance);

  instance->instance.cb = &instance_cb;

  sns_cstruct_extn_init(&instance->extn, instance, sizeof(*instance), 2);
  state_idx = sns_cstruct_extn_setup_buffer(&instance->extn, state_size);
  instance->diag_header.cstruct_enxtn_idx = sns_cstruct_extn_setup_buffer(&instance->extn,
    diag_size);
  instance->diag_header.datatypes_cnt = sensors_cnt;

  instance->instance.state = sns_cstruct_extn_get_buffer(&instance->extn, state_idx);
  instance->instance.state->state_len = state_len;
  instance->sensor = fw_sensor;
  instance->unpublished_event = NULL;
  instance->inst_removing = SNS_INST_REMOVING_NOT_START;

  sns_isafe_list_init(&instance->client_req_lists);
  sns_isafe_list_init(&instance->data_streams);
  sns_isafe_list_item_init(&instance->list_entry, instance);

  sns_isafe_list_iter_init(&iter, &fw_sensor->sensor_instances, false);
  sns_isafe_list_iter_insert(&iter, &instance->list_entry, true);

  instance->island_operation = island_instance ?
    SNS_ISLAND_STATE_IN_ISLAND : SNS_ISLAND_STATE_NOT_IN_ISLAND;

  for(uint8_t i = 0; i < ARR_SIZE(state_init); i++)
    if(NULL != state_init[i].init)
      state_init[i].init(instance, fw_sensor);

  if(NULL != sensor->instance_api)
  {
    if(!sns_island_is_island_ptr((intptr_t)sensor->instance_api->init))
      SNS_ISLAND_EXIT();

    rv = sensor->instance_api->init(
          (sns_sensor_instance*)instance, sensor->state);

    if(SNS_RC_SUCCESS != rv)
    {
      SNS_PRINTF(LOW, sns_fw_printf, "instance_api->init error %i", rv);
      for(size_t i = 0; i < ARR_SIZE(state_init); i++)
        if(NULL != state_init[i].deinit)
          state_init[i].deinit(instance);
      sns_isafe_list_item_remove(&instance->list_entry);
      sns_free(instance);
      instance = NULL;
    }
    else
    {
      SNS_PRINTF(LOW, sns_fw_printf, "Created new Sensor Instance " SNS_DIAG_PTR
          " from Sensor " SNS_DIAG_PTR " with Island operation: %d",
          instance, sensor, instance->island_operation);
    }
  }

  return (sns_sensor_instance*)instance;
}

SNS_SECTION(".text.sns") void
sns_sensor_instance_delete(sns_fw_sensor_instance *this)
{
  sns_isafe_list_iter *iter;
  SNS_PRINTF(MED, sns_fw_printf, "sns_sensor_instance_delete " SNS_DIAG_PTR, this);

  iter = &this->sensor->instances_iter;
  if((void*)this == sns_isafe_list_iter_get_curr_data(iter))
  {
    sns_isafe_list_iter_remove(iter);
    sns_isafe_list_iter_return(iter);
  }
  else
  {
    sns_isafe_list_item_remove(&this->list_entry);
  }

  sns_sensor_delete(this->sensor);
  sns_free(this);
}

SNS_SECTION(".text.sns") void
sns_sensor_instance_deinit(sns_sensor_instance *instance)
{
  sns_fw_sensor_instance *this = (sns_fw_sensor_instance*)instance;
  sns_isafe_list_iter iter;
  sns_sensor *sensor = &this->sensor->sensor;

  SNS_PRINTF(MED, sns_fw_printf, "sns_sensor_instance_deinit " SNS_DIAG_PTR, this);
  this->inst_removing = SNS_INST_REMOVING_START;

  if(NULL != sensor->instance_api && NULL != sensor->instance_api->deinit)
  {
    if(!sns_island_is_island_ptr((intptr_t)sensor->instance_api->deinit))
      SNS_ISLAND_EXIT();
    sensor->instance_api->deinit((sns_sensor_instance*)this);
  }

  // In most cases, the Instance will be removed once it has no clients, but
  // upon an error, we need to send the appropriate event to those that remain
  sns_event_service_publish_error(this, SNS_RC_INVALID_STATE);

  // Data streams (and their associated client requests) are freed by their
  // client; here we simply remove references to them from this source instance
  sns_isafe_list_iter_init(&iter, &this->client_req_lists, true);
  while(NULL != sns_isafe_list_iter_curr(&iter))
  {
    sns_client_req_list *req_list = (sns_client_req_list*)
      sns_isafe_list_iter_get_curr_data(&iter);
    sns_isafe_list_iter temp;

    sns_isafe_list_iter_init(&temp, &req_list->client_requests, true);
    while(NULL != sns_isafe_list_iter_curr(&temp))
      sns_isafe_list_iter_remove(&temp);

    sns_isafe_list_iter_remove(&iter);
    sns_free(req_list);
  }

  for(size_t i = 0; i < ARR_SIZE(state_init); i++)
    if(NULL != state_init[i].deinit)
      state_init[i].deinit(this);

  sns_isafe_list_iter_init(&iter, &this->data_streams, true);
  if(0 == sns_isafe_list_iter_len(&iter))
  {
    sns_sensor_instance_delete(this);
  }
  else
  {
    for(; NULL != sns_isafe_list_iter_curr(&iter);
        sns_isafe_list_iter_advance(&iter))
    {
      struct sns_fw_data_stream *stream = (struct sns_fw_data_stream*)
        sns_isafe_list_iter_get_curr_data(&iter);
      this->inst_removing = SNS_INST_REMOVING_WAITING;
      sns_data_stream_deinit(stream);
    }
  }
}

SNS_SECTION(".text.sns") int32_t
sns_instance_client_cnt(sns_fw_sensor_instance *this,
    sns_sensor_uid const *sensor_uid)
{
  sns_isafe_list_iter iter;
  int32_t rv = 0;

  for(sns_isafe_list_iter_init(&iter, &this->client_req_lists, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_client_req_list *req_list = (sns_client_req_list*)
      sns_isafe_list_iter_get_curr_data(&iter);
    sns_sensor_uid const *suid = req_list->suid;

    if(NULL == sensor_uid || sns_sensor_uid_compare(sensor_uid, suid))
    {
      sns_isafe_list_iter req_iter;
      sns_isafe_list_iter_init(&req_iter, &req_list->client_requests, true);
      rv += sns_isafe_list_iter_len(&req_iter);
      if(NULL != sensor_uid)
      {
        break;
      }
    }
  }

  return rv;
}

SNS_SECTION(".text.sns") bool
sns_sensor_instance_set_island(sns_fw_sensor_instance *instance, bool enabled)
{
  bool rv = false;

  if(!enabled)
  {
    if(SNS_ISLAND_STATE_IN_ISLAND == instance->island_operation)
      instance->island_operation = SNS_ISLAND_STATE_ISLAND_DISABLED;
    rv = true;
  }
  else
  {
    if(SNS_ISLAND_STATE_ISLAND_DISABLED == instance->island_operation)
    {
      rv = sns_instance_streams_in_island((sns_sensor_instance*)instance);

      if(rv)
        instance->island_operation = SNS_ISLAND_STATE_IN_ISLAND;
    }
  }

  return rv;
}
