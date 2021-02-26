/*=============================================================================
  @file sns_oem1_sensor.c

  The oem1 virtual Sensor implementation

  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_attribute_util.h"
#include "sns_diag_service.h"
#include "sns_oem1.pb.h"
#include "sns_oem1_sensor.h"
#include "sns_oem1_sensor_instance.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_suid.pb.h"
#include "sns_types.h"

// API used by registry
#include "sns_registry.pb.h"
/*=============================================================================
  Function Definitions
  ===========================================================================*/
/**
 * Initialize attributes to their default state.  They may/will be updated
 * within oem1_notify.
 */
static void
publish_attributes(sns_sensor *const this)
{
  {
    char const name[] = "oem1";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = "oem1";
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
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR};
    char const proto1[] = "sns_oem1.proto";
    char const proto2[] = "sns_std_sensor.proto";
    values[0].str.funcs.encode = pb_encode_string_cb;
    values[0].str.arg = &((pb_buffer_arg)
        { .buf = proto1, .buf_len = sizeof(proto1) });
    values[1].str.funcs.encode = pb_encode_string_cb;
    values[1].str.arg = &((pb_buffer_arg)
        { .buf = proto2, .buf_len = sizeof(proto2) });
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_API,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = 1;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, true);
  }
#ifdef OEM1_SUPPORT_EVENT_TYPE
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint     = SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE, &value, 1, true);
  }
#else
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint     = SNS_STD_SENSOR_STREAM_TYPE_STREAMING;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE, &value, 1, false);
  }
  {
    //publishing the rate attributes
    sns_std_attr_value_data values[] = {SNS_ATTR};
    values[0].has_flt = true;
    values[0].flt = OEM1_MINUMUM_SAMPLE_RATE;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_RATES, values, ARR_SIZE(values), false);
  }
#endif
}

