/*=============================================================================
  @file sns_ext_svc_sensor.c

  The External Service sensor.  Provides a bridge for SEE Sensors to external
  QMI services.

  All QMI callbacks are copied to to a pending events queue, where the
  external service thread is responsible for publishing the events to the
  event service.

  Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "pb_encode.h"
#include "sns_assert.h"
#include "sns_attribute_util.h"
#include "sns_event_service.h"
#include "sns_ext_svc.h"
#include "sns_fw_sensor.h"
#include "sns_island.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service_manager.h"
#include "sns_thread_prio.h"
#include "sns_time.h"
#include "sns_types.h"

#include "location_service_v02.h"
#include "sns_ext_svc.pb.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/

// Pending events received from QMI but not yet sent to the client
typedef struct sns_ext_svc_event
{
  sns_isafe_list_item list_entry;
  // Sensor Instance from which to send this event
  sns_sensor_instance *instance;
  // Message ID, as from sns_ext_svc_msgid
  uint32_t msg_id;
  // Length of payload buffer
  uint32_t payload_len;
  // PB-encoded message; sns_ext_svc_resp or sns_ext_svc_ind
  uint8_t payload[];
} sns_ext_svc_event;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* See sns_sensor_api::get_sensor_uid */
static sns_sensor_uid const*
get_sensor_uid(sns_sensor const *this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = {
    .sensor_uid = {
      0xbb, 0x35, 0x79, 0xaf, 0xb1, 0x24, 0xf7, 0xd2,
      0x13, 0x85, 0xc4, 0x50, 0x9f, 0x74, 0x4e, 0x8d,
    }
  };
  return &sensor_uid;
}

/**
 * Thread passes events received from QMI on the QMI thread to SEE, where it
 * can be delivered to Sensor/Instance client.
 */
static void
event_task(void *arg)
{
  sns_sensor *const this = (sns_sensor*)arg;
  sns_osa_thread_active();
  sns_ext_svc_sensor_state *sensor_state =
    (sns_ext_svc_sensor_state*)this->state->state;
  sns_service_manager *manager =
    this->cb->get_service_manager(this);
  sns_event_service *event_service =
    (sns_event_service*)manager->get_service(manager, SNS_EVENT_SERVICE);
  sns_island_client_handle island_client = sns_island_aggregator_register_client("EXT_SVC_SENSOR");
  for(;;)
  {
    sns_isafe_list_iter iter;
    sns_island_unblock(island_client);
    sns_osa_thread_idle();
    sns_osa_sem_wait(sensor_state->events_sem);
    sns_osa_thread_active();
    sns_island_block(island_client);

    // Note: library lock must be acquired prior to events_lock
    sns_osa_lock_acquire(((sns_fw_sensor*)this)->library->library_lock);
    sns_osa_lock_acquire(sensor_state->events_lock);

    for(sns_isafe_list_iter_init(&iter, &sensor_state->events, false);
        NULL != sns_isafe_list_iter_curr(&iter);)
    {
      sns_ext_svc_event *svc_event = (sns_ext_svc_event*)
        sns_isafe_list_iter_get_curr_data(&iter);
      sns_sensor_instance *instance = NULL;

      for(instance = this->cb->get_sensor_instance(this, true);
          NULL != instance;
          instance = this->cb->get_sensor_instance(this, false))
      {
        if(instance == svc_event->instance)
        {
          sns_sensor_event *event = event_service->api->alloc_event(
              event_service, svc_event->instance, svc_event->payload_len);

          event->event_len = svc_event->payload_len;
          event->message_id = svc_event->msg_id;
          event->timestamp = sns_get_system_time();

          sns_memscpy(event->event, event->event_len,
              svc_event->payload, svc_event->payload_len);

          event_service->api->publish_event(event_service,
            svc_event->instance, event, get_sensor_uid(this));

          break;
        }
      }

      sns_isafe_list_iter_remove(&iter);
      sns_free(svc_event);
    }

    sns_osa_lock_release(sensor_state->events_lock);
    sns_osa_lock_release(((sns_fw_sensor*)this)->library->library_lock);
  }
}

