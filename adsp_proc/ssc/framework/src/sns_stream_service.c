/*=============================================================================
  @file sns_stream_service.c

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Id: //components/rel/ssc.slpi/3.2/framework/src/sns_stream_service.c#10 $
  $DateTime: 2018/08/07 13:34:15 $
  $Change: 16826239 $
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdlib.h>
#include "sns_assert.h"
#include "sns_diag_print.h"
#include "sns_fw.pb.h"
#include "sns_fw_attribute_service.h"
#include "sns_fw_data_stream.h"
#include "sns_fw_diag_service.h"
#include "sns_fw_event_service.h"
#include "sns_fw_log.h"
#include "sns_fw_request.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_event.h"
#include "sns_fw_sensor_instance.h"
#include "sns_fw_stream_service.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_pb_util.h"
#include "sns_printf_int.h"
#include "sns_profiler.h"
#include "sns_rc.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_std.pb.h"
#include "sns_stream_service.h"
#include "sns_thread_manager.h"
#include "sns_types.h"
// PEND: Remove this when DISABLE REQ is deprecated.
#include "sns_client.pb.h"

/*=============================================================================
  Macros and constants
  ===========================================================================*/
// Length of the largest request message that will be stored within island memory
#define SNS_STREAM_MAX_ISLAND_REQ_LEN 150

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * Some messages will be handled specially by the Framework; these messages
 * are stored in the msg_handlers table below, using this format.
 */
typedef struct sns_msg_handler
{
  uint32_t msg_id;
  sns_rc (*handle_req)(sns_sensor*, sns_fw_request*);
} sns_msg_handler;

/*=============================================================================
  Static Data Definitions
  ===========================================================================*/

static sns_fw_stream_service stream_service SNS_SECTION(".data.sns");
static sns_stream_service_api stream_service_api SNS_SECTION(".data.sns");
static const uint16_t version SNS_SECTION(".rodata.sns") = 1;

/*=============================================================================
  Public Data Definitions
  ===========================================================================*/

/* Declared and commented in sns_sensor.h */
sns_sensor_instance sns_instance_no_error;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Add integrity checksum to a request
 *
 * @param[i] request Request to which checksum will be added
 */
SNS_SECTION(".text.sns") static void
request_add_checksum(sns_fw_request *request)
{
  if(NULL != request)
  {
    request->sanity = (SNS_STREAM_SERVICE_REQUEST_SANITY ^
                       (uint32_t)request->request.request_len ^
                       (uint32_t)(uintptr_t)request);
  }
}

/**
 * Determine whether the given sensor matches the search query.
 *
 * @return false to stop search; true to continue
 */
SNS_SECTION(".text.sns") static bool
sns_find_sensor(struct sns_attribute_info *attr_info, void *arg)
{
  sns_find_sensor_arg *find_arg = (sns_find_sensor_arg*)arg;
  sns_sensor_uid suid = sns_attr_info_get_suid(attr_info);

  // PEND: Don't use sensors whose "removing" is true
  if(sns_sensor_uid_compare(find_arg->suid, &suid))
  {
    find_arg->sensor = sns_attr_info_get_sensor(attr_info);
    find_arg->priority = sns_attr_info_get_priority(attr_info);
    find_arg->available = sns_attr_info_get_available(attr_info);
    return false;
  }

  return true;
}

/**
 * Log a message describing a new data stream which was just successfully
 * created.
 *
 * @param[i] stream Newly created data stream
 */
