/**
 * @file sns_lsm6ds3_dae_if.c
 *
 * LSM6DS3 - DAE sensor interface
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#include "sns_mem_util.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_time.h"
#include "sns_sensor_event.h"
#include "sns_types.h"

#include "sns_lsm6ds3_hal.h"
#include "sns_lsm6ds3_sensor.h"
#include "sns_lsm6ds3_sensor_instance.h"

#include "sns_dae.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_diag_service.h"
#include "sns_printf.h"

/*======================================================================================
  Macros
  ======================================================================================*/

/*======================================================================================
  Helper Functions
  ======================================================================================*/
static bool stream_usable(lsm6ds3_dae_stream *dae_stream)
{
  return (NULL != dae_stream->stream && dae_stream->stream_usable);
}

/* ------------------------------------------------------------------------------------ */
static sns_rc send_accel_static_config_request(sns_sensor_instance *const this)
{
  sns_rc rc = SNS_RC_NOT_AVAILABLE;
  lsm6ds3_instance_state *state = (lsm6ds3_instance_state*)this->state->state;
  lsm6ds3_dae_if_info* dae_if = &state->dae_if;
  sns_dae_set_static_config config_req = sns_dae_set_static_config_init_default;

  if(NULL != dae_if->ag.stream)
  {
    uint8_t encoded_msg[sns_dae_set_static_config_size];
    sns_request req = {
      .message_id  = SNS_DAE_MSGID_SNS_DAE_SET_STATIC_CONFIG,
      .request     = encoded_msg,
      .request_len = 0
    };

    dae_if->ag.nano_hal_vtable_name = "lsm6ds3_hal_table";

    sns_strlcpy(config_req.func_table_name,
                dae_if->ag.nano_hal_vtable_name,
                sizeof(config_req.func_table_name));

    config_req.interrupt              = true;
    config_req.has_irq_config         = true;
    config_req.irq_config             = state->irq_info.irq_config;
    config_req.has_s4s_config         = false;
    config_req.ascp_config            = state->ascp_config;
    config_req.has_accel_info         = true;
    switch(state->accel_info.range)
    {
      case LSM6DS3_ACCEL_RANGE_2G:
        config_req.accel_info.accel_range = 2;
        break;
      case LSM6DS3_ACCEL_RANGE_4G:
        config_req.accel_info.accel_range = 4;
        break;
      case LSM6DS3_ACCEL_RANGE_8G:
        config_req.accel_info.accel_range = 8;
        break;
      case LSM6DS3_ACCEL_RANGE_16G:
        config_req.accel_info.accel_range = 16;
        break;
    }

    config_req.accel_info.axis_map_count = ARR_SIZE(config_req.accel_info.axis_map);
    for(uint32_t i = 0; i < config_req.accel_info.axis_map_count; i++)
    {
      config_req.accel_info.axis_map[i].ipaxis = state->axis_map[i].ipaxis;
      config_req.accel_info.axis_map[i].opaxis = state->axis_map[i].opaxis;
      config_req.accel_info.axis_map[i].invert = state->axis_map[i].invert;
    }

    req.request_len = pb_encode_request(encoded_msg,
                                        sizeof(encoded_msg),
                                        &config_req,
                                        sns_dae_set_static_config_fields,
                                        NULL);
    if(0 < req.request_len)
    {
      rc = dae_if->ag.stream->api->send_request(dae_if->ag.stream, &req);
    }
  }
  return rc;
}