/**
 * Let the SEE know that this Sensor is now available for clients.
 */
static void
publish_available(sns_sensor *const this)
{
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = true;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
  }
}

/**
 * Publish all attributes for this Sensor.
 */
static void
publish_attributes(sns_sensor *const this)
{
  {
    char const name[] = "External Service";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = "ext_svc";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = type, .buf_len = sizeof(type) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
  }
  {
    char const vendor[] = "qualcomm";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = vendor, .buf_len = sizeof(vendor) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VENDOR, &value, 1, false);
  }
  {
    char const proto_files[] = "sns_ext_svc.proto";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = proto_files, .buf_len = sizeof(proto_files) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_API, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = 1;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, true);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint     = SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE;
    sns_publish_attribute(
      this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE, &value, 1, true);
  }
}

/* See sns_sensor_api::notify_event;
 *
 * This Sensor does not register for any data streams, and hence will never
 * receive any event notifications. */
static sns_rc
notify_event(sns_sensor* const this)
{
  UNUSED_VAR(this);
  return SNS_RC_SUCCESS;
}

/* See sns_sensor_api::init */
static sns_rc
init(sns_sensor* const this)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_osa_thread_attr thread_attr;
  sns_osa_sem_attr sem_attr;
  sns_osa_lock_attr lock_attr;
  sns_ext_svc_sensor_state *sensor_state =
    (sns_ext_svc_sensor_state*)this->state->state;

  publish_attributes(this);

  sns_isafe_list_init(&sensor_state->services);
  sns_isafe_list_init(&sensor_state->events);

  rc |= sns_osa_lock_attr_init(&lock_attr);
  rc |= sns_osa_sem_attr_init(&sem_attr);
  rc |= sns_osa_thread_attr_init(&thread_attr);
  rc |= sns_osa_thread_attr_set_name(&thread_attr, "SNS_EXT_SVC");
  rc |= sns_osa_thread_attr_set_stack(&thread_attr, (uintptr_t)NULL, 4096);
  rc |= sns_osa_thread_attr_set_memory_partition(&thread_attr, SNS_OSA_MEM_TYPE_NORMAL);
  rc |= sns_osa_thread_attr_set_priority(&thread_attr, SNS_EXT_SVC_THREAD_PRIO);
  if(SNS_RC_SUCCESS != rc)
  {
    SNS_PRINTF(ERROR, this, "Error initializing attributes %i", rc);
  }
  else
  {
    rc |= sns_osa_lock_create(&lock_attr, &sensor_state->events_lock);
    rc |= sns_osa_sem_create(&sem_attr, &sensor_state->events_sem);
    rc |= sns_osa_thread_create(&event_task, this, &thread_attr,
          &sensor_state->events_thread);
    if(SNS_RC_SUCCESS != rc)
    {
      SNS_PRINTF(ERROR, this,
          "Error creating semaphore/lock/thread %i", rc);

      if(NULL != sensor_state->events_thread)
        sns_osa_thread_delete(sensor_state->events_thread);
      if(NULL != sensor_state->events_lock)
        sns_osa_lock_delete(sensor_state->events_lock);
      if(NULL != sensor_state->events_sem)
        sns_osa_sem_delete(sensor_state->events_sem);
    }
    else
    {
      sns_ext_svc_add(&sensor_state->services, SNS_EXT_SVC_LOCATION,
          loc_get_service_object_v02());

      publish_available(this);
    }
  }

  return SNS_RC_SUCCESS == rc ? SNS_RC_SUCCESS : SNS_RC_INVALID_STATE;
}

/* See sns_sensor_api::deinit */
static sns_rc
deinit(sns_sensor* const this)
{
  sns_ext_svc_sensor_state *sensor_state =
    (sns_ext_svc_sensor_state*)this->state->state;

  sns_osa_thread_delete(sensor_state->events_thread);
  sns_osa_sem_delete(sensor_state->events_sem);
  sns_osa_lock_delete(sensor_state->events_lock);

  return SNS_RC_SUCCESS;
}