SNS_SECTION(".text.sns") static void
log_stream_create(sns_fw_data_stream const *stream)
{
  if(SNS_ISLAND_STATE_IN_ISLAND != sns_island_get_island_state())
  {
    char *src_name = NULL, *dst_name = NULL;

    if(NULL != stream->dst_instance)
    {
      if(NULL != stream->dst_instance->sensor->diag_config.config)
        dst_name = stream->dst_instance->sensor->diag_config.config->datatype;
      if(NULL != stream->data_source->diag_config.config)
        src_name = stream->data_source->diag_config.config->datatype;
      if(NULL != src_name && NULL != dst_name)
      {
        SNS_SPRINTF(MED, sns_fw_printf, "Created data stream to Sensor '%s' ("SNS_DIAG_PTR
            ") from Instance '%s' ("SNS_DIAG_PTR"): "SNS_DIAG_PTR,
            src_name, stream->data_source, dst_name, stream->dst_instance, stream);
      }
    }
    else if(NULL != stream->dst_sensor)
    {
      if(NULL != stream->dst_sensor->diag_config.config)
        dst_name = stream->dst_sensor->diag_config.config->datatype;
      if(NULL != stream->data_source->diag_config.config)
        src_name = stream->data_source->diag_config.config->datatype;
      if(NULL != src_name && NULL != dst_name)
      {
        SNS_SPRINTF(MED, sns_fw_printf, "Created data stream to Sensor '%s' ("SNS_DIAG_PTR
            ") from Sensor '%s' ("SNS_DIAG_PTR"): "SNS_DIAG_PTR,
            src_name, stream->data_source, dst_name, stream->dst_sensor, stream);
      }
    }
  }
}

/* See sns_stream_service_api::create_sensor_stream */
SNS_SECTION(".text.sns") static sns_rc
create_sensor_stream(sns_stream_service *this,
    sns_sensor *dst_sensor, sns_sensor_uid sensor_uid,
    struct sns_data_stream **data_stream)
{
  UNUSED_VAR(this);
  SNS_ISLAND_EXIT();
  SNS_ASSERT(NULL != dst_sensor);
  SNS_PROFILE(SNS_STREAM_SERVICE_CREATE_SENSOR_STREAM_START, 0);

  sns_rc rv = SNS_RC_NOT_AVAILABLE;
  sns_find_sensor_arg sensor = (sns_find_sensor_arg)
      { .suid = &sensor_uid, .sensor = NULL };
  sns_attr_svc_sensor_foreach(&sns_find_sensor, &sensor);

  *data_stream = NULL;
  if(NULL != sensor.sensor)
  {
    rv = data_stream_init(
        (sns_fw_sensor*)dst_sensor, NULL,
        &sensor, (sns_fw_data_stream**)data_stream);
  }

  if(NULL != *data_stream)
  {
    log_stream_create((sns_fw_data_stream*)*data_stream);
  }
  else
  {
    SNS_PRINTF(MED, sns_fw_printf,
        "Unable to create stream to Sensor "SNS_DIAG_PTR
        " from Sensor "SNS_DIAG_PTR, sensor.sensor, dst_sensor);
  }

  if(SNS_ISLAND_STATE_IN_ISLAND == ((sns_fw_sensor*)dst_sensor)->island_operation &&
     !sns_island_is_island_ptr((intptr_t)*data_stream))
  {
    sns_sensor_set_island((sns_fw_sensor*)dst_sensor, false);
    SNS_PRINTF(MED, sns_fw_printf, "Sensor " SNS_DIAG_PTR
        " marked as ISLAND_DISABLED", dst_sensor);
  }

  SNS_PROFILE(SNS_STREAM_SERVICE_CREATE_SENSOR_STREAM_END, 0);
  return rv;
}

/* See sns_stream_service_api::create_sensor_instance_stream */
SNS_SECTION(".text.sns") static sns_rc
create_sensor_instance_stream(sns_stream_service *this,
    sns_sensor_instance *dst_instance, sns_sensor_uid sensor_uid,
    struct sns_data_stream **data_stream)
{
  UNUSED_VAR(this);
  SNS_ASSERT(NULL != dst_instance);
  SNS_PROFILE(SNS_STREAM_SERVICE_CREATE_INSTANCE_STREAM_START, 0);
  SNS_ISLAND_EXIT();

  sns_rc rv = SNS_RC_NOT_AVAILABLE;
  sns_find_sensor_arg sensor = (sns_find_sensor_arg)
      { .suid = &sensor_uid, .sensor = NULL };
  sns_attr_svc_sensor_foreach(&sns_find_sensor, &sensor);

  *data_stream = NULL;
  if(NULL != sensor.sensor)
  {
    rv = data_stream_init(
        NULL, (sns_fw_sensor_instance*)dst_instance,
        &sensor, (sns_fw_data_stream**)data_stream);
  }

