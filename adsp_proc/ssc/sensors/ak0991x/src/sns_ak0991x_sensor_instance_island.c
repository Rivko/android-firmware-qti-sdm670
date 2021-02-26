/**
 * @file sns_ak0991x_sensor_instance_island.c
 *
 * AK0991X Mag virtual Sensor Instance implementation.
 *
 * Copyright (c) 2016-2018 Asahi Kasei Microdevices
 * All Rights Reserved.
 *
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 **/

#include "sns_island_service.h"
#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_time.h"
#include "sns_sensor_event.h"
#include "sns_types.h"

#include "sns_ak0991x_hal.h"
#include "sns_ak0991x_sensor.h"
#include "sns_ak0991x_sensor_instance.h"
#include "sns_ak0991x_s4s.h"

#include "sns_interrupt.pb.h"
#include "sns_async_com_port.pb.h"
#include "sns_timer.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_sync_com_port_service.h"
#ifdef AK0991X_ENABLE_DIAG_LOGGING
#include "sns_diag_service.h"
#include "sns_diag.pb.h"
#endif

extern log_sensor_state_raw_info log_mag_state_raw_info;

const odr_reg_map reg_map_ak0991x[AK0991X_REG_MAP_TABLE_SIZE] = {
  {
    .odr = AK0991X_ODR_0,
    .mag_odr_reg_value = AK0991X_MAG_ODR_OFF,
  },
  {
    .odr = AK0991X_ODR_1,
    .mag_odr_reg_value = AK0991X_MAG_ODR1,
  },
  {
    .odr = AK0991X_ODR_10,
    .mag_odr_reg_value = AK0991X_MAG_ODR10,
  },
  {
    .odr = AK0991X_ODR_20,
    .mag_odr_reg_value = AK0991X_MAG_ODR20,
  },
  {
    .odr = AK0991X_ODR_50,
    .mag_odr_reg_value = AK0991X_MAG_ODR50,
  },
  {
    .odr = AK0991X_ODR_100,
    .mag_odr_reg_value = AK0991X_MAG_ODR100,
  },
  {
    .odr = AK0991X_ODR_200,
    .mag_odr_reg_value = AK0991X_MAG_ODR200,
  }
};

static void ak0991x_inst_exit_island(sns_sensor_instance *this)
{
  sns_service_manager *smgr = this->cb->get_service_manager(this);
  sns_island_service  *island_svc  =
    (sns_island_service *)smgr->get_service(smgr, SNS_ISLAND_SERVICE);
  island_svc->api->sensor_instance_island_exit(island_svc, this);
}

#ifdef AK0991X_ENABLE_FIFO
static void ak0991x_process_com_port_vector(sns_port_vector *vector,
                                     void *user_arg)
{
  sns_sensor_instance *instance = (sns_sensor_instance *)user_arg;

  ak0991x_instance_state *state = (ak0991x_instance_state *)instance->state->state;

  if (AKM_AK0991X_REG_HXL == vector->reg_addr)
  {
    if(vector->bytes != 0){
      ak0991x_process_mag_data_buffer(instance,
                                      state->first_data_ts_of_batch,
                                      state->averaged_interval,
                                      vector->buffer,
                                      vector->bytes);
    }
    else{
      AK0991X_INST_PRINT(LOW, instance, "skip ak0991x_process_mag_data_buffer because vector->bytes=%d detected.", vector->bytes);
    }
  }
}
#endif