/* See sns_sensor_api::set_client_request */
static sns_sensor_instance*
set_client_request(sns_sensor* const this, sns_request const *curr_req,
    sns_request const *new_req, bool remove)
{
  sns_sensor_instance *rv_inst = sns_sensor_util_find_instance(
      this, curr_req, this->sensor_api->get_sensor_uid(this));

  if(NULL != rv_inst)
  {
    rv_inst->cb->remove_client_request(rv_inst, curr_req);
  }
  else
  {
    rv_inst = this->cb->create_instance(this, sizeof(sns_ext_svc_inst_state));
  }

  if(remove)
  {
    this->cb->remove_instance(rv_inst);
  }
  else
  {
    rv_inst->cb->add_client_request(rv_inst, new_req);
    this->instance_api->set_client_config(rv_inst, new_req);
  }

  return rv_inst;
}

/*=============================================================================
  SEE Sensor API Definition
  ===========================================================================*/

sns_sensor_api sns_ext_svc_sensor_api =
{
  .struct_len = sizeof(sns_sensor_api),
  .init = &init,
  .deinit = &deinit,
  .get_sensor_uid = &get_sensor_uid,
  .set_client_request = &set_client_request,
  .notify_event = &notify_event,
};

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

void
add_pending_event(sns_sensor_instance *instance, pb_field_t const *fields,
    void const *c_struct, uint32_t msg_id)
{
  sns_ext_svc_inst_state *inst_state = (sns_ext_svc_inst_state*)instance->state->state;
  sns_ext_svc_sensor_state *sensor_state = inst_state->sensor_state;
  size_t encoded_len;

  if(!pb_get_encoded_size(&encoded_len, fields, c_struct))
  {
    SNS_INST_PRINTF(LOW, instance, "pb_get_encoded_size failed");
  }
  else
  {
    sns_ext_svc_event *event;

    event = sns_malloc(SNS_HEAP_MAIN, sizeof(*event) + encoded_len);
    SNS_ASSERT(NULL != event);

    event->payload_len = encoded_len;
    event->msg_id = msg_id;
    event->instance = instance;

    pb_ostream_t stream =
      pb_ostream_from_buffer((pb_byte_t*)event->payload, event->payload_len);

    if(!pb_encode(&stream, fields, c_struct))
    {
      SNS_INST_PRINTF(ERROR, instance, "Error encoding event");
      sns_free(event);
    }
    else
    {
      sns_isafe_list_iter iter;

      sns_isafe_list_item_init(&event->list_entry, event);
      sns_osa_lock_acquire(sensor_state->events_lock);
      sns_isafe_list_iter_init(&iter, &sensor_state->events, false);
      sns_isafe_list_iter_insert(&iter, &event->list_entry, true);
      sns_osa_lock_release(sensor_state->events_lock);
      sns_osa_sem_post(sensor_state->events_sem);
    }
  }
}

void
remove_pending_events(sns_sensor_instance const *instance)
{
  sns_ext_svc_inst_state *inst_state = (sns_ext_svc_inst_state*)instance->state->state;
  sns_ext_svc_sensor_state *sensor_state = inst_state->sensor_state;
  sns_isafe_list_iter iter;

  sns_osa_lock_acquire(sensor_state->events_lock);
  for(sns_isafe_list_iter_init(&iter, &sensor_state->events, false);
      NULL != sns_isafe_list_iter_curr(&iter);)
  {
    sns_ext_svc_event *event = (sns_ext_svc_event*)
      sns_isafe_list_iter_get_curr_data(&iter);

    if(instance == event->instance)
    {
      sns_isafe_list_iter_remove(&iter);
      sns_free(event);
    }
    else
    {
      sns_isafe_list_iter_advance(&iter);
    }
  }
  sns_osa_lock_release(sensor_state->events_lock);
}