  if(NULL != *data_stream)
  {
    log_stream_create((sns_fw_data_stream*)*data_stream);
  }
  else
  {
    SNS_PRINTF(MED, sns_fw_printf,
        "Unable to create stream to Sensor "SNS_DIAG_PTR
        " from Instance "SNS_DIAG_PTR, sensor.sensor, dst_instance);
  }

  if(SNS_ISLAND_STATE_IN_ISLAND == ((sns_fw_sensor_instance *)dst_instance)->island_operation &&
     !sns_island_is_island_ptr((intptr_t)*data_stream))
  {
    sns_sensor_instance_set_island((sns_fw_sensor_instance*)dst_instance, false);
    SNS_PRINTF(MED, sns_fw_printf, "Instance " SNS_DIAG_PTR
        " ISLAND_DISABLED", dst_instance);
  }
  SNS_PROFILE(SNS_STREAM_SERVICE_CREATE_INSTANCE_STREAM_END, 0);
  return rv;
}

/* See sns_stream_service_api::remove_stream */
SNS_SECTION(".text.sns") static sns_rc
remove_stream(sns_stream_service *this, struct sns_data_stream *data_stream)
{
  UNUSED_VAR(this);
  sns_rc ret_val = SNS_RC_INVALID_VALUE;
  sns_fw_data_stream *stream = (sns_fw_data_stream*)data_stream;

  if(!sns_island_is_island_ptr((intptr_t)data_stream))
    SNS_ISLAND_EXIT();

  if(sns_data_stream_validate(stream))
  {
    sns_data_stream_deinit(stream);
    ret_val = SNS_RC_SUCCESS;
  }
  else
  {
    SNS_PRINTF(ERROR, sns_fw_printf,
        "Refusing to remove invalid stream 0x"SNS_DIAG_PTR, stream);
  }
  return ret_val;
}

/**
 * Handle a request from the client to destroy a data stream.
 *
 * Remove this client from the source Sensor, and remove all unprocessed
 * events, as the client will no longer be processing them.
 *
 * This request is handled asynchronously to avoid deadlock; a meta-event
 * will be sent back to the client (and handled by the Framework), to finalize
 * freeing the data stream.
 *
 * @note sensor::library::library_lock must be held
 *
 * @param[i] sensor Source sensor of this stream
 * @param[i] request Request message associated with a single data stream
 *
 * @return SNS_RC_SUCCESS
 */
SNS_SECTION(".text.sns") static sns_rc
handle_stream_destroy(sns_sensor *sensor, sns_fw_request *request)
{
  SNS_PROFILE(SNS_STREAM_SERVICE_HANDLE_STREAM_DESTROY_START, 1, request->stream);
  sns_fw_data_stream *stream = request->stream;
  sns_sensor_instance *bad_instance = NULL;
  sns_rc ret_val = SNS_RC_SUCCESS;
  SNS_PRINTF(MED, sns_fw_printf, "handle_stream_destroy " SNS_DIAG_PTR, stream);
  sns_osa_lock_acquire(stream->events_lock);
  stream->removing = SNS_DATA_STREAM_WAIT_RECEIPT;
  sns_osa_lock_release(stream->events_lock);

  if(NULL != stream->client_request)
  {
    if(!sns_island_is_island_ptr((intptr_t)sensor->sensor_api->set_client_request) ||
       (NULL != sensor->instance_api &&
       !sns_island_is_island_ptr((intptr_t)sensor->instance_api->set_client_config)))
    {
      SNS_ISLAND_EXIT();
    }
    sensor->sensor_api->set_client_request(sensor,
        (sns_request*)stream->client_request, NULL, true);

    bad_instance = sns_sensor_util_find_instance(sensor,
      (sns_request*)stream->client_request, sensor->sensor_api->get_sensor_uid(sensor));
    SNS_VERBOSE_ASSERT(NULL == bad_instance, "Instance did not remove request!");
  }

  sns_event_service_send_event(stream,
      SNS_FW_MSGID_SNS_DESTROY_COMPLETE_EVENT, 0, NULL);

  SNS_PROFILE(SNS_STREAM_SERVICE_HANDLE_STREAM_DESTROY_END, 0);
  return ret_val;
}