static sns_rc ak0991x_heart_beat_timer_event(sns_sensor_instance *const this)
{
  ak0991x_instance_state *state =
    (ak0991x_instance_state *)this->state->state;
  sns_rc rv = SNS_RC_SUCCESS;

  if(state->mag_info.desired_odr != AK0991X_MAG_ODR_OFF)
  {
    if (state->mag_info.use_dri)
    {
#ifdef AK0991X_ENABLE_DRI
      AK0991X_INST_PRINT(HIGH, this, "Detect streaming has stopped #HB= %u start_time= %u period = %u fire_time %u now= %u",
                         state->heart_beat_attempt_count,
                         (uint32_t)state->req_payload.start_time,
                         (uint32_t)state->req_payload.timeout_period,
                         (uint32_t)state->hb_timer_fire_time,
                         (uint32_t)state->system_time);

      // Streaming is unable to resume after 4 attempts
      if (state->heart_beat_attempt_count >= 4)
      {
        ak0991x_inst_exit_island(this);
        SNS_INST_PRINTF(ERROR, this, "Streaming is unable to resume after 3 attempts");
        rv = SNS_RC_INVALID_STATE;
      }
      // Perform a reset operation in an attempt to revive the sensor
      else
      {
        state->heart_beat_attempt_count++;
        ak0991x_read_mag_samples(this);

        if(state->heart_beat_attempt_count >= 3)
        {
          ak0991x_inst_exit_island(this);
          rv = ak0991x_device_sw_reset(this,
                                       state->scp_service,
                                       state->com_port_info.port_handle);
          if (rv != SNS_RC_SUCCESS)
          {
            SNS_INST_PRINTF(ERROR, this, "soft reset failed");
          }
          else
          {
            AK0991X_INST_PRINT(LOW, this, "soft reset called");
          }
          // Indicate streaming error
          rv = SNS_RC_NOT_AVAILABLE;
          ak0991x_reconfig_hw(this);
        }
      }
#endif // AK0991X_ENABLE_DRI
    }
    else
    {
      uint8_t heart_beat_thresthold =
        ( state->mag_info.use_fifo )? 1 : 4;
      if (state->heart_beat_sample_count < heart_beat_thresthold)
      {
        state->heart_beat_sample_count++;
      }
      else
      {
        AK0991X_INST_PRINT(LOW, this, "heart_beat_gap=%u, heart_beat_timeout=%u",
          (uint32_t)((state->interrupt_timestamp-state->heart_beat_timestamp)/19200),
          (uint32_t)(state->heart_beat_timeout_period/19200));
        // Detect streaming has stopped
        if (state->interrupt_timestamp > state->heart_beat_timestamp + state->heart_beat_timeout_period)
        {
          AK0991X_INST_PRINT(HIGH, this, "Detect streaming has stopped");
          // Streaming is unable to resume after 3 attempts
          if (state->heart_beat_attempt_count >= 3)
          {
            ak0991x_inst_exit_island(this);
            SNS_INST_PRINTF(ERROR, this, "Streaming is unable to resume after 3 attempts");
            rv = SNS_RC_INVALID_STATE;
          }
          // Perform a reset operation in an attempt to revive the sensor
          else
          {
            ak0991x_inst_exit_island(this);
            rv = ak0991x_device_sw_reset(this,
                                         state->scp_service,
                                         state->com_port_info.port_handle);
            if (rv == SNS_RC_SUCCESS) {
              AK0991X_INST_PRINT(LOW, this, "soft reset called");
            } else {
              SNS_INST_PRINTF(ERROR, this, "soft reset failed");
            }
            // Indicate streaming error
            rv = SNS_RC_NOT_AVAILABLE;
            ak0991x_reconfig_hw(this);
            state->heart_beat_attempt_count++;
          }
        }
        else
        {
          state->heart_beat_timestamp = state->interrupt_timestamp;
          state->heart_beat_sample_count = 0;
          state->heart_beat_attempt_count = 0;
        }
      }
    }
  }
  else
  {
    SNS_INST_PRINTF(ERROR, this, "heart beat timer event is skipped since ODR=0.");
  }

  return rv;
}