/* ------------------------------------------------------------------------------------ */
static bool send_ag_config(lsm6ds3_dae_stream *dae_stream, lsm6ds3_instance_state* state)
{
  bool cmd_sent = false;
  sns_accel_dynamic_info *accel_info;
  sns_dae_set_streaming_config config_req = sns_dae_set_streaming_config_init_default;
  uint8_t encoded_msg[sns_dae_set_streaming_config_size];
  sns_request req = {
    .message_id   = SNS_DAE_MSGID_SNS_DAE_SET_STREAMING_CONFIG,
    .request      = encoded_msg
  };

  config_req.dae_watermark            = state->fifo_info.max_requested_wmk;
  config_req.has_data_age_limit_ticks = true;
  config_req.data_age_limit_ticks     = state->fifo_info.max_requested_flush_ticks;
  config_req.has_polling_config  = false;

  config_req.has_accel_info      = true;
  accel_info                   = &config_req.accel_info;
  accel_info->odr              = lsm6ds3_get_accel_odr(state->accel_info.curr_odr);
  accel_info->num_initial_invalid_samples = state->accel_info.num_samples_to_discard;
  accel_info->offset_cal_count = 3;
  accel_info->offset_cal[0]    = state->accel_registry_cfg.fac_cal_bias[0];
  accel_info->offset_cal[1]    = state->accel_registry_cfg.fac_cal_bias[1];
  accel_info->offset_cal[2]    = state->accel_registry_cfg.fac_cal_bias[2];
  accel_info->scale_cal_count  = 3;
  accel_info->scale_cal[0]     = state->accel_registry_cfg.fac_cal_corr_mat.e00;
  accel_info->scale_cal[1]     = state->accel_registry_cfg.fac_cal_corr_mat.e11;
  accel_info->scale_cal[2]     = state->accel_registry_cfg.fac_cal_corr_mat.e22;

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
static bool send_temp_config(
  lsm6ds3_dae_stream       *dae_stream,
  sns_sensor_instance      *this)
{
  bool cmd_sent = false;
  sns_time time_now = sns_get_system_time();
  lsm6ds3_instance_state *state = (lsm6ds3_instance_state*)this->state->state;
  lsm6ds3_sensor_temp_info *temp_info = &state->sensor_temp_info;
  uint32_t dae_wm = (uint32_t)(temp_info->sampling_rate_hz / temp_info->report_rate_hz);
  sns_dae_set_streaming_config config_req = sns_dae_set_streaming_config_init_zero;
  uint8_t encoded_msg[sns_dae_set_streaming_config_size];
  sns_request req = {
    .message_id   = SNS_DAE_MSGID_SNS_DAE_SET_STREAMING_CONFIG,
    .request      = encoded_msg
  };

  config_req.dae_watermark = dae_wm;
  config_req.has_data_age_limit_ticks = true;
  config_req.data_age_limit_ticks     = state->fifo_info.max_requested_flush_ticks;
  config_req.has_polling_config = true;
  config_req.polling_config.polling_interval_ticks = temp_info->sampling_intvl;
  config_req.polling_config.polling_offset =
    (time_now + temp_info->sampling_intvl) / temp_info->sampling_intvl *
    temp_info->sampling_intvl;

  LSM6DS3_INST_PRINTF(MED, this,
                 "send_temp_config:: SR=%u RR=%u WM=%u now=%x %08x", 
                 (uint32_t)temp_info->sampling_rate_hz,
                 (uint32_t)temp_info->report_rate_hz, dae_wm, 
                 (uint32_t)(time_now >> 32), (uint32_t)time_now);
  LSM6DS3_INST_PRINTF(MED, this,
                 "send_temp_config:: wm=%u age=%u invtl=%u offset=%x %08x", 
                 config_req.dae_watermark, (uint32_t)config_req.data_age_limit_ticks,
                 (uint32_t)config_req.polling_config.polling_interval_ticks,
                 (uint32_t)(config_req.polling_config.polling_offset >> 32),
                 (uint32_t)config_req.polling_config.polling_offset);

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
static bool flush_hw(lsm6ds3_dae_stream *dae_stream)
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
static bool flush_samples(lsm6ds3_dae_stream *dae_stream)
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
static bool stop_streaming(lsm6ds3_dae_stream *dae_stream)
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
  bool gyro_enabled = ((buf[1] & 0xF0) != 0);
  uint16_t num_sample_sets = gyro_enabled ? (buf_len / 12) : (buf_len / 6);
  lsm6ds3_accel_odr accel_odr = (lsm6ds3_accel_odr)(*buf & 0xF0);
  sns_time sampling_intvl = lsm6ds3_get_sample_interval(accel_odr);

  if(num_sample_sets >= 1 && sampling_intvl > 0)
  {
    sns_time first_timestamp = timestamp - sampling_intvl * (num_sample_sets - 1);
    lsm6ds3_process_fifo_data_buffer(this,
                                     gyro_enabled,
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
  if(buf_len == 3)
  {
    lsm6ds3_convert_and_send_temp_sample(this, timestamp, buf + 1);
  }
  else
  {
    LSM6DS3_INST_PRINTF(ERROR, this,
                   "Unexpected data len %u from DAE sensor", buf_len);
  }
}


/* ------------------------------------------------------------------------------------ */
static void process_data_event(
  sns_sensor_instance *this,
  lsm6ds3_dae_stream  *dae_stream,
  pb_istream_t        *pbstream)
{
  pb_buffer_arg decode_arg;
  sns_dae_data_event data_event = sns_dae_data_event_init_default;
  data_event.sensor_data.funcs.decode = &pb_decode_string_cb;
  data_event.sensor_data.arg = &decode_arg;
  if(pb_decode(pbstream, sns_dae_data_event_fields, &data_event))
  {
    lsm6ds3_instance_state *state = (lsm6ds3_instance_state*)this->state->state;
    lsm6ds3_dae_if_info* dae_if = &state->dae_if;
    if(dae_stream == &dae_if->ag)
    {
      if (!state->fifo_flush_in_progress)
      {
        state->interrupt_timestamp = data_event.timestamp; 
      }
      process_fifo_samples(
        this, data_event.timestamp, (uint8_t*)decode_arg.buf, decode_arg.buf_len);
    }
    else
    {
      process_temp_samples(
        this, data_event.timestamp, (uint8_t*)decode_arg.buf, decode_arg.buf_len);
    }

    if(LSM6DS3_CONFIG_IDLE == state->config_step && lsm6ds3_dae_if_stop_streaming(this))
    {
      state->config_step = LSM6DS3_CONFIG_STOPPING_STREAM;
    }
  }
}

/* ------------------------------------------------------------------------------------ */
static void process_interrupt_event(
  sns_sensor_instance *this,
  lsm6ds3_dae_stream  *dae_stream,
  pb_istream_t        *pbstream)
{
  lsm6ds3_instance_state *state = (lsm6ds3_instance_state*)this->state->state;

  LSM6DS3_INST_PRINTF(HIGH, this,
                 "process_interrupt_event:: stream=%x", dae_stream->stream);

  if(dae_stream == &state->dae_if.ag)
  {
    sns_dae_interrupt_event interrupt_event = sns_dae_interrupt_event_init_default;
    if(pb_decode(pbstream, sns_dae_interrupt_event_fields, &interrupt_event))
    {
      LSM6DS3_INST_PRINTF(HIGH, this,
                     "process_interrupt_event:: byte=%x", 
                     interrupt_event.registers.bytes[0]);
      lsm6ds3_handle_md_interrupt(this, interrupt_event.timestamp);
    }
    else
    {
      LSM6DS3_INST_PRINTF(ERROR, this,
                     "Failed decoding interrupt event");
    }
  }
  else
  {
    LSM6DS3_INST_PRINTF(ERROR, this, "Unexpected interrupt");
  }
}

/* ------------------------------------------------------------------------------------ */
static void process_response(
  sns_sensor_instance *this,
  lsm6ds3_dae_stream  *dae_stream,
  pb_istream_t        *pbstream)
{
  lsm6ds3_instance_state *state = (lsm6ds3_instance_state*)this->state->state;

  sns_dae_resp resp = sns_dae_resp_init_default;
  if(pb_decode(pbstream, sns_dae_resp_fields, &resp))
  {
    LSM6DS3_INST_PRINTF(MED, this,
                   "process_response:: msg=%u err=%u step=%u", 
                   resp.msg_id, resp.err, state->config_step);
    switch(resp.msg_id)
    {
      case SNS_DAE_MSGID_SNS_DAE_SET_STATIC_CONFIG:
        if(SNS_STD_ERROR_NO_ERROR != resp.err)
        {
          /* DAE sensor does not have support for this driver */
          dae_stream->stream_usable = false;
        }
        break;
      case SNS_DAE_MSGID_SNS_DAE_S4S_DYNAMIC_CONFIG:
        break;
      case SNS_DAE_MSGID_SNS_DAE_SET_STREAMING_CONFIG:
        if(dae_stream->stream != NULL && dae_stream->state == STREAM_STARTING)
        {
          dae_stream->state = (SNS_STD_ERROR_NO_ERROR == resp.err) ? STREAMING : IDLE;
        }
        if(dae_stream->state == STREAMING)
        {
          if(state->accel_info.curr_odr != LSM6DS3_ACCEL_ODR_OFF ||
             state->gyro_info.curr_odr  != LSM6DS3_GYRO_ODR_OFF)
          {
            lsm6ds3_enable_fifo_intr(state, state->fifo_info.fifo_enabled);
          }
        }
        break;
      case SNS_DAE_MSGID_SNS_DAE_FLUSH_HW:
        dae_stream->flushing_hw = false;
        state->fifo_flush_in_progress = false;
        if(LSM6DS3_CONFIG_IDLE == state->config_step && lsm6ds3_dae_if_stop_streaming(this))
        {
          state->config_step = LSM6DS3_CONFIG_STOPPING_STREAM;
        }

        break;
      case SNS_DAE_MSGID_SNS_DAE_FLUSH_DATA_EVENTS:
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
          lsm6ds3_dae_if_flush_samples(this);
          lsm6ds3_reconfig_hw(this);
        }
        break;
      case SNS_DAE_MSGID_SNS_DAE_PAUSE_S4S_SCHED:
        break;

      case SNS_DAE_MSGID_SNS_DAE_RESP:
      case SNS_DAE_MSGID_SNS_DAE_DATA_EVENT:
        break; /* unexpected */
      }
  }
}

/* ------------------------------------------------------------------------------------ */
static void process_events(sns_sensor_instance *this, lsm6ds3_dae_stream *dae_stream)
{
  sns_sensor_event *event;

  while(NULL != dae_stream->stream &&
        NULL != (event = dae_stream->stream->api->peek_input(dae_stream->stream)))
  {
    if (dae_stream->stream_usable)
    {
      pb_istream_t pbstream =
        pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);

      if (SNS_DAE_MSGID_SNS_DAE_DATA_EVENT == event->message_id)
      {
        process_data_event(this, dae_stream, &pbstream);
      }
      else if(SNS_DAE_MSGID_SNS_DAE_INTERRUPT_EVENT == event->message_id)
      {
        process_interrupt_event(this, dae_stream, &pbstream);
      }
      else if(SNS_DAE_MSGID_SNS_DAE_RESP == event->message_id)
      {
        process_response(this, dae_stream, &pbstream);
      }
      else if(SNS_STD_MSGID_SNS_STD_ERROR_EVENT == event->message_id)
      {
        LSM6DS3_INST_PRINTF(MED, this,
                       "process_events:: ERROR_EVENT stream=%x state=%u", 
                       dae_stream->stream, dae_stream->state);
        dae_stream->stream_usable = false;
        lsm6ds3_register_interrupt(this);
      }
      else
      {
        LSM6DS3_INST_PRINTF(ERROR, this,
                       "Unexpected message id %u", event->message_id);
      }
    }
    event = dae_stream->stream->api->get_next_input(dae_stream->stream);
  }
}

/*======================================================================================
  Public Functions
  ======================================================================================*/
bool lsm6ds3_dae_if_available(sns_sensor_instance *this)
{
  /* both streams must be availble */
  lsm6ds3_dae_if_info *dae_if = &((lsm6ds3_instance_state*)this->state->state)->dae_if;
  return (stream_usable(&dae_if->ag) && stream_usable(&dae_if->temp));
}

/* ------------------------------------------------------------------------------------ */
sns_rc lsm6ds3_dae_if_init(
  sns_sensor_instance  *const this,
  sns_stream_service   *stream_mgr,
  sns_sensor_uid       *dae_suid)
{
  sns_rc rc = SNS_RC_NOT_AVAILABLE;
  lsm6ds3_instance_state *state = (lsm6ds3_instance_state*)this->state->state;
  lsm6ds3_dae_if_info* dae_if = &state->dae_if;
  sns_dae_set_static_config config_req = sns_dae_set_static_config_init_default;

  stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                 this,
                                                 *dae_suid,
                                                 &dae_if->ag.stream);
  stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                 this,
                                                 *dae_suid,
                                                 &dae_if->temp.stream);

  rc = send_accel_static_config_request(this);
  if(rc == SNS_RC_SUCCESS && NULL != dae_if->temp.stream)
  {
    uint8_t encoded_msg[sns_dae_set_static_config_size];
    sns_request req = {
      .message_id  = SNS_DAE_MSGID_SNS_DAE_SET_STATIC_CONFIG,
      .request     = encoded_msg,
      .request_len = 0
    };

    dae_if->temp.nano_hal_vtable_name = "lsm6ds3_temperature_hal_table";
    sns_strlcpy(config_req.func_table_name,
        dae_if->temp.nano_hal_vtable_name,
                sizeof(config_req.func_table_name));
    config_req.ascp_config            = state->ascp_config;
      config_req.interrupt          = false;
      config_req.has_irq_config     = false;
      config_req.has_accel_info     = false;

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

  if(SNS_RC_SUCCESS != rc)
  {
    lsm6ds3_dae_if_deinit(state, stream_mgr);
  }
  else
  {
    dae_if->ag.stream_usable   = true;
    dae_if->temp.stream_usable = true;
  }
  LSM6DS3_INST_PRINTF(HIGH, this, "dae_if_init:: rc=%d", rc);
  return rc;
}

