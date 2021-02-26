/*=============================================================================
  @file sns_attribute_util.c

  Utility functions provided by Qualcomm for use by Sensors to handle and
  publish attributes.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdbool.h>
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_attribute_service.h"
#include "sns_attribute_util.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_sensor.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_std.pb.h"
#include "sns_types.h"

#define SNS_SECTION(name)  \
  __attribute__((section(name)))

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

SNS_SECTION(".text.sns") bool
sns_pb_encode_attr_cb(pb_ostream_t *stream, const pb_field_t *field,
    void *const *arg)
{
  pb_buffer_arg *values_buf = (pb_buffer_arg*)*arg;
  sns_std_attr_value_data const *values =
    (sns_std_attr_value_data const*)values_buf->buf;
  int i;

  for(i = 0; i < values_buf->buf_len; i++)
  {
    if(!pb_encode_tag_for_field(stream, field))
      return false;
    if(!pb_encode_submessage(stream, sns_std_attr_value_data_fields, &values[i]))
      return false;
  }


  return true;
}

SNS_SECTION(".text.sns") void
sns_publish_attribute(sns_sensor *const sensor,
    uint32_t attribute_id, sns_std_attr_value_data const *values,
    uint32_t values_len, bool completed)
{
  size_t attribute_len = 0;
  sns_std_attr std_attr = (sns_std_attr)
    { .attr_id = attribute_id, .value.values.funcs.encode = &sns_pb_encode_attr_cb,
      .value.values.arg = &((pb_buffer_arg){ .buf = values, .buf_len = values_len }) };

  if(pb_get_encoded_size(&attribute_len, sns_std_attr_fields, &std_attr))
  {
    sns_service_manager *manager = sensor->cb->get_service_manager(sensor);
    sns_attribute_service *attribute_service =
      (sns_attribute_service*)manager->get_service(manager, SNS_ATTRIBUTE_SERVICE);

    uint8_t attribute[attribute_len];
    pb_ostream_t stream = pb_ostream_from_buffer(attribute, attribute_len);

    if(pb_encode(&stream, sns_std_attr_fields, &std_attr))
      attribute_service->api->publish_attribute(attribute_service, sensor,
        attribute, attribute_len, attribute_id, completed);

    // PEND: Print a message upon errors
  }
}
