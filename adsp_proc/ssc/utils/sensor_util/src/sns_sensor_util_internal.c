/*=============================================================================
  @file sns_sensor_util_internal.c

  Internal utility functions for use by Sensor Developers.

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "pb_decode.h"
#include "sns_data_stream.h"
#include "sns_island.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_sensor_util_internal.h"
#include "sns_stream_service.h"
#include "sns_suid.pb.h"
#include "sns_suid_util.h"
#include "sns_types.h"

#define SNS_SECTION(name)  \
  __attribute__((section(name)))

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/
SNS_SECTION(".text.sns") sns_rc
sns_sensor_util_find_consolidated_req(sns_sensor_instance *instance,
                                      sns_sensor_uid const *suid,
                                      sns_std_request* std_req)
{
  sns_request const *req;
  sns_rc ret_val = SNS_RC_INVALID_STATE;
  std_req->batching.batch_period = UINT32_MAX;
  std_req->batching.flush_period = 0;
  std_req->batching.max_batch=true;
  std_req->batching.flush_only=true;
  for(req = instance->cb->get_client_request(instance, suid, true);
      NULL != req;
      req = instance->cb->get_client_request(instance, suid, false))
  {
    sns_std_request decoded_req = sns_std_request_init_default;
    pb_istream_t stream = pb_istream_from_buffer(req->request, req->request_len);
    if (pb_decode(&stream, sns_std_request_fields, &decoded_req))
    {
      if (decoded_req.has_batching)
      {
        std_req->batching.batch_period = decoded_req.batching.batch_period < std_req->batching.batch_period ?
          decoded_req.batching.batch_period : std_req->batching.batch_period;

        if (decoded_req.batching.has_flush_period)
        {
          std_req->batching.flush_period = decoded_req.batching.flush_period > std_req->batching.flush_period ?
            decoded_req.batching.flush_period : std_req->batching.flush_period;
        }
        if (decoded_req.batching.has_max_batch)
        {
          std_req->batching.max_batch = decoded_req.batching.max_batch == true ?
              std_req->batching.max_batch : false;
        }
        else
        {
          std_req->batching.max_batch = false;
        }
        if (decoded_req.batching.has_flush_only)
        {
          std_req->batching.flush_only = decoded_req.batching.flush_only == true ?
              std_req->batching.flush_only : false;
        }
        else
        {
          std_req->batching.flush_only = false;
        }
      }
      else
      {
        // TODO: Decide if (decoded_req.has_batching == false) should be interpreted as
        // "no batching" or as "don't care". Current implementation is for interpreting
        // it as "no batching".
        std_req->batching.batch_period = 0;
        std_req->batching.max_batch = false;
        std_req->batching.flush_only = false;
      }
      ret_val = SNS_RC_SUCCESS;
    }
    else
    {
      SNS_INST_PRINTF(ERROR, instance, "Error decoding client request. Assuming Batch period of 0");
      std_req->batching.batch_period = 0;
      std_req->batching.flush_period = 0;
      std_req->batching.max_batch = false;
      std_req->batching.flush_only = false;
      ret_val = SNS_RC_FAILED;
      break;
    }
  }
  return ret_val;
}

SNS_SECTION(".text.sns") sns_rc
sns_sensor_util_update_std_sensor_batching(
    sns_sensor *const this,
    sns_sensor_instance *curr_inst)
{
  sns_rc rc = SNS_RC_SUCCESS;
  struct sns_request const* cur_request = curr_inst->cb->get_client_request(curr_inst,
    this->sensor_api->get_sensor_uid(this), true);

  if(NULL != cur_request)
  {
    pb_istream_t stream;
    sns_std_sensor_config config = sns_std_sensor_config_init_default;
    sns_std_request request = sns_std_request_init_default;
    pb_simple_cb_arg arg =
      { .decoded_struct = &config,
        .fields = sns_std_sensor_config_fields };

    request.payload = (struct pb_callback_s)
        { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
    stream = pb_istream_from_buffer(cur_request->request, cur_request->request_len);
    if(pb_decode(&stream, sns_std_request_fields, &request))
    {
      size_t encoded_len;
      uint8_t buffer[100];

      sns_sensor_util_find_consolidated_req(curr_inst,
        this->sensor_api->get_sensor_uid(this),
        &request);
      if(request.batching.batch_period > 0)
      {
        request.has_batching = true;
      }
      if(request.batching.flush_period > 0)
      {
        request.has_batching = true;
        request.batching.has_flush_period = true;
      }
      if(request.batching.max_batch)
      {
        request.has_batching = true;
        request.batching.has_max_batch = true;
      }
      if(request.batching.flush_only)
      {
        request.has_batching = true;
        request.batching.has_flush_only = true;
      }
      encoded_len = pb_encode_request(buffer,
                                      sizeof(buffer),
                                      &config,
                                      sns_std_sensor_config_fields,
                                      &request);
      if(0 < encoded_len)
      {
        sns_request request = (sns_request){
          .message_id = cur_request->message_id,
          .request_len = encoded_len, .request = buffer };
        this->instance_api->set_client_config(curr_inst,&request);
      }
      else
      {
        SNS_PRINTF(ERROR, this, "Failed to encode request");
        rc = SNS_RC_FAILED;
      }
    }
    else
    {
      SNS_PRINTF(ERROR, this, "Failed to decode request");
      rc = SNS_RC_FAILED;
    }
  }
  else
  {
    SNS_PRINTF(LOW, this, "No request left for instance");
    rc = SNS_RC_INVALID_VALUE;
  }
  return rc;
}

SNS_SECTION(".text.sns") bool
sns_sensor_util_instance_match(float const *sample_rate,
  struct sns_request const *new_req, struct sns_request const *exist_req)
{

  bool ret = false;
  pb_istream_t new_req_stream;
  pb_istream_t exist_req_stream;
  sns_std_request new_std_request = sns_std_request_init_default;
  sns_std_request exist_std_request = sns_std_request_init_default;
  sns_std_sensor_config new_config = sns_std_sensor_config_init_default;
  sns_std_sensor_config exist_config = sns_std_sensor_config_init_default;
  pb_simple_cb_arg new_arg = {
    .decoded_struct = &new_config,
    .fields = sns_std_sensor_config_fields
  };
  pb_simple_cb_arg exist_arg = {
    .decoded_struct = &exist_config,
    .fields = sns_std_sensor_config_fields
  };
  new_std_request.payload = (struct pb_callback_s){
    .funcs.decode = &pb_decode_simple_cb,
    .arg = &new_arg
  };
  exist_std_request.payload = (struct pb_callback_s){
    .funcs.decode = &pb_decode_simple_cb,
    .arg = &exist_arg
  };

  new_req_stream = pb_istream_from_buffer(new_req->request, new_req->request_len);
  exist_req_stream = pb_istream_from_buffer(exist_req->request, exist_req->request_len);
  if(pb_decode(&new_req_stream, sns_std_request_fields, &new_std_request) &&
     pb_decode(&exist_req_stream, sns_std_request_fields, &exist_std_request))
  {
    if(0 == sns_memcmp(sample_rate, &new_config.sample_rate,
        sizeof(new_config.sample_rate)))
    {
      sns_std_request_batch_spec *new_req_spec = &new_std_request.batching;
      sns_std_request_batch_spec *exist_req_spec = &exist_std_request.batching;

      if(new_std_request.has_batching == exist_std_request.has_batching &&
       new_req_spec->has_flush_only == exist_req_spec->has_flush_only)
      {
        ret = true;
      }
    }
  }
  else
  {
    ret = false;
  }
  return ret;
}

bool
sns_sensor_util_decode_attr_value(pb_istream_t *stream,
    const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_sensor_util_attrib *sensor_attrib = *arg;

  if(NULL != sensor_attrib)
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    pb_buffer_arg str_data = (pb_buffer_arg){.buf = NULL, .buf_len = 0 };
    value.str.funcs.decode = &pb_decode_string_cb;
    value.str.arg = &str_data;

    value.subtype.values.funcs.decode = &sns_sensor_util_decode_attr_value;
    value.subtype.values.arg = (void*)sensor_attrib;

    if(!pb_decode(stream, sns_std_attr_value_data_fields, &value))
    {
      SNS_PRINTF(ERROR, sensor_attrib->sensor, "Error decoding attribute");
      return false;
    }

    if(value.has_sint)
    {
      sensor_attrib->attr_value = value.sint;
    }
  }
  return true;
}

bool
sns_sensor_util_decode_attr(pb_istream_t *stream,
    const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_sensor_util_attrib *sensor_attrib = *arg;
  pb_istream_t stream_cpy = *stream;

  sns_std_attr attribute = sns_std_attr_init_default;
  attribute.value.values.funcs.decode = NULL;
  attribute.value.values.arg = arg;

  if (!pb_decode(stream, sns_std_attr_fields, &attribute)) {
    SNS_PRINTF(ERROR, sensor_attrib->sensor, "Error decoding attribute");
    return false;
  }

  if(sensor_attrib->attr_id == attribute.attr_id)
  {
    attribute.attr_id = -1;
    attribute.value.values.funcs.decode = &sns_sensor_util_decode_attr_value;
    attribute.value.values.arg = (void *)sensor_attrib;

    if(!pb_decode(&stream_cpy, sns_std_attr_fields, &attribute)) {
      SNS_PRINTF(ERROR, sensor_attrib->sensor, "Error decoding attribute");
      return false;
    }
  }
  return true;
}