/* ------------------------------------------------------------------------------------ */
void lsm6ds3_dae_if_deinit(lsm6ds3_instance_state *state, sns_stream_service *stream_mgr)
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
bool lsm6ds3_dae_if_stop_streaming(sns_sensor_instance *this)
{
  bool cmd_sent = false;
  lsm6ds3_instance_state *state = (lsm6ds3_instance_state*)this->state->state;
  lsm6ds3_dae_if_info    *dae_if = &state->dae_if;

  if(stream_usable(&state->dae_if.ag) && 
     (dae_if->ag.state == STREAMING || dae_if->ag.state == STREAM_STARTING))
  {
    LSM6DS3_INST_PRINTF(MED, this,
                   "stop_streaming:: AG stream=%x", &dae_if->ag.stream);
    cmd_sent |= stop_streaming(&dae_if->ag);
  }

  if(stream_usable(&state->dae_if.temp) &&
     (dae_if->temp.state == STREAMING || dae_if->temp.state == STREAM_STARTING))
  {
    LSM6DS3_INST_PRINTF(MED, this,
                   "stop_streaming:: Temp stream=%x", &dae_if->temp.stream);
    cmd_sent |= stop_streaming(&dae_if->temp);
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
bool lsm6ds3_dae_if_start_streaming(sns_sensor_instance *this)
{
  bool cmd_sent = false;
  lsm6ds3_instance_state *state = (lsm6ds3_instance_state*)this->state->state;
  lsm6ds3_dae_if_info    *dae_if = &state->dae_if;

  LSM6DS3_INST_PRINTF(HIGH, this,
                 "start_streaming:: publish=%x tmp_intvl=%lu",
                 state->fifo_info.publish_sensors, state->sensor_temp_info.sampling_intvl);

  if(stream_usable(&dae_if->ag) && dae_if->ag.state != STREAMING &&
     (0 < state->accel_info.curr_odr || 0 < state->gyro_info.curr_odr))
  {
    LSM6DS3_INST_PRINTF(MED, this,
                   "start_streaming:: AG stream=%x", &dae_if->ag.stream);
    cmd_sent |= send_ag_config(&dae_if->ag, state);
  }

  if(stream_usable(&dae_if->temp) && dae_if->temp.state != STREAMING &&
     0 < state->sensor_temp_info.sampling_intvl)
  {
    LSM6DS3_INST_PRINTF(MED, this,
                   "start_streaming:: Temp stream=%x", &dae_if->temp.stream);
    cmd_sent |= send_temp_config(&dae_if->temp, this);
  }

  state->config_step = LSM6DS3_CONFIG_IDLE;
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
bool lsm6ds3_dae_if_flush_hw(sns_sensor_instance *this)
{
  lsm6ds3_dae_if_info *dae_if = &((lsm6ds3_instance_state*)this->state->state)->dae_if;
  bool cmd_sent = dae_if->ag.flushing_hw;

  if(stream_usable(&dae_if->ag) && !dae_if->ag.flushing_hw)
  {
    cmd_sent |= flush_hw(&dae_if->ag);
    LSM6DS3_INST_PRINTF(MED, this,
                   "flush_hw:: AG stream=%x flushing=%u", 
                   &dae_if->ag.stream, dae_if->ag.flushing_hw);
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
bool lsm6ds3_dae_if_flush_samples(sns_sensor_instance *this)
{
  lsm6ds3_dae_if_info *dae_if = &((lsm6ds3_instance_state*)this->state->state)->dae_if;
  bool cmd_sent = dae_if->ag.flushing_data | dae_if->temp.flushing_data;

  if(stream_usable(&dae_if->ag))
  {
    if(!dae_if->ag.flushing_data)
    {
      flush_samples(&dae_if->ag);
    }
    cmd_sent |= dae_if->ag.flushing_data;
    LSM6DS3_INST_PRINTF(MED, this,
                   "flush_samples:: AG stream=%x flushing=%u", 
                   &dae_if->ag.stream, dae_if->ag.flushing_data);
  }

  if(stream_usable(&dae_if->temp))
  {
    if(!dae_if->temp.flushing_data)
    {
      flush_samples(&dae_if->temp);
    }
    cmd_sent |= dae_if->temp.flushing_data;
    LSM6DS3_INST_PRINTF(MED, this,
                   "flush_samples:: Temp stream=%x flushing=%u", 
                   &dae_if->temp.stream, dae_if->temp.flushing_data);
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
void lsm6ds3_dae_if_process_events(sns_sensor_instance *this)
{
  lsm6ds3_instance_state *state = (lsm6ds3_instance_state*)this->state->state;
  process_events(this, &state->dae_if.ag);
  process_events(this, &state->dae_if.temp);

  if(!stream_usable(&state->dae_if.ag) || !stream_usable(&state->dae_if.temp))
  {
    /* both streams are needed; if one was removed, remove the other one too */
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_mgr =
      (sns_stream_service*)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    lsm6ds3_dae_if_deinit(state, stream_mgr);
  }
}