void ak0991x_switch_cal_data(sns_sensor_instance *const instance)
{
#ifdef AK0991X_ENABLE_DEVICE_MODE_SENSOR
  float *fac_cal_bias = NULL;
  matrix3 *fac_cal_corr_mat = NULL;
  ak0991x_instance_state *inst_state = (ak0991x_instance_state *)instance->state->state;

  uint32_t cal_id = ak0991x_device_mode2cal_id(instance);
  fac_cal_bias     = inst_state->cal_parameter[cal_id].fac_cal_bias;
  fac_cal_corr_mat = &inst_state->cal_parameter[cal_id].fac_cal_corr_mat;

  for (int i = 0; i < TRIAXIS_NUM; i++)
  {
    inst_state->mag_registry_cfg.fac_cal_bias[i] = fac_cal_bias[i];
  }
  for (int i = 0; i < ARR_SIZE(fac_cal_corr_mat->data); i++)
  {
    inst_state->mag_registry_cfg.fac_cal_corr_mat.data[i] = fac_cal_corr_mat->data[i];
  }
#else // AK0991X_ENABLE_DEVICE_MODE_SENSOR
  UNUSED_VAR(instance);
#endif // AK0991X_ENABLE_DEVICE_MODE_SENSOR
}

#ifdef AK0991X_ENABLE_DEVICE_MODE_SENSOR
bool sns_device_mode_event_decode_cb(pb_istream_t *stream, const pb_field_t *field,
    void **arg)
{
  UNUSED_VAR(field);
  ak0991x_instance_state *state = (ak0991x_instance_state *)*arg;
  if(!pb_decode(stream, sns_device_mode_event_mode_spec_fields, &state->device_mode[state->device_mode_cnt]))
  {
    return false;
  }
  else
  {
    state->device_mode_cnt++;
  }
  return true;
}
#endif // AK0991X_ENABLE_DEVICE_MODE_SENSOR

sns_rc ak0991x_handle_device_mode_stream(sns_sensor_instance *const this)
{
#ifdef AK0991X_ENABLE_DEVICE_MODE_SENSOR
  ak0991x_instance_state *state = (ak0991x_instance_state *)this->state->state;
  sns_sensor_event    *event;
  sns_rc rv = SNS_RC_FAILED;

  if (NULL != state->device_mode_stream)
  {
    event = state->device_mode_stream->api->peek_input(state->device_mode_stream);

    while (NULL != event)
    {
      if(event->message_id == SNS_DEVICE_MODE_MSGID_SNS_DEVICE_MODE_EVENT)
      {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t *)event->event,
                                                   event->event_len);
        sns_device_mode_event device_mode_event = sns_device_mode_event_init_default;
        state->device_mode_cnt = 0;
        device_mode_event.device_mode.arg = (void*)state;
        device_mode_event.device_mode.funcs.decode = sns_device_mode_event_decode_cb;
        if(!pb_decode(&stream, sns_device_mode_event_fields, &device_mode_event))
        {
          SNS_INST_PRINTF(ERROR, this, "Error in decoding device mode event!");
        }
        else
        {
          AK0991X_INST_PRINT(LOW, this, "Received %u device mode specs", state->device_mode_cnt);
          for(int i = 0; i < state->device_mode_cnt; ++i)
          {
            AK0991X_INST_PRINT(LOW, this, "mode: %u, state: %u", state->device_mode[i].mode,
                                                                 state->device_mode[i].state);
          }
          state->cal_id = ak0991x_device_mode2cal_id(this);
          rv = SNS_RC_SUCCESS;
        }
      }
      event = state->device_mode_stream->api->get_next_input(state->device_mode_stream);
    }
  }
#else // AK0991X_ENABLE_DEVICE_MODE_SENSOR
  sns_rc rv = SNS_RC_FAILED;
  UNUSED_VAR(this);
#endif // AK0991X_ENABLE_DEVICE_MODE_SENSOR

  return rv;
}