/**
 * Handle a non-specialized request, incoming to some Sensor.
 *
 * @note library::library_lock must be held
 *
 * @param[i] sensor Destination of the request
 * @param[i] pend_req Request sent to sensor
 *
 * @return
 * SNS_RC_SUCCESS
 */
SNS_SECTION(".text.sns") static sns_rc
handle_req(sns_sensor *sensor, sns_fw_request *pend_req)
{
  SNS_PROFILE(SNS_STREAM_SERVICE_HANDLE_REQ_START, 0);

  struct sns_fw_request *exist_request = pend_req->stream->client_request;
  sns_sensor_instance *instance = &sns_instance_no_error;
  sns_fw_data_stream *data_stream = (sns_fw_data_stream*)pend_req->stream;
  sns_time ts = sns_get_system_time();
  bool free_pend_req = false, log_inst_map = false;

  if(SNS_DATA_STREAM_AVAILABLE != data_stream->removing)
  {
    SNS_PRINTF(HIGH, sensor, "Unavailable stream "SNS_DIAG_PTR
        "; Sensor "SNS_DIAG_PTR, data_stream, sensor);
  }
  else
  {
    if(!sns_island_is_island_ptr((intptr_t)sensor->sensor_api->set_client_request) ||
       (NULL != sensor->instance_api &&
       !sns_island_is_island_ptr((intptr_t)sensor->instance_api->set_client_config))||
       !sns_island_is_island_ptr((intptr_t)exist_request))
    {
      SNS_ISLAND_EXIT();
    }

    SNS_ASSERT(NULL != sensor->sensor_api->set_client_request);

    sns_fw_log_sensor_api_req(sensor, pend_req, ts);

    instance = sensor->sensor_api->set_client_request(
        sensor, (sns_request*)exist_request, (sns_request*)pend_req, false);
  }

  if(&sns_instance_no_error == instance)
  {
    SNS_VERBOSE_ASSERT(!sns_isafe_list_item_present(&pend_req->list_entry),
        "Pending request on list");
    free_pend_req = true;
  }
  else if(NULL == instance)
  {
    sns_event_service_send_error_event(pend_req->stream,
        SNS_RC_NOT_SUPPORTED);

    SNS_VERBOSE_ASSERT(!sns_isafe_list_item_present(&pend_req->list_entry),
        "Pending request on list");
    free_pend_req = true;
  }
  else if(!sns_isafe_list_item_present(&pend_req->list_entry))
  {
    // If the new request did not replace the existing, simply free pend_req
    free_pend_req = true;
  }
  else
  {
    if(NULL != data_stream->client_request)
    {
      SNS_VERBOSE_ASSERT(!sns_isafe_list_item_present(
          &data_stream->client_request->list_entry),
          "Previous request on list");
      sns_free(data_stream->client_request);
    }
    data_stream->client_request = pend_req;

    sns_diag_sensor_instance_init((sns_fw_sensor_instance*)instance,
        (sns_fw_sensor*)sensor);

    log_inst_map = true;
  }

  if(log_inst_map)
    sns_fw_log_inst_map((sns_sensor_instance*)instance);

  if(free_pend_req)
    sns_free(pend_req);

  SNS_PROFILE(SNS_STREAM_SERVICE_HANDLE_REQ_END, 0);
  return SNS_RC_SUCCESS;
}

static sns_msg_handler msg_handlers[] SNS_SECTION(".data.sns") =
{
  { SNS_STD_MSGID_SNS_STD_ATTR_REQ, &sns_attr_svc_handle_req },
  { SNS_FW_MSGID_SNS_DESTROY_REQ, &handle_stream_destroy }
};

/**
 * Process a single pending request.
 *
 * See sns_thread_mgr_task_func().
 */
