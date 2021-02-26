/**
 * @file sns_interrupt_sensor.c
 *
 * The Interrupt virtual Sensor implementation
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/sensors/interrupt/hexagon/sns_interrupt_sensor.c#2 $
 * $DateTime: 2018/03/27 11:46:46 $
 * $Change: 15784639 $
 *
 **/

#include <string.h>
#include "sns_attribute_service.h"
#include "sns_attribute_util.h"
#include "sns_event_service.h"
#include "sns_interrupt_sensor.h"
#include "sns_interrupt_sensor_instance.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_mem_util.h"
#include "sns_printf.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_uid.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_signal.h"
#include "sns_stream_service.h"
#include "sns_thread_prio.h"
#include "sns_types.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_pb_util.h"
#include "sns_std.pb.h"
#include "sns_suid.pb.h"


/**
 * Sensor UID
 */
  static const sns_sensor_uid sensor_uid SNS_SECTION(".rodata.sns")=
      {
        .sensor_uid = {0x61, 0x8f, 0xf5, 0xe2, 0x56, 0x11, 0x46, 0xe2,
                       0xb6, 0xcf, 0xe3, 0x6e, 0xee, 0x3d, 0xd0, 0x45}
      };


/**
 * Initialize attributes to their default state.  They may/will be updated
 * within notify_event.
 */
static void publish_attributes(sns_sensor *const this)
{
  {
    char const name[] = "interrupt";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = "interrupt";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = type, .buf_len = sizeof(type) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
  }
  {
    char const vendor[] = "qti_qualcomm";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = vendor, .buf_len = sizeof(vendor) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VENDOR, &value, 1, false);
  }
  {
    char const proto_files[] = "sns_interrupt.proto";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = proto_files, .buf_len = sizeof(proto_files) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_API, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = true;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = 1;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, true);
  }
}

/* See sns_sensor::notify_event */
static sns_rc sns_irq_notify_event(sns_sensor *const this)
{
  UNUSED_VAR(this);
  return SNS_RC_SUCCESS;
}

/* See sns_sensor::init */
static sns_rc sns_irq_init(sns_sensor *const this)
{
  sns_rc rc = SNS_RC_FAILED;
  interrupt_state *state = (interrupt_state *)this->state->state;

  state->signal_thread = sns_signal_thread_create(this, "sns_sig_irq",
    SNS_INTERRUPT_THREAD_PRIO, sns_irq_gen_event, NULL, true);
  if (state->signal_thread)
  {
    rc = SNS_RC_SUCCESS;
  }
  publish_attributes(this);
  return rc;
}

/* See sns_sensor::get_sensor_uid */
SNS_SECTION(".text.sns") static sns_sensor_uid const*
  sns_irq_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  return &sensor_uid;
}

/**
 * See sns_sensor_util_instance_match_cb
 */
SNS_SECTION(".text.sns") static bool
find_instance_match(struct sns_request const *request,
   sns_sensor_instance const *instance)
{
  sns_interrupt_instance_state const *inst_state =
      (sns_interrupt_instance_state*)instance->state->state;

  /*Use only irq_num in request to find an instance match.
    The idea is to have one instance per irq_num.
    Say accel is streaming and had already registered the interupt
    and then gyro wants to change drive strength of the pin,
    we should be able to change config of the same instance and not
    create a new one. In case both accel and gyro have the same irq
    config then the instance will handle a no-op like operation.*/
  return (inst_state->client_req.interrupt_num ==
       ((sns_interrupt_req*)request->request)->interrupt_num);
}