/** See sns_sensor_instance_api::notify_event */
static sns_rc ak0991x_inst_notify_event(sns_sensor_instance *const this)
{
  ak0991x_instance_state *state =
    (ak0991x_instance_state *)this->state->state;
  sns_sensor_event    *event;
  sns_rc rv = SNS_RC_SUCCESS;

  // Turn COM port ON
  state->scp_service->api->sns_scp_update_bus_power(
    state->com_port_info.port_handle,
    true);

  ak0991x_dae_if_process_events(this);

#ifdef AK0991X_ENABLE_DRI
  // Handle interrupts
  sns_interrupt_event irq_event = sns_interrupt_event_init_zero;
  if (NULL != state->interrupt_data_stream)
  {
    event = state->interrupt_data_stream->api->peek_input(
        state->interrupt_data_stream);

    while (NULL != event)
    {
      if (SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT == event->message_id)
      {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t *)event->event,
                                                     event->event_len);

        if(pb_decode(&stream, sns_interrupt_event_fields, &irq_event))
        {
          if(!state->in_clock_error_procedure)
          {
            // check DRDY status.
            ak0991x_get_st1_status(this);

            if(state->data_is_ready)
            {
              state->irq_event_time = irq_event.timestamp;
              state->irq_info.detect_irq_event = true; // detect interrupt
              state->system_time = sns_get_system_time();
#ifdef AK0991X_VERBOSE_DEBUG
              AK0991X_INST_PRINT(LOW, this, "Data is ready. Detect interrupt.");
#endif

              if(state->system_time > irq_event.timestamp + state->averaged_interval)
              {
                AK0991X_INST_PRINT(MED, this, "Delayed interrupt event. irq_event %u, now=%u",
                                   (uint32_t)irq_event.timestamp,
                                   (uint32_t)state->system_time);
              }

              if(state->ascp_xfer_in_progress == 0)
              {
                ak0991x_read_mag_samples(this);
              }
              else
              {
                AK0991X_INST_PRINT(LOW, this, "ascp_xfer_in_progress=%d.",state->ascp_xfer_in_progress);
                state->re_read_data_after_ascp = true;
              }
            }
          }
          else
          {
            state->irq_event_time = irq_event.timestamp;
            ak0991x_clock_error_calc_procedure(this);
          }
        }
        else
        {
          SNS_INST_PRINTF(ERROR, this, "Failed decoding interrupt event");
        }
      }
      else if (SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT == event->message_id)
      {
        state->irq_info.is_ready = true;
        ak0991x_reconfig_hw(this);
      }
      else
      {
        AK0991X_INST_PRINT(ERROR, this, "Received invalid event id=%d",
                                      event->message_id);
      }
      event = state->interrupt_data_stream->api->get_next_input(state->interrupt_data_stream);

      if(NULL != event)
      {
        AK0991X_INST_PRINT(ERROR, this, "Still have int event in the queue... %u DRDY= %d", (uint32_t)sns_get_system_time(), state->data_is_ready);
      }
    }
  }
#endif // AK0991X_ENABLE_DRI

#ifdef AK0991X_ENABLE_FIFO
  // Handle Async Com Port events
  if (NULL != state->async_com_port_data_stream)
  {
    event = state->async_com_port_data_stream->api->peek_input(state->async_com_port_data_stream);

    while (NULL != event)
    {
      if (SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW == event->message_id)
      {
        pb_istream_t stream = pb_istream_from_buffer((uint8_t *)event->event, event->event_len);

        sns_ascp_for_each_vector_do(&stream, ak0991x_process_com_port_vector, (void *)this);

        if(state->ascp_xfer_in_progress>0)
        {
          state->ascp_xfer_in_progress--;
        }
        else
        {
          AK0991X_INST_PRINT(ERROR, this, "ascp_xfer_in_progress is already 0. Can't decriment.");
        }

        if(state->ascp_xfer_in_progress != 0)
        {
          AK0991X_INST_PRINT(LOW, this, "ascp_xfer_in_progress = %d", state->ascp_xfer_in_progress);
        }

        if(state->re_read_data_after_ascp && (state->ascp_xfer_in_progress == 0))
        {
          ak0991x_send_fifo_flush_done(this);
          state->re_read_data_after_ascp = false;
        }

        if(state->config_mag_after_ascp_xfer)
        {
          ak0991x_start_mag_streaming(this);
          state->config_mag_after_ascp_xfer = false;
        }
      }
      else if (SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_ERROR == event->message_id)
      {
        SNS_INST_PRINTF(ERROR, this, "Received ASYNC_COM_PORT_ERROR");
      }

      event = state->async_com_port_data_stream->api->get_next_input(
          state->async_com_port_data_stream);
    }
  }