SNS_SECTION(".text.sns") static sns_rc
sns_stream_service_process(void *func_arg)
{
  SNS_PROFILE(SNS_STREAM_SERVICE_PROCESS_START, 0);
  sns_fw_request *pend_req = func_arg;
  sns_time ts = sns_get_system_time();
  uint32_t i;
  sns_sensor *sensor;

  if(!sns_island_is_island_ptr((intptr_t)pend_req) ||
     !sns_island_is_island_ptr((intptr_t)pend_req->stream) ||
      SNS_ISLAND_STATE_IN_ISLAND != pend_req->stream->island_operation)
    SNS_ISLAND_EXIT();

  sensor = &pend_req->stream->data_source->sensor;
  SNS_PRINTF(LOW, sns_fw_printf,
    "Process request for Sensor "SNS_DIAG_PTR" on "SNS_DIAG_PTR,
    sensor, pend_req->stream);

  for(i = 0; i < ARR_SIZE(msg_handlers); i++)
  {
    if(msg_handlers[i].msg_id == pend_req->request.message_id)
    {
      SNS_ISLAND_EXIT();
      sns_fw_log_sensor_api_req(sensor, pend_req, ts);
      msg_handlers[i].handle_req(sensor, pend_req);

      sns_free(pend_req);
      break;
    }
  }
  if(ARR_SIZE(msg_handlers) == i)
    handle_req(sensor, pend_req);

  SNS_PROFILE(SNS_STREAM_SERVICE_PROCESS_END, 0);
  return SNS_RC_SUCCESS;
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_fw_stream_service*
sns_stream_service_init(void)
{
  stream_service_api = (sns_stream_service_api)
  {
    .struct_len = sizeof(stream_service_api),
    .create_sensor_stream = &create_sensor_stream,
    .create_sensor_instance_stream = &create_sensor_instance_stream,
    .remove_stream = &remove_stream
  };

  stream_service.service.service.version = version;
  stream_service.service.service.type = SNS_STREAM_SERVICE;
  stream_service.service.api = &stream_service_api;

  return &stream_service;
}

SNS_SECTION(".text.sns") bool
sns_data_stream_validate_request(sns_fw_request *request)
{
  return (SNS_STREAM_SERVICE_REQUEST_SANITY ==
                     (request->sanity ^
                     (uint32_t)request->request.request_len ^
                     (uint32_t)(uintptr_t)request));
}

SNS_SECTION(".text.sns") void
sns_stream_service_add_request(struct sns_fw_data_stream *stream,
    struct sns_request *request)
{
  SNS_PROFILE(SNS_STREAM_SERVICE_ADD_REQ_START, 0);
  sns_fw_request *pending_req = NULL;
  size_t pending_req_size = ALIGN_8(sizeof(*pending_req)) + request->request_len;
  bool island_req;

  if(!sns_island_is_island_ptr((intptr_t)stream) ||
     SNS_ISLAND_STATE_IN_ISLAND != stream->island_operation)
    SNS_ISLAND_EXIT();

  // If the source supports island mode, attempt to provide this request
  // in island mode so that the source can iterate over all client requests
  // in island mode.
  island_req = SNS_ISLAND_STATE_NOT_IN_ISLAND != stream->island_operation &&
               request->request_len < SNS_STREAM_MAX_ISLAND_REQ_LEN;

  if(island_req)
    pending_req = sns_malloc(SNS_HEAP_ISLAND, pending_req_size);
  if(NULL == pending_req)
  {
    SNS_ISLAND_EXIT();
    pending_req = sns_malloc(SNS_HEAP_MAIN, pending_req_size);
  }

  SNS_PRINTF(MED, sns_fw_printf, "Add request " SNS_DIAG_PTR
      " on stream " SNS_DIAG_PTR " (length %i; ID %i)",
      pending_req, stream, (int)request->request_len, request->message_id);

  SNS_ASSERT(NULL != pending_req);
  pending_req->request.request =
    (void *)(((uint8_t*)pending_req) + ALIGN_8(sizeof(*pending_req)));

  pending_req->request.request_len = request->request_len;
  pending_req->request.message_id = request->message_id;
  pending_req->stream = (sns_fw_data_stream*)stream;
  sns_memscpy(pending_req->request.request,
      pending_req->request.request_len, request->request, request->request_len);

  sns_isafe_list_item_init(&pending_req->list_entry, pending_req);
  request_add_checksum(pending_req);

  sns_thread_manager_add(stream->data_source->library,
      &sns_stream_service_process, pending_req, stream->req_priority);

  SNS_PROFILE(SNS_STREAM_SERVICE_ADD_REQ_END, 0);
}