static void
publish_available(sns_sensor *const this)
{
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  value.has_boolean = true;
  value.boolean = true;
  sns_publish_attribute(
      this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
}

/* See sns_sensor::init */
sns_rc
sns_oem1_init(sns_sensor *const this)
{
  sns_oem1_sensor_state *state = (sns_oem1_sensor_state*)this->state->state;
  struct sns_service_manager *smgr = this->cb->get_service_manager(this);
  float data[3];

  state->diag_service = (sns_diag_service*)
        smgr->get_service(smgr, SNS_DIAG_SERVICE);

  // set default output value corresponding to OEM1_FACING_DOWN to 50. will 
  // rewrite if registry sensor is available
  state->config.down_value = 50.0;
  state->first_pass = true;

  // determine encoded output event size
  state->config.encoded_data_event_len =
      pb_get_encoded_size_sensor_stream_event(data, 3);

  SNS_SUID_LOOKUP_INIT(state->suid_lookup_data, NULL);
  sns_suid_lookup_add(this, &state->suid_lookup_data, "accel");
  sns_suid_lookup_add(this, &state->suid_lookup_data, "registry");
#ifndef OEM1_SUPPORT_DIRECT_SENSOR_REQUEST
  sns_suid_lookup_add(this, &state->suid_lookup_data, "resampler");
#endif

  publish_attributes(this);
  SNS_PRINTF(MED, this, "OEM1 init success and attributes published");
  return SNS_RC_SUCCESS;
}

/* See sns_sensor::deinit */
sns_rc
sns_oem1_deinit(sns_sensor *const this)
{
  UNUSED_VAR(this);
  SNS_PRINTF(MED, this, "sns_oem1_deinit");
  return SNS_RC_SUCCESS;
}


#ifdef OEM1_SUPPORT_REGISTRY
static sns_rc
sns_oem1_sensor_send_error(sns_sensor *const this,
    sns_std_error error)
{
  sns_sensor_instance *instance;
  sns_rc ret_val = SNS_RC_SUCCESS;
  for(instance = this->cb->get_sensor_instance(this, true);
      NULL != instance;
      instance = this->cb->get_sensor_instance(this, false))
  {
    sns_std_error_event error_event;
    error_event.error = error;
    if(!pb_send_event(instance,
        sns_std_error_event_fields,
        &error_event,
        sns_get_system_time(),
        SNS_STD_MSGID_SNS_STD_ERROR_EVENT,
        this->sensor_api->get_sensor_uid(this)))
    {
      ret_val = SNS_RC_FAILED;
    }
  }
  return ret_val;
}

static void
sns_oem1_registry_req(sns_sensor *const this)
{
  sns_oem1_sensor_state *state =
      (sns_oem1_sensor_state*)this->state->state;
  sns_service_manager* service_mgr = this->cb->get_service_manager(this);
  sns_stream_service* stream_service =
      (sns_stream_service*)service_mgr->get_service(service_mgr,
          SNS_STREAM_SERVICE);
  uint8_t buffer[100];
  size_t encoded_len;
  sns_sensor_uid registry_suid;
  sns_suid_lookup_get(&state->suid_lookup_data, "registry", &registry_suid);

  SNS_PRINTF(LOW, this, "sns_oem1_registry_req");

  //Create a data stream to Registry
  stream_service->api->create_sensor_stream(stream_service,
      this,
      registry_suid,
      &state->registry_stream);
  //Send a request to Registry
  sns_registry_read_req sns_oem1_registry;
  pb_buffer_arg data = (pb_buffer_arg)
      {
        .buf = "sns_oem1",
        .buf_len = (strlen("sns_oem1") + 1)
      };
  sns_oem1_registry.name.arg = &data;
  sns_oem1_registry.name.funcs.encode = pb_encode_string_cb;
  sns_std_request registry_std_req = sns_std_request_init_default;
  encoded_len = pb_encode_request(buffer,
      sizeof(buffer),
      &sns_oem1_registry,
      sns_registry_read_req_fields,
      &registry_std_req);
  if(0 < encoded_len && NULL != state->registry_stream )
  {
    sns_request request = (sns_request){
      .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ,
          .request_len = encoded_len, .request = buffer };
    state->registry_stream->api->send_request(
        state->registry_stream, &request);
  }
}

static bool
sns_oem1_parse_registry(pb_istream_t *stream,
    const pb_field_t *field,
    void **arg)
{
  UNUSED_VAR(field);
  sns_registry_data_item reg_item;
  pb_buffer_arg item_name = {0,0};
  reg_item.name.arg = &item_name;
  reg_item.name.funcs.decode = pb_decode_string_cb;
  bool rv = pb_decode(stream, sns_registry_data_item_fields, &reg_item);
  sns_oem1_config *config = (sns_oem1_config *)*arg;
  if(0 == strncmp((char*)item_name.buf, "param1", item_name.buf_len))
  {
    if (reg_item.has_flt)
    {
      config->down_value = reg_item.flt;
    }
    else
    {
      rv = false;
    }
  }

  return rv;
}

static sns_rc
handle_oem1_registry_event(sns_sensor *const this)
{
  sns_rc rv = SNS_RC_SUCCESS;
  sns_sensor_event *event;
  sns_oem1_sensor_state *state =
      (sns_oem1_sensor_state*)this->state->state;

  SNS_PRINTF(LOW, this, "OEM1 - handling registry event");

  for(event = state->registry_stream->api->peek_input(state->registry_stream);
      NULL != event;
      event = (NULL == state->registry_stream)?NULL:
          state->registry_stream->api->get_next_input(state->registry_stream))
  {

    if(SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
    {
      pb_istream_t stream = pb_istream_from_buffer((void*)event->event,
          event->event_len);
      sns_registry_read_event read_event = sns_registry_read_event_init_default;
      pb_buffer_arg group_name = {0,0};
      read_event.name.arg = &group_name;
      read_event.name.funcs.decode = pb_decode_string_cb;
      read_event.data.items.arg = &state->config;
      read_event.data.items.funcs.decode = sns_oem1_parse_registry;

      if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
      {
        SNS_PRINTF(ERROR, this, "Error decoding registry event");
      }
      else if(0 == strncmp((char*)group_name.buf, "sns_oem1", group_name.buf_len))
      {
        SNS_PRINTF(LOW, this, "OEM1 - Remove registry stream");
        sns_sensor_util_remove_sensor_stream(this,&state->registry_stream);
        state->registry_stream = NULL;
      }
      else
      {
        SNS_PRINTF(HIGH, this, "OEM1 - config from registry failed");
      }
    }
    else if(SNS_STD_MSGID_SNS_STD_ERROR_EVENT == event->message_id)
    {
      SNS_PRINTF(ERROR, this, "Error event received %%d", event->message_id);
      rv = sns_oem1_sensor_send_error(this, SNS_STD_ERROR_INVALID_STATE);
      if(SNS_RC_SUCCESS != rv)
      {
        SNS_PRINTF(ERROR, this, "Unable to send error event");
      }
      // Consume all remaining events
      while(NULL != state->registry_stream->api->get_next_input(state->registry_stream));
      rv = SNS_RC_INVALID_STATE;
      break;
    }
    else
    {
      SNS_PRINTF(ERROR, this, "Unknown event received %d", event->message_id);
    }
  }

  return rv;
}
#endif

/* See sns_sensor::notify_event */
sns_rc
sns_oem1_notify_event(sns_sensor *const this)
{
  sns_oem1_sensor_state *state = (sns_oem1_sensor_state*)this->state->state;
  SNS_PRINTF(LOW, this, "sns_oem1_notify_event");
  sns_suid_lookup_handle(this, &state->suid_lookup_data);

#ifdef OEM1_SUPPORT_REGISTRY
  if(NULL != state->registry_stream)
  {
    handle_oem1_registry_event(this);
  }
#endif

  if(sns_suid_lookup_complete(&state->suid_lookup_data))
  {
#ifdef OEM1_SUPPORT_REGISTRY
    if(state->first_pass == true){
      state->first_pass = false;
      sns_oem1_registry_req(this);
    }
#endif
    publish_available(this);
    sns_suid_lookup_deinit(this, &state->suid_lookup_data);
  }
  return SNS_RC_SUCCESS;
}

#ifndef OEM1_SUPPORT_EVENT_TYPE
/**
 * See sns_sensor_util_instance_match_cb
 */
static bool
find_instance_match(struct sns_request const *request,
    sns_sensor_instance const *instance)
{
  sns_oem1_inst_state const *inst_state =
      (sns_oem1_inst_state*)instance->state->state;
  sns_std_sensor_config *oem1_req = (sns_std_sensor_config*)request->request;
  return (&inst_state->client_config.sample_rate == &oem1_req->sample_rate);
}
#endif

/* See sns_sensor::set_client_request */
sns_sensor_instance*
sns_oem1_set_client_request(sns_sensor *const this,
    sns_request const *curr_req,
    sns_request const *new_req,
    bool remove)
{
  sns_sensor_instance *curr_inst =
      sns_sensor_util_find_instance(
          this,
          curr_req,
          this->sensor_api->get_sensor_uid(this));
  sns_sensor_instance *rv_inst = NULL;
  sns_sensor_instance *match_inst = NULL;
  bool status = true;
#ifndef OEM1_SUPPORT_EVENT_TYPE
  sns_request decoded_req;
#endif

  SNS_PRINTF(MED, this, "sns_oem1_set_client_request");

  if((NULL != curr_inst) &&
     (NULL != new_req) &&
     (new_req->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ))
  {
      sns_sensor_util_send_flush_event(this->sensor_api->get_sensor_uid(this), curr_inst);
      rv_inst = curr_inst;
  }
  else if(remove)
  {
    if(NULL != curr_inst)
    {
      curr_inst->cb->remove_client_request(curr_inst, curr_req);
    }
  }
  else if((new_req->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG) || 
          (new_req->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG))
  {
#ifndef OEM1_SUPPORT_EVENT_TYPE
    pb_istream_t stream;
    sns_std_sensor_config config = {0};
    sns_std_request request = sns_std_request_init_default;

    pb_simple_cb_arg arg =
    { .decoded_struct = &config, .fields = sns_std_sensor_config_fields };
    request.payload = (struct pb_callback_s)
          { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
    stream = pb_istream_from_buffer(new_req->request, new_req->request_len);

    if(pb_decode(&stream, sns_std_request_fields, &request))
    {
      decoded_req.message_id = new_req->message_id;
      decoded_req.request_len = sizeof(config);
      decoded_req.request = &config;
      match_inst = sns_sensor_util_find_instance_match(
          this, &decoded_req, &find_instance_match);
    }
    else
    {
      SNS_PRINTF(ERROR, this, "failed to decode request");
      status = false;
    }
#endif
    if(status == true)
    {
      if(NULL == curr_inst)
      {
        // If this is a request from a new client
        if(NULL == match_inst)
        {
          rv_inst = this->cb->create_instance(this,
              (sizeof(sns_oem1_inst_state)));
        }
        else
        {
          rv_inst = match_inst;
        }
      }
      else
      {
        if(NULL != curr_req)
        {
          curr_inst->cb->remove_client_request(curr_inst, curr_req);
        }
        if(NULL != match_inst)
        {
          rv_inst = match_inst;
        }
        else
        {
          rv_inst = this->cb->create_instance(this,
              (sizeof(sns_oem1_inst_state)));
        }
      }

      if(NULL != rv_inst)
      {
        rv_inst->cb->add_client_request(rv_inst, new_req);
#ifndef OEM1_SUPPORT_EVENT_TYPE
        this->instance_api->set_client_config(rv_inst, &decoded_req);
#else
        this->instance_api->set_client_config(rv_inst, new_req);
#endif
      }
    }
  }

  if(NULL != curr_inst && NULL == curr_inst->cb->get_client_request(
      curr_inst, this->sensor_api->get_sensor_uid(this), true))
  {
    this->cb->remove_instance(curr_inst);
  }

  return rv_inst;
}

