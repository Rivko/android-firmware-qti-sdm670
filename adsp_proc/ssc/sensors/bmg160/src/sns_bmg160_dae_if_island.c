/*******************************************************************************
 * Copyright (c) 2018, Bosch Sensortec GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Bosch Sensortec GmbH nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#if BMG160_CONFIG_ENABLE_DAE
#include "sns_mem_util.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_event.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_time.h"
#include "sns_types.h"

#include "sns_bmg160_hal.h"
#include "sns_bmg160_sensor.h"
#include "sns_bmg160_sensor_instance.h"

#include "sns_dae.pb.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_diag_service.h"
#include "sns_pb_util.h"
#include "sns_printf.h"


/*======================================================================================
  Helper Functions
  ======================================================================================*/

/* ------------------------------------------------------------------------------------ */
static bool send_gyro_config(bmg160_dae_stream *dae_stream, bmg160_fifo_info* fifo_info)
{
  bool cmd_sent = false;
  sns_dae_set_streaming_config config_req = sns_dae_set_streaming_config_init_default;
  uint8_t encoded_msg[sns_dae_set_streaming_config_size];
  sns_request req = {
    .message_id   = SNS_DAE_MSGID_SNS_DAE_SET_STREAMING_CONFIG,
    .request      = encoded_msg
  };

  config_req.has_accel_info      = false;
  config_req.dae_watermark       = fifo_info->max_requested_wmk;
  config_req.has_polling_config  = false;

  if((req.request_len = 
      pb_encode_request(encoded_msg, 
                        sizeof(encoded_msg), 
                        &config_req,
                        sns_dae_set_streaming_config_fields, 
                        NULL)) > 0)
  {
    if(SNS_RC_SUCCESS == dae_stream->stream->api->send_request(dae_stream->stream, &req))
    {
      cmd_sent = true;
    }
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
static bool send_temp_config(
  bmg160_dae_stream       *dae_stream,
  bmg160_sensor_temp_info *temp_info)
{
  bool cmd_sent = false;
  sns_time time_now = sns_get_system_time();
  uint64_t dae_wm = (uint64_t)(temp_info->sampling_rate_hz / temp_info->report_rate_hz);
  sns_dae_set_streaming_config config_req = sns_dae_set_streaming_config_init_default;
  uint8_t encoded_msg[sns_dae_set_streaming_config_size];
  sns_request req = {
    .message_id   = SNS_DAE_MSGID_SNS_DAE_SET_STREAMING_CONFIG,
    .request      = encoded_msg
  };

  config_req.dae_watermark = (dae_wm > UINT32_MAX) ? UINT32_MAX : (uint32_t)dae_wm;
  config_req.has_polling_config = true;
  config_req.polling_config.polling_interval_ticks = temp_info->sampling_intvl;
  config_req.polling_config.polling_offset = 
    (time_now + temp_info->sampling_intvl) / temp_info->sampling_intvl * 
    temp_info->sampling_intvl;

  if((req.request_len = 
      pb_encode_request(encoded_msg, 
                        sizeof(encoded_msg), 
                        &config_req,
                        sns_dae_set_streaming_config_fields, 
                        NULL)) > 0)
  {
    if(SNS_RC_SUCCESS == dae_stream->stream->api->send_request(dae_stream->stream, &req))
    {
      dae_stream->state = STREAM_STARTING;
      cmd_sent = true;
    }
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
static bool flush_hw(bmg160_dae_stream *dae_stream)
{
  bool cmd_sent = false;
  sns_request req = {
    .message_id   = SNS_DAE_MSGID_SNS_DAE_FLUSH_HW,
    .request      = NULL,
    .request_len  = 0
  };

  if(SNS_RC_SUCCESS == dae_stream->stream->api->send_request(dae_stream->stream, &req))
  {
    cmd_sent = true;
    dae_stream->flushing_hw = true;
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
static bool flush_samples(bmg160_dae_stream *dae_stream)
{
  bool cmd_sent = false;
  sns_request req = {
    .message_id   = SNS_DAE_MSGID_SNS_DAE_FLUSH_DATA_EVENTS,
    .request      = NULL,
    .request_len  = 0
  };

  if(SNS_RC_SUCCESS == dae_stream->stream->api->send_request(dae_stream->stream, &req))
  {
    cmd_sent = true;
    dae_stream->flushing_data = true;
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
static bool stop_streaming(bmg160_dae_stream *dae_stream)
{
  bool cmd_sent = false;
  sns_request req = {
    .message_id   = SNS_DAE_MSGID_SNS_DAE_PAUSE_SAMPLING,
    .request      = NULL,
    .request_len  = 0
  };

  if(SNS_RC_SUCCESS == dae_stream->stream->api->send_request(dae_stream->stream, &req))
  {
    cmd_sent = true;
    dae_stream->state = STREAM_STOPPING;
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
static void process_fifo_samples(
  sns_sensor_instance *this, 
  sns_time            timestamp,
  const uint8_t       *buf, 
  size_t              buf_len)
{
  const uint8_t *fifo_start = buf + 2; /* 1st byte = REG_CTRL1_A, 2nd byte = REG_CTRL2_G */
  uint16_t num_sample_sets =(buf_len / 6);
  bmg160_gyro_odr_regv accel_odr = (bmg160_gyro_odr_regv)(*buf & 0xF0);
  sns_time sampling_intvl = bmg160_get_sample_interval(accel_odr);

  if(num_sample_sets >= 1 && sampling_intvl > 0)
  {
    sns_time first_timestamp = timestamp - sampling_intvl * (num_sample_sets - 1);
    bmg160_process_fifo_data_buffer(this,
                                     first_timestamp, 
                                     sampling_intvl,
                                     fifo_start, 
                                     buf_len - 2);
  }
}

/* ------------------------------------------------------------------------------------ */
static void process_temp_samples(
  sns_sensor_instance *this, 
  sns_time            timestamp,
  const uint8_t       *buf, 
  size_t              buf_len)
{
  int8_t temp_data = (int8_t)buf[0];
  if(buf_len == 2)
  {
    bmg160_convert_and_send_temp_sample(this, timestamp, temp_data);
  }
  else
  {
    SNS_INST_PRINTF(ERROR, this,  "Unexpected data len %u from DAE sensor", buf_len);
  }
}


/* ------------------------------------------------------------------------------------ */
static void process_data_event(
  sns_sensor_instance *this, 
  bmg160_dae_stream  *dae_stream,
  pb_istream_t        *pbstream)
{
  pb_buffer_arg decode_arg;
  sns_dae_data_event data_event = sns_dae_data_event_init_default;
  data_event.sensor_data.funcs.decode = &pb_decode_string_cb;
  data_event.sensor_data.arg = &decode_arg;
  if(pb_decode(pbstream, sns_dae_data_event_fields, &data_event))
  {
    bmg160_dae_if_info *dae_if = &((bmg160_instance_state*)this->state->state)->dae_if;
    if (dae_stream == &dae_if->ag)
    {
      process_fifo_samples(
        this, data_event.timestamp, (uint8_t*)decode_arg.buf, decode_arg.buf_len);
    }
    else
    {
      process_temp_samples(
        this, data_event.timestamp, (uint8_t*)decode_arg.buf, decode_arg.buf_len);
    }
  }
}

/* ------------------------------------------------------------------------------------ */
static bool process_response(
  sns_sensor_instance *this, 
  bmg160_dae_stream  *dae_stream,
  pb_istream_t        *pbstream)
{
  bool stream_usable = true;
  bmg160_instance_state *state = (bmg160_instance_state*)this->state->state;

  sns_dae_resp resp = sns_dae_resp_init_default;
  if(pb_decode(pbstream, sns_dae_resp_fields, &resp))
  {
    switch(resp.msg_id)
    {
    case SNS_DAE_MSGID_SNS_DAE_SET_STATIC_CONFIG:
      if(SNS_STD_ERROR_NO_ERROR != resp.err)
      {
        /* DAE sensor does not have support for this driver */
        stream_usable = false;
      }
      break;
    case SNS_DAE_MSGID_SNS_DAE_S4S_DYNAMIC_CONFIG:
      break;
    case SNS_DAE_MSGID_SNS_DAE_SET_STREAMING_CONFIG:
      if(dae_stream->stream != NULL && dae_stream->state == STREAM_STARTING)
      {
        dae_stream->state = (SNS_STD_ERROR_NO_ERROR == resp.err) ? STREAMING : IDLE;
      }
      break;
    case SNS_DAE_MSGID_SNS_DAE_FLUSH_HW:
      if(state->config_step != BMG160_CONFIG_IDLE)
      {
        bmg160_hal_reconfig_hw(this);
      }
      break;
    case SNS_DAE_MSGID_SNS_DAE_FLUSH_DATA_EVENTS:
      if(state->fifo_flush_in_progress)
      {
        state->fifo_flush_in_progress = false;
        bmg160_send_fifo_flush_done(this);
      }
      break;
    case SNS_DAE_MSGID_SNS_DAE_PAUSE_SAMPLING:
      if(dae_stream->state == STREAM_STOPPING)
      {
        dae_stream->state = (SNS_STD_ERROR_NO_ERROR != resp.err) ? STREAMING : IDLE;
      }
      if(NULL != state->dae_if.ag.stream && 
         state->dae_if.ag.state != STREAM_STOPPING &&
         NULL != state->dae_if.temp.stream &&
         state->dae_if.temp.state != STREAM_STOPPING)
      {
        /* done waiting */
        if(state->config_step == BMG160_CONFIG_STOPPING_STREAM)
        {
          bmg160_dae_if_flush_hw(this);
        }
      }
      break;
    case SNS_DAE_MSGID_SNS_DAE_PAUSE_S4S_SCHED:
      break;

    case SNS_DAE_MSGID_SNS_DAE_RESP:
    case SNS_DAE_MSGID_SNS_DAE_DATA_EVENT:
      break; /* unexpected */
    }
  }
  return stream_usable;
}

/* ------------------------------------------------------------------------------------ */
static void process_events(sns_sensor_instance *this, bmg160_dae_stream *dae_stream)
{
  sns_sensor_event *event;
  bool stream_usable = true;

  while(NULL != dae_stream->stream && 
        NULL != (event = dae_stream->stream->api->peek_input(dae_stream->stream)))
  {
    if (stream_usable)
    {
      pb_istream_t pbstream = 
        pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);

      if (SNS_DAE_MSGID_SNS_DAE_DATA_EVENT == event->message_id)
      {
        process_data_event(this, dae_stream, &pbstream);
      }
      else if(SNS_DAE_MSGID_SNS_DAE_INTERRUPT_EVENT == event->message_id)
      {
      }
      else if(SNS_DAE_MSGID_SNS_DAE_RESP == event->message_id)
      {
        stream_usable = process_response(this, dae_stream, &pbstream);
      }
      else
      {
        SNS_INST_PRINTF(ERROR, this, "Unexpected message id %u", event->message_id);
        /* TODO - report unexpected message */
      }
    }
    event = dae_stream->stream->api->get_next_input(dae_stream->stream);
  }

  if(!stream_usable)
  {
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_mgr =
      (sns_stream_service*)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    stream_mgr->api->remove_stream(stream_mgr, dae_stream->stream);
    dae_stream->stream = NULL;
  }
}

/*======================================================================================
  Public Functions
  ======================================================================================*/
bool bmg160_dae_if_available(sns_sensor_instance *this)
{
  /* both streams must be availble */
  bmg160_dae_if_info *dae_if = &((bmg160_instance_state*)this->state->state)->dae_if;
  return (dae_if->ag.stream != NULL && dae_if->temp.stream != NULL);
}

/* ------------------------------------------------------------------------------------ */
sns_rc bmg160_dae_if_init(
  sns_sensor_instance *const this,
  sns_stream_service  *stream_mgr,
  sns_sensor_uid      *dae_suid)
{
  sns_rc rc = SNS_RC_NOT_AVAILABLE;
  bmg160_instance_state *state = (bmg160_instance_state*)this->state->state;
  bmg160_dae_if_info* dae_if = &state->dae_if;
  sns_dae_set_static_config config_req = sns_dae_set_static_config_init_default;

  stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                 this,
                                                 *dae_suid,
                                                 &dae_if->ag.stream);
  stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                 this,
                                                 *dae_suid,
                                                 &dae_if->temp.stream);

  if(NULL != dae_if->ag.stream && NULL != dae_if->temp.stream)
  {
    uint8_t encoded_msg[sns_dae_set_static_config_size];
    sns_request req = {
      .message_id  = SNS_DAE_MSGID_SNS_DAE_SET_STATIC_CONFIG,
      .request     = encoded_msg,
      .request_len = 0
    };

    dae_if->ag.nano_hal_vtable_name = "bmg160_hal_table";

    sns_strlcpy(config_req.func_table_name,
                dae_if->ag.nano_hal_vtable_name, 
                sizeof(config_req.func_table_name));
    config_req.interrupt              = false;
    //config_req.axis_map_count       = 3;
    //config_req.axis_map             = {...};
    config_req.has_irq_config         = true;
    config_req.irq_config             = state->irq_info.irq_config;
    config_req.has_s4s_config         = false;
    config_req.ascp_config            = state->ascp_config;

    req.request_len = pb_encode_request(encoded_msg, 
                                        sizeof(encoded_msg), 
                                        &config_req,
                                        sns_dae_set_static_config_fields, 
                                        NULL);
    if(0 < req.request_len)
    {
      rc = dae_if->ag.stream->api->send_request(dae_if->ag.stream, &req);
    }

    if(SNS_RC_SUCCESS == rc)
    {
      dae_if->temp.nano_hal_vtable_name = "bmg160_temperature_hal_table";
      sns_strlcpy(config_req.func_table_name,
                  dae_if->temp.nano_hal_vtable_name, 
                  sizeof(config_req.func_table_name));
      config_req.interrupt          = false;
      config_req.has_irq_config     = false;

      req.request_len = pb_encode_request(encoded_msg,
                                          sizeof(encoded_msg), 
                                          &config_req,
                                          sns_dae_set_static_config_fields, 
                                          NULL);
      if(0 < req.request_len)
      {
        rc = dae_if->temp.stream->api->send_request(dae_if->temp.stream, &req);
      }
    }
  }

  if(SNS_RC_SUCCESS != rc)
  {
    bmg160_dae_if_deinit(state, stream_mgr);
  }
  return rc;
}

/* ------------------------------------------------------------------------------------ */
void bmg160_dae_if_deinit(bmg160_instance_state *state, sns_stream_service *stream_mgr)
{
  if(NULL != state->dae_if.ag.stream)
  {
    stream_mgr->api->remove_stream(stream_mgr, state->dae_if.ag.stream);
    state->dae_if.ag.stream = NULL;
  }
  if(NULL != state->dae_if.temp.stream)
  {
    stream_mgr->api->remove_stream(stream_mgr, state->dae_if.temp.stream);
    state->dae_if.temp.stream = NULL;
  }
}

/* ------------------------------------------------------------------------------------ */
bool bmg160_dae_if_stop_streaming(sns_sensor_instance *this)
{
  bool cmd_sent = false;
  bmg160_instance_state *state = (bmg160_instance_state*)this->state->state;
  bmg160_dae_if_info    *dae_if = &state->dae_if;

  if(NULL != dae_if->ag.stream && 
     (dae_if->ag.state == STREAMING || dae_if->ag.state == STREAM_STARTING))
  {
    cmd_sent |= stop_streaming(&dae_if->ag);
  }

  if(NULL != dae_if->temp.stream && 
     (dae_if->temp.state == STREAMING || dae_if->temp.state == STREAM_STARTING))
  {
    cmd_sent |= stop_streaming(&dae_if->temp);
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
bool bmg160_dae_if_start_streaming(sns_sensor_instance *this)
{
  bool cmd_sent = false;
  bmg160_instance_state *state = (bmg160_instance_state*)this->state->state;
  bmg160_dae_if_info    *dae_if = &state->dae_if;

  if(NULL != dae_if->ag.stream &&
     (0 < state->gyro_info.curr_odr_regv))
  {
    cmd_sent |= send_gyro_config(&dae_if->ag, &state->fifo_info);
  }

  if(NULL != dae_if->temp.stream && 0 < state->sensor_temp_info.sampling_intvl)
  {
    cmd_sent |= send_temp_config(&dae_if->temp, &state->sensor_temp_info);
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
bool bmg160_dae_if_flush_hw(sns_sensor_instance *this)
{
  bool cmd_sent = false;
  bmg160_dae_if_info *dae_if = &((bmg160_instance_state*)this->state->state)->dae_if;

  if(NULL != dae_if->ag.stream && !dae_if->ag.flushing_hw)
  {
    cmd_sent |= flush_hw(&dae_if->ag);
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
bool bmg160_dae_if_flush_samples(sns_sensor_instance *this)
{
  bool cmd_sent = false;
  bmg160_dae_if_info *dae_if = &((bmg160_instance_state*)this->state->state)->dae_if;

  if(NULL != dae_if->ag.stream && !dae_if->ag.flushing_data)
  {
    if(!dae_if->ag.flushing_data)
    {
      flush_samples(&dae_if->ag);
    }
    cmd_sent |= dae_if->ag.flushing_data;
  }

  if(NULL != dae_if->temp.stream)
  {
    if(!dae_if->temp.flushing_data)
    {
      flush_samples(&dae_if->temp);
    }
    cmd_sent |= dae_if->temp.flushing_data;
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
void bmg160_dae_if_process_events(sns_sensor_instance *this)
{
  bmg160_instance_state *state = (bmg160_instance_state*)this->state->state;
  process_events(this, &state->dae_if.ag);
  process_events(this, &state->dae_if.temp);

  if(NULL == state->dae_if.ag.stream || NULL == state->dae_if.temp.stream)
  {
    /* both streams are needed; if one was removed, remove the other one too */
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_mgr =
      (sns_stream_service*)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    bmg160_dae_if_deinit(state, stream_mgr);
  }
}

#endif