/* See sns_sensor::set_client_request */
SNS_SECTION(".text.sns") static sns_sensor_instance*
sns_irq_set_client_request(sns_sensor *const this,
                           struct sns_request const *exist_request,
                           struct sns_request const *new_request,
                           bool remove)
{
  sns_sensor_instance *curr_inst = sns_sensor_util_find_instance(this,
      exist_request, this->sensor_api->get_sensor_uid(this));
  sns_sensor_instance *rv_inst = NULL;

  if(remove)
  {
    if(NULL != curr_inst)
    {
      curr_inst->cb->remove_client_request(curr_inst, exist_request);
    }
  }
  else
  {
    pb_istream_t stream;
    sns_interrupt_req payload;
    sns_std_request request = sns_std_request_init_default;
    pb_simple_cb_arg arg =
      { .decoded_struct = &payload, .fields = sns_interrupt_req_fields };
    request.payload = (struct pb_callback_s)
      { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };

    if(new_request->message_id != SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REQ
       &&
       new_request->message_id != SNS_INTERRUPT_MSGID_SNS_INTERRUPT_IS_CLEARED)
    {
      SNS_PRINTF(ERROR, this, "Unsupported message id = %d", new_request->message_id);
      return NULL;
    }

    if(NULL == exist_request)
    {
      /** new request for interrupt sensor */
      stream = pb_istream_from_buffer(new_request->request,
                                      new_request->request_len);
    }
    else
    {
      /** updated request/message to/for interrupt sensor
       *  Continue using the existing request to extract interrupt num.
       *  This should be the original interrupt registration request. */
      stream = pb_istream_from_buffer(exist_request->request,
                                      exist_request->request_len);
    }

    if(pb_decode(&stream, sns_std_request_fields, &request))
    {
      sns_request decoded_new_request =
      {
        .message_id = new_request->message_id,
        .request_len = sizeof(payload),
        .request = &payload
      };

      sns_sensor_instance *match_inst = sns_sensor_util_find_instance_match(
          this, &decoded_new_request, &find_instance_match);

      if(NULL == curr_inst)
      {
        /** If this is a request from a new client */
        if(NULL == match_inst
           && new_request->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REQ)
        {
          rv_inst = this->cb->create_instance(this,
                                              sizeof(sns_interrupt_instance_state));
        }
        else
        {
          rv_inst = match_inst;
        }
      }
      else
      {
        if(NULL != match_inst)
        {
          rv_inst = match_inst;
        }
        else if(NULL != curr_inst->cb->get_client_request(
              curr_inst, sns_irq_get_sensor_uid(this), true))
        {
          rv_inst = curr_inst;
        }
        else
        {
          if(new_request->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REQ)
          {
            rv_inst = this->cb->create_instance(this,
                                                sizeof(sns_interrupt_instance_state));
          }
          else
          {
            SNS_PRINTF(ERROR, this, "Cannot create instance for msg id = %d", new_request->message_id);
          }
        }
      }
      if(NULL != rv_inst)
      {
        /** Interrupt sensor only adds a new interrupt registration request
         *  to the instance's list of client requests.
         *  Interrupt is cleared messages are handled as transitory messages. */
        if(new_request->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REQ)
        {
          rv_inst->cb->add_client_request(rv_inst, new_request);
        }
        this->instance_api->set_client_config(rv_inst, &decoded_new_request);
      }
    }

    if(NULL != curr_inst
       && new_request->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REQ)
    {
      curr_inst->cb->remove_client_request(curr_inst, exist_request);
    }
  }

  if(NULL != curr_inst && NULL == curr_inst->cb->get_client_request(
        curr_inst, sns_irq_get_sensor_uid(this), true))
  {
    this->cb->remove_instance(curr_inst);
  }

  return rv_inst;
}

static sns_rc sns_irq_deinit(sns_sensor *const this)
{
  interrupt_state *state = (interrupt_state *)this->state->state;
  sns_rc rc = SNS_RC_SUCCESS;
  if(SNS_RC_SUCCESS != sns_signal_thread_destroy(state->signal_thread))
  {
    rc = SNS_RC_INVALID_STATE;
  }
  return rc;
}

/** Public Data Definitions. */

sns_sensor_api interrupt_sensor_api SNS_SECTION(".data.sns") =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &sns_irq_init,
  .deinit             = &sns_irq_deinit,
  .get_sensor_uid     = &sns_irq_get_sensor_uid,
  .set_client_request = &sns_irq_set_client_request,
  .notify_event       = &sns_irq_notify_event,
};