#endif // AK0991X_ENABLE_FIFO

  // Handle timer event
  if (NULL != state->timer_data_stream)
  {
    event = state->timer_data_stream->api->peek_input(state->timer_data_stream);

    while (NULL != event)
    {
      if (SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT == event->message_id)
      {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t *)event->event,
                                                     event->event_len);
        sns_timer_sensor_event timer_event;

        if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
        {
          sns_time now = sns_get_system_time();
          state->system_time = timer_event.requested_timeout_time;
          if(state->system_time + state->nominal_intvl < now )
          {
            SNS_INST_PRINTF(ERROR, this, "req_to=%u now=%u", 
                            (uint32_t)timer_event.requested_timeout_time, (uint32_t)now);
          }
		  
          // for regular polling mode
          if (!state->mag_info.use_dri && state->reg_event_done)
          {
            AK0991X_INST_PRINT(LOW, this, "Execute handle timer event. now %u req_timeout_time %u",
                       (uint32_t)now,
                       (uint32_t)state->system_time);

            // mag data read
            ak0991x_read_mag_samples(this);
          }
          else
          {
            // reset system time for heart beat timer on the DRI mode
            state->system_time = now;
          }

          // check heart beat fire time
          if(now > state->hb_timer_fire_time)
          {
            rv = ak0991x_heart_beat_timer_event(this);
          }
          else
          {
            SNS_INST_PRINTF(ERROR, this, "Wrong HB timer fired. fire_time %u now %u",(uint32_t)state->hb_timer_fire_time, (uint32_t)now );
          }
        }
        else
        {
          SNS_INST_PRINTF(ERROR, this, "Failed decoding timer event");
        }
      }
      else if (SNS_TIMER_MSGID_SNS_TIMER_SENSOR_REG_EVENT == event->message_id)
      {
        //TODO:add support for handling SNS_TIMER_SENSOR_REG_EVENT timer event to successfully support S4S
        // When a range of start times is provided to the timer sensor, the timer sensor will pick a specific time.
        // That specific time will be returned in the SNS_TIMER_SENSOR_REG_EVENT event --
        // and will be needed by the mag sensor to populate the fields sent to the DAE sensor(so that timers remain synchronized in the DAE environment),
        // and the field in the Physical Sensor Config event (which needs absolute timing for the future events).
        if(!state->mag_info.use_dri)
        {
          state->reg_event_done = true;
        }
      }
      else
      {
        AK0991X_INST_PRINT(ERROR, this, "Received invalid event id=%d", event->message_id);
      }

      if(NULL != state->timer_data_stream)
      {
        event = state->timer_data_stream->api->get_next_input(state->timer_data_stream);
      }
    }
  }

  //Handle device_mode stream events
  if( ak0991x_handle_device_mode_stream(this) == SNS_RC_SUCCESS )
  {
    // switch
    ak0991x_switch_cal_data(this);

    // report
    ak0991x_send_cal_event(this);
  }

  // Handle timer data stream for S4S
  ak0991x_s4s_handle_timer_data_stream(this);

  // Turn COM port OFF
  state->scp_service->api->sns_scp_update_bus_power(
    state->com_port_info.port_handle,
    false);
  return rv;
}

/** Public Data Definitions. */

sns_sensor_instance_api ak0991x_sensor_instance_api = {
  .struct_len        = sizeof(sns_sensor_instance_api),
  .init              = &ak0991x_inst_init,
  .deinit            = &ak0991x_inst_deinit,
  .set_client_config = &ak0991x_inst_set_client_config,
  .notify_event      = &ak0991x_inst_notify_event
};
