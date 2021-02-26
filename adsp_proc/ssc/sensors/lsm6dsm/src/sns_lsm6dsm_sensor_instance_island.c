/**
 * @file sns_lsm6dsm_sensor_instance.c
 *
 * LSM6DSM Accel virtual Sensor Instance implementation.
 *
 * Copyright (c) 2018, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the STMicroelectronics nor the
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
 *
 **/

#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_event_service.h"
#include "sns_island_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_time.h"
#include "sns_sensor_event.h"
#include "sns_types.h"

#include "sns_lsm6dsm_hal.h"
#include "sns_lsm6dsm_sensor.h"
#include "sns_lsm6dsm_sensor_instance.h"

#include "sns_interrupt.pb.h"
#include "sns_async_com_port.pb.h"
#include "sns_timer.pb.h"
#include "sns_sensor_util.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_diag_service.h"
#include "sns_diag.pb.h"
#include "sns_printf.h"

/* accel_discard_samples = AN4987 value + 2
   gyro_discard_samples =  AN4987 value + 2 + LSM6DSM_GYRO_ON_TIME_MS equivalent samples
*/
#define MAX_HEART_ATTACKS 6
const odr_reg_map lsm6dsm_odr_map[LSM6DSM_REG_MAP_TABLE_SIZE] =
{
  {
    .odr = LSM6DSM_ODR_0,
    .accel_odr_reg_value = LSM6DSM_ACCEL_ODR_OFF,
    .gyro_odr_reg_value = LSM6DSM_GYRO_ODR_OFF,
    .accel_discard_samples = 0,
    .gyro_discard_samples = 0
  },
  {
    .odr = LSM6DSM_ODR_13,
    .accel_odr_reg_value = LSM6DSM_ACCEL_ODR13,
    .gyro_odr_reg_value = LSM6DSM_GYRO_ODR13,
    .accel_discard_samples = 3,
    .gyro_discard_samples = 3
  },
  {
    .odr = LSM6DSM_ODR_26,
    .accel_odr_reg_value = LSM6DSM_ACCEL_ODR26,
    .gyro_odr_reg_value = LSM6DSM_GYRO_ODR26,
    .accel_discard_samples = 3,
    .gyro_discard_samples = 4
  },
  {
    .odr = LSM6DSM_ODR_52,
    .accel_odr_reg_value = LSM6DSM_ACCEL_ODR52,
    .gyro_odr_reg_value = LSM6DSM_GYRO_ODR52,
    .accel_discard_samples = 3,
    .gyro_discard_samples = 4
  },
  {
    .odr = LSM6DSM_ODR_104,
    .accel_odr_reg_value = LSM6DSM_ACCEL_ODR104,
    .gyro_odr_reg_value = LSM6DSM_GYRO_ODR104,
    .accel_discard_samples = 4,
    .gyro_discard_samples = 4
  },
  {
    .odr = LSM6DSM_ODR_208,
    .accel_odr_reg_value = LSM6DSM_ACCEL_ODR208,
    .gyro_odr_reg_value = LSM6DSM_GYRO_ODR208,
    .accel_discard_samples = 4,
    .gyro_discard_samples = 4
  },
  {
    .odr = LSM6DSM_ODR_416,
    .accel_odr_reg_value = LSM6DSM_ACCEL_ODR416,
    .gyro_odr_reg_value = LSM6DSM_GYRO_ODR416,
    .accel_discard_samples = 4,
    .gyro_discard_samples = 4
  },
  {
    .odr = LSM6DSM_ODR_833,
    .accel_odr_reg_value = LSM6DSM_ACCEL_ODR833,
    .gyro_odr_reg_value = LSM6DSM_GYRO_ODR833,
    .accel_discard_samples = 4,
    .gyro_discard_samples = 4
  },
  {
    .odr = LSM6DSM_ODR_1660,
    .accel_odr_reg_value = LSM6DSM_ACCEL_ODR1660,
    .gyro_odr_reg_value = LSM6DSM_GYRO_ODR1660,
    .accel_discard_samples = 4,
    .gyro_discard_samples = 136
  },
  {
    .odr = LSM6DSM_ODR_3330,
    .accel_odr_reg_value = LSM6DSM_ACCEL_ODR3330,
    .gyro_odr_reg_value = LSM6DSM_GYRO_ODR1660,
    .accel_discard_samples = 6,
    .gyro_discard_samples = 271
  },
  {
    .odr = LSM6DSM_ODR_6660,
    .accel_odr_reg_value = LSM6DSM_ACCEL_ODR6660,
    .gyro_odr_reg_value = LSM6DSM_GYRO_ODR1660,
    .accel_discard_samples = 15,
    .gyro_discard_samples = 541
  },
};

static void inst_exit_island(sns_sensor_instance *this)
{
  sns_service_manager *smgr = this->cb->get_service_manager(this);
  sns_island_service  *island_svc  =
    (sns_island_service *)smgr->get_service(smgr, SNS_ISLAND_SERVICE);
  island_svc->api->sensor_instance_island_exit(island_svc, this);
}

void lsm6dsm_inst_create_timer(sns_sensor_instance *this,
    sns_data_stream** timer_data_stream,
    sns_time timeout_ticks,
    bool is_periodic)
{
  lsm6dsm_instance_state *state = (lsm6dsm_instance_state*)this->state->state;
  if(timer_data_stream == NULL) {
    DBG_INST_PRINT(this, ERROR, __FILENAME__, __LINE__,
        "Invalid timer data stream pointer passed");
    return;
  }

  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  size_t req_len;
  uint8_t buffer[50];
  sns_memset(buffer, 0, sizeof(buffer));
  req_payload.is_periodic = is_periodic;
  req_payload.start_time = sns_get_system_time();
  req_payload.timeout_period = timeout_ticks;

  req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
      sns_timer_sensor_config_fields, NULL);
  if(req_len > 0)
  {
    if(*timer_data_stream == NULL)
    {
      sns_service_manager *service_mgr = this->cb->get_service_manager(this);
      sns_stream_service *stream_mgr = (sns_stream_service*)
        service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
      stream_mgr->api->create_sensor_instance_stream(stream_mgr,
          this,
          state->timer_suid,
          timer_data_stream);
      DBG_INST_PRINT(this, MED, __FILENAME__, __LINE__,
          "creating timer stream!");
    }
    if(*timer_data_stream == NULL)
    {
      SNS_INST_PRINTF(HIGH, this,
          "Error creating timer stream!");
      return;
    }
    sns_request timer_req =
    {  .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
      .request = buffer, .request_len = req_len  };
    (*timer_data_stream)->api->send_request(*timer_data_stream, &timer_req);
    LSM6DSM_INST_DEBUG_TS(MED, this, __FILENAME__, __LINE__,
        "set timer start_time=%u timeout_period=%u",(uint32_t)req_payload.start_time, (uint32_t)req_payload.timeout_period);
  }
  else
  {
    SNS_INST_PRINTF(ERROR, this, "LSM timer req encode error");
  }
}

static sns_rc lsm6dsm_match_odr(lsm6dsm_sensor_type sensor,
    float desired_sample_rate,
    uint8_t* rate_idx,
    float *chosen_sample_rate,
    uint8_t *chosen_reg_value,
    uint16_t *num_samples_to_discard)
{
  uint8_t idx;

  if((LSM6DSM_ODR_833 < desired_sample_rate)
     ||
     NULL == chosen_sample_rate
     ||
     NULL == chosen_reg_value)
  {
    return SNS_RC_NOT_SUPPORTED;
  }

  for(idx = 0; idx < ARR_SIZE(lsm6dsm_odr_map); idx++)
  {
    if(desired_sample_rate <= lsm6dsm_odr_map[idx].odr)
    {
      break;
    }
  }

  if (idx >= ARR_SIZE(lsm6dsm_odr_map))
  {
    return SNS_RC_NOT_SUPPORTED;
  }

  *chosen_sample_rate = lsm6dsm_odr_map[idx].odr;
  *rate_idx = idx;

  if(sensor == LSM6DSM_ACCEL) {
    *chosen_reg_value = lsm6dsm_odr_map[idx].accel_odr_reg_value;
    *num_samples_to_discard = lsm6dsm_odr_map[idx].accel_discard_samples;
    *chosen_sample_rate = lsm6dsm_odr_map[idx].odr;
  } else if(sensor == LSM6DSM_GYRO) {
    *chosen_sample_rate = lsm6dsm_odr_map[idx].odr;
    *chosen_reg_value = lsm6dsm_odr_map[idx].gyro_odr_reg_value;
    //consider worst case +10%
    *num_samples_to_discard = (1.1 * lsm6dsm_odr_map[idx].odr * LSM6DSM_GYRO_ON_TIME_MS )/1000 + lsm6dsm_odr_map[idx].gyro_discard_samples;
  } else
    return SNS_RC_INVALID_VALUE;

  return SNS_RC_SUCCESS;
}


static sns_rc lsm6dsm_validate_sensor_temp_odr(lsm6dsm_instance_state *state)
{
  sns_rc rc = SNS_RC_SUCCESS;
  if(state->sensor_temp_info.desired_sampling_rate_hz <= LSM6DSM_SENSOR_TEMP_ODR_1)
  {
    state->sensor_temp_info.desired_sampling_rate_hz = LSM6DSM_SENSOR_TEMP_ODR_1;
  }
  else if(state->sensor_temp_info.desired_sampling_rate_hz > LSM6DSM_SENSOR_TEMP_ODR_1
          &&
          state->sensor_temp_info.desired_sampling_rate_hz <= LSM6DSM_SENSOR_TEMP_ODR_5)
  {
    state->sensor_temp_info.desired_sampling_rate_hz = LSM6DSM_SENSOR_TEMP_ODR_5;
  }
  else
  {
    state->sensor_temp_info.sampling_intvl = 0;
    rc = SNS_RC_NOT_SUPPORTED;
  }

  if (rc == SNS_RC_SUCCESS)
  {
    state->sensor_temp_info.sampling_intvl =
      sns_convert_ns_to_ticks(1000000000.0 / state->sensor_temp_info.desired_sampling_rate_hz);
  }

  return rc;
}

static void lsm6dsm_handle_hw_interrupts(sns_sensor_instance *const this)
{
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)this->state->state;
  sns_interrupt_event irq_event = sns_interrupt_event_init_zero;
  sns_sensor_event *event;

  // Handle interrupts
  if(NULL != state->interrupt_data_stream)
  {
    event = state->interrupt_data_stream->api->peek_input(state->interrupt_data_stream);
    while(NULL != event)
    {
      if (SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT == event->message_id)
      {
        DBG_INST_PRINT_EX(this, MED, __FILENAME__, __LINE__,
                       "INT reg");

        state->irq_info.irq_ready = true;
        if(state->md_info.enable_md_int)
        {
          lsm6dsm_update_md_intr(this, true, false);
        }
        if((state->fifo_info.publish_sensors & (LSM6DSM_ACCEL | LSM6DSM_GYRO)) ||
            (state->accel_info.gated_client_present && !state->md_info.enable_md_int)) {
          lsm6dsm_enable_fifo_intr(this, state->fifo_info.fifo_enabled);
        }
      }
      else if(SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT == event->message_id)
      {
        //set this once entered in irq handle
        state->fifo_info.recheck_int = false;

        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);
        if(pb_decode(&stream, sns_interrupt_event_fields, &irq_event))
        {
          if(state->fifo_info.reconfig_req)
            DBG_INST_PRINT_EX(this, MED, __FILENAME__, __LINE__,
                "HW INT");

          //set heart_attack false
          state->health.heart_attack = false;
          state->health.heart_attack_cnt = 0;
          //reset the timer
          // QC: Should be done after handling the event so as not to add any delay to async com port request.
          lsm6dsm_inst_create_timer(this, &state->timer_heart_beat_data_stream, state->heart_beat_timeout, true);

          if((state->md_info.enable_md_int) && (state->md_info.is_filter_settled) && (!state->self_test_info.test_alive))
          {
            lsm6dsm_handle_md_interrupt(this, irq_event.timestamp, NULL);
          }

          if(state->fifo_info.cur_wmk > 0
              && state->fifo_info.fifo_rate > LSM6DSM_ACCEL_ODR_OFF)
          {
            DBG_INST_PRINT(this, MED, __FILENAME__, __LINE__,
                "DSM INT. irq_time=%u",(uint32_t)(irq_event.timestamp));
            state->fifo_info.th_info.interrupt_fired = true;
            state->fifo_info.th_info.interrupt_ts = irq_event.timestamp;
            lsm6dsm_read_fifo_data(this, irq_event.timestamp, false);
          }
        }
      }
      else
      {
        SNS_INST_PRINTF(ERROR, this, "invalid msg %d",
            event->message_id);
      }
      event = state->interrupt_data_stream->api->get_next_input(state->interrupt_data_stream);
    }
  }
}

void lsm6dsm_send_interrupt_is_cleared_msg(sns_sensor_instance *const this)
{
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)this->state->state;
  sns_data_stream* data_stream = state->interrupt_data_stream;
  sns_request irq_msg =
  {
    .message_id = SNS_INTERRUPT_MSGID_SNS_INTERRUPT_IS_CLEARED,
    .request    = NULL
  };

  data_stream->api->send_request(data_stream, &irq_msg);
}
/** this function executes for handling fifo read data only
 * driver make sure to have only one ascp com port req at a time*/
static void lsm6dsm_handle_ascp_events(sns_sensor_instance *const this)
{
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)this->state->state;
  sns_sensor_event *event = NULL;
  uint32_t port_rw_events_rcvd = 0;

  if(NULL == state->async_com_port_data_stream ||
     NULL == (event = state->async_com_port_data_stream->api->peek_input(state->async_com_port_data_stream))) {
    return;
  }

  // Handle Async Com Port events
  while(NULL != event)
  {
    if(SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_ERROR == event->message_id)
    {
      SNS_INST_PRINTF(ERROR, this, "ASCP error=%d",
          event->message_id);
    }
    else if((SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW == event->message_id) && state->ascp_req_count)
    {

      if(state->fifo_info.reconfig_req)
        DBG_INST_PRINT(this, HIGH, __FILENAME__, __LINE__,
            "ASCP VEC_RW req_cnt=%d", state->ascp_req_count);
      state->ascp_req_count--;

      //is this timestamp accurate to use?
      state->fifo_info.ascp_event_timestamp = event->timestamp;
      pb_istream_t stream = pb_istream_from_buffer((uint8_t *)event->event, event->event_len);
      sns_ascp_for_each_vector_do(&stream, lsm6dsm_process_com_port_vector, (void *)this);

      if(state->fifo_info.bh_info.flush_client_present) {
        if((state->fifo_info.publish_sensors & LSM6DSM_ACCEL) ||
             (state->accel_info.gated_client_present && !state->md_info.enable_md_int))
        {
          lsm6dsm_send_fifo_flush_done(this, &state->accel_info.suid);
        }
        if(state->fifo_info.publish_sensors & LSM6DSM_GYRO)
        {
          lsm6dsm_send_fifo_flush_done(this, &state->gyro_info.suid);
        }
        state->fifo_info.bh_info.flush_client_present = false;
        state->fifo_info.th_info.flush_client_present = false;
      }
      else if(state->irq_info.irq_config.interrupt_trigger_type == SNS_INTERRUPT_TRIGGER_TYPE_HIGH
          || state->irq_info.irq_config.interrupt_trigger_type == SNS_INTERRUPT_TRIGGER_TYPE_LOW)
      {
        // Send is_cleared message only for level triggered interrupt
        lsm6dsm_send_interrupt_is_cleared_msg(this);
      }
      port_rw_events_rcvd++;
    }
    event = state->async_com_port_data_stream->api->get_next_input(state->async_com_port_data_stream);
  }

    if(port_rw_events_rcvd > 0)
    {
      if(port_rw_events_rcvd > 1) {
        DBG_INST_PRINT_EX(this, MED, __FILENAME__, __LINE__, "#rw_evs=%u", port_rw_events_rcvd);
      }
      //if heart beat had happened, just reset the flag
      //if events are coming through, no need to worry about heart_attack
      state->health.heart_attack = false;
      //TODO::how about the counter
      if(state->health.heart_attack_cnt < MAX_HEART_ATTACKS/2) {
        state->health.heart_attack_cnt = 0;
        // how does framework reacts receiving data after heartattack
      }

      //check if fifo int is still high, if high read fifo data
      //item:1 May be not req for ACCTIVE HIGH/LOW  Interrupts
      //recheck int status
      sns_time cur_time = sns_get_system_time();
      uint16_t min_samples_to_check = state->fifo_info.cur_wmk;
      if(state->fifo_info.reconfig_req || state->fifo_info.th_info.flush_req)
        min_samples_to_check = 1;

      DBG_INST_PRINT(this, MED, __FILENAME__, __LINE__,
          "recheck fifo_int cur_t=%u last=%u elapsed=%d",
          (uint32_t)cur_time,(uint32_t)(state->fifo_info.last_timestamp), (int32_t)(cur_time - state->fifo_info.last_timestamp));

      state->fifo_info.recheck_int = true;
      if(state->fifo_info.th_info.flush_req || state->fifo_info.th_info.interrupt_fired)
        state->fifo_info.recheck_int = false;
#if 0
      int64_t elapsed_time = cur_time - state->fifo_info.last_timestamp;
      if(state->fifo_info.th_info.flush_req || state->fifo_info.th_info.interrupt_fired ||
          ((elapsed_time > 0) && (elapsed_time > 0.7 * state->fifo_info.bh_info.sampling_intvl * min_samples_to_check))) {
        lsm6dsm_read_fifo_data(this, cur_time, false);
      } else {
        state->fifo_info.recheck_int = false;
        if(state->fifo_info.th_info.flush_client_present &&
            !state->fifo_info.bh_info.flush_client_present) {
          if(state->fifo_info.publish_sensors & LSM6DSM_ACCEL)
          {
            lsm6dsm_send_fifo_flush_done(this, &state->accel_info.suid);
          }
          if(state->fifo_info.publish_sensors & LSM6DSM_GYRO)
          {
            lsm6dsm_send_fifo_flush_done(this, &state->gyro_info.suid);
          }
          state->fifo_info.th_info.flush_client_present = false;
        }
        state->fifo_info.th_info.flush_req = false;
        state->fifo_info.th_info.interrupt_fired = false;
      }
#else
      if(state->fifo_info.reconfig_req)
        state->fifo_info.th_info.flush_req = true;
      lsm6dsm_read_fifo_data(this, cur_time, false);
      //still req reconfig, do it here, hard reconfig

      if((state->fifo_info.reconfig_req) && (!state->ascp_req_count))
        lsm6dsm_reconfig_fifo(this, false);

#endif

    }
}

static sns_rc lsm6dsm_handle_heart_attack_timer_events(sns_sensor_instance *const this, sns_timer_sensor_event *timer_event)

{
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)this->state->state;
  sns_rc rv = SNS_RC_SUCCESS;
  if((state->fifo_info.publish_sensors & (LSM6DSM_ACCEL | LSM6DSM_GYRO)) ||
       (state->accel_info.gated_client_present && !state->md_info.enable_md_int))
  {
    DBG_INST_PRINT(this, ERROR, __FILENAME__, __LINE__,
        "Heart Attack: count = %d!",
        state->health.heart_attack_cnt);
  }
  else
  {
    sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_heart_beat_data_stream);
    state->health.heart_attack = false;
    return SNS_RC_SUCCESS;
  }
  //check this event is closer to last timestamp
  //case: interrupt and timer would have fired at the same time
  //no need to handle this, check current time with previous interrupt time
  // QC: It's probably sufficient to look at the timer event timestamp.
  sns_time now = sns_get_system_time();
  int64_t diff = timer_event->timeout_time - state->fifo_info.th_info.interrupt_ts;
  DBG_INST_PRINT(this, LOW, __FILENAME__, __LINE__,
      "Heart Attack:timeout=%u irq_ts=%u diff=%d",
      (uint32_t)timer_event->timeout_time,
      (uint32_t)state->fifo_info.th_info.interrupt_ts,
      (int32_t)diff);
  if((diff > 0) && (diff < state->heart_beat_timeout)) {
    //checkng most recent event, if the diff is less ignore
    return SNS_RC_SUCCESS;
  }

  state->health.heart_attack = true;
  state->health.heart_attack_cnt++;
  rv = SNS_RC_SUCCESS;
  if(state->health.heart_attack_cnt >= MAX_HEART_ATTACKS) {
    //report not available
    DBG_INST_PRINT(this, ERROR, __FILENAME__, __LINE__,
        "Heart Beat: Max reset tried. Notify Event to return: SNS_RC_INVALID_STATE!");
    sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_heart_beat_data_stream);
    rv = SNS_RC_INVALID_STATE;
    state->health.heart_attack_cnt = 0;

  } else if(state->health.heart_attack_cnt >= MAX_HEART_ATTACKS/2) {
    // reset sensor Something is wrong. Reset device.
    DBG_INST_PRINT(this, ERROR, __FILENAME__, __LINE__,
        "Heart Beat: Reseting Device!");
    // Reset Sensor
    inst_exit_island(this);
    rv = lsm6dsm_recover_device(this);
    //restart timer here even if this is periodic
    lsm6dsm_inst_create_timer(this, &state->timer_heart_beat_data_stream, state->heart_beat_timeout, true);
    // increment till it reaches MAX_HEART_ATTACKS
    state->health.heart_attack_cnt++;
    state->health.heart_attack = false;
    rv = SNS_RC_NOT_AVAILABLE;
  }
  if(rv == SNS_RC_SUCCESS) {
    DBG_INST_PRINT_EX(this, LOW, __FILENAME__, __LINE__,
        "Heart Attack: flushing fifo");
    //heart attack timer fires, flush fifo and check last_ts
    lsm6dsm_flush_fifo(this);
    int64_t ts_diff = now - state->fifo_info.last_timestamp;
    if((!state->ascp_req_count) && (ts_diff < state->heart_beat_timeout)) {
      state->health.heart_attack_cnt = 0;
      state->health.heart_attack = false;
    }
  }
  return rv;
}

sns_rc lsm6dsm_handle_timer_events(sns_sensor_instance *const this)
{
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)this->state->state;
  sns_sensor_event *event;
  sns_rc rv = SNS_RC_SUCCESS;


  if(NULL != state->timer_sensor_temp_data_stream)
  {
    event = state->timer_sensor_temp_data_stream->api->peek_input(state->timer_sensor_temp_data_stream);
    while(NULL != event)
    {
      if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
      {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
            event->event_len);
        sns_timer_sensor_event timer_event;
        if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
        {
          if(state->fifo_info.publish_sensors & LSM6DSM_SENSOR_TEMP
              &&
              state->sensor_temp_info.timer_is_active
              &&
              state->sensor_temp_info.sampling_intvl > 0)
          {
            lsm6dsm_handle_sensor_temp_sample(this);
          }
          else
          {
            sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_sensor_temp_data_stream);
            state->sensor_temp_info.timer_is_active = false;
            return rv;
          }
        }
        else
        {
          DBG_INST_PRINT_EX(this, ERROR, __FILENAME__, __LINE__,
              "Failed to decode sensor temp timer event %u", event->message_id);
        }
      }
      event = state->timer_sensor_temp_data_stream->api->get_next_input(state->timer_sensor_temp_data_stream);
    }
  }

  if(NULL != state->timer_self_test_data_stream)
  {
    event = state->timer_self_test_data_stream->api->peek_input(state->timer_self_test_data_stream);
    while(NULL != event)
    {
      if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
      {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
            event->event_len);
        sns_timer_sensor_event timer_event;
        if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
        {
          if(state->self_test_info.test_alive)
          {
            inst_exit_island(this);
            if(state->self_test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_HW)
              lsm6dsm_inst_hw_self_test(this);
            else if(state->self_test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
              lsm6dsm_inst_factory_self_test(this);
          }
        }
        else
        {
          DBG_INST_PRINT_EX(this, ERROR, __FILENAME__, __LINE__,
              "Failed to decode sensor temp timer event %u", event->message_id);
        }
      }
      if(NULL != state->timer_self_test_data_stream)
      {
        event = state->timer_self_test_data_stream->api->get_next_input(state->timer_self_test_data_stream);
      }
      else
      {
        event = NULL;
        break;
      }
    }
  }

  if(NULL != state->timer_md_data_stream)
  {
    event = state->timer_md_data_stream->api->peek_input(state->timer_md_data_stream);
    while(NULL != event)
    {
      if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
      {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
            event->event_len);
        sns_timer_sensor_event timer_event;

        if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
        {
          lsm6dsm_update_md_filter(this);
        }
        else
        {
          DBG_INST_PRINT(this, ERROR, __FILENAME__, __LINE__,
              "decode fail msg=%u", event->message_id);
        }
      }
      event = state->timer_md_data_stream->api->get_next_input(state->timer_md_data_stream);
    }
  }
  // QC: Put in another function?
  if(NULL != state->timer_heart_beat_data_stream)
  {
    sns_timer_sensor_event latest_timer_event;
    event = state->timer_heart_beat_data_stream->api->peek_input(state->timer_heart_beat_data_stream);
    uint8_t event_cnt = 0;

    while(NULL != event)
    {
      if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
      {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
            event->event_len);
        sns_timer_sensor_event timer_event;
        if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
        {
          latest_timer_event = timer_event;
          if(!state->self_test_info.test_alive) //Skip heart attack for  self-test case
            event_cnt++;
        }
        else
        {
          DBG_INST_PRINT_EX(this, ERROR, __FILENAME__, __LINE__,
              "Failed to decode heart beat timer event %u", event->message_id);
        }
      }
      event = state->timer_heart_beat_data_stream->api->get_next_input(state->timer_heart_beat_data_stream);
    }
    if(event_cnt) {
      DBG_INST_PRINT(this, MED, __FILENAME__, __LINE__,
          "Heart attack timer event count %d", event_cnt);
      rv = lsm6dsm_handle_heart_attack_timer_events(this, &latest_timer_event);
    }
  }
  return rv;
}

/** See sns_sensor_instance_api::notify_event */
static sns_rc lsm6dsm_inst_notify_event(sns_sensor_instance *const this)
{
  sns_rc rv = SNS_RC_SUCCESS;
  sns_rc com_rv = SNS_RC_SUCCESS;
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)this->state->state;

  // Turn COM port ON
  com_rv = state->scp_service->api->sns_scp_update_bus_power(
     state->com_port_info.port_handle, true);
  if (SNS_RC_SUCCESS != com_rv)
  {
    SNS_INST_PRINTF(ERROR, this, "notify_event:: update_bus_power ON %d", com_rv);
  }

  lsm6dsm_dae_if_process_events(this);

  lsm6dsm_handle_ascp_events(this);
  lsm6dsm_handle_hw_interrupts(this);
  rv = lsm6dsm_handle_timer_events(this);

  // Turn COM port OFF
  com_rv = state->scp_service->api->sns_scp_update_bus_power(
     state->com_port_info.port_handle, false);
  if (SNS_RC_SUCCESS != com_rv)
  {
    SNS_INST_PRINTF(ERROR, this, "notify_event:: update_bus_power OFF %d", com_rv);
  }

  return rv;
}

/** See sns_sensor_instance_api::set_client_config */
static sns_rc lsm6dsm_inst_set_client_config(sns_sensor_instance *const this,
                                             sns_request const *client_request)
{
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)this->state->state;
  float desired_sample_rate = 0;
  float desired_report_rate = 0;
  float accel_chosen_sample_rate = 0;
  float gyro_chosen_sample_rate = 0;
  lsm6dsm_accel_odr accel_chosen_sample_rate_reg_value = LSM6DSM_ACCEL_ODR_OFF;
  lsm6dsm_gyro_odr gyro_chosen_sample_rate_reg_value = LSM6DSM_GYRO_ODR_OFF ;
  uint16_t desired_wmk = 0;
  sns_rc rv = SNS_RC_SUCCESS;
  uint16_t accel_num_samples_to_discard,gyro_num_samples_to_discard;
  float *fac_cal_bias = NULL;
  matrix3 *fac_cal_corr_mat = NULL;
  uint32_t *registry_persist_version = NULL;
  sns_service_manager *mgr = this->cb->get_service_manager(this);
  sns_event_service *e_service = (sns_event_service*)mgr->get_service(mgr, SNS_EVENT_SERVICE);

  sns_lsm6dsm_req *payload = (sns_lsm6dsm_req*)client_request->request;
  accel_num_samples_to_discard = gyro_num_samples_to_discard = 0;
  DBG_INST_PRINT_EX(this, MED, __FILENAME__, __LINE__,
                 "set_client_config: msg=%u", client_request->message_id);

#if LSM6DSM_AUTO_DEBUG
  SNS_INST_PRINTF(HIGH, this, "set_client_config: msg=%u", client_request->message_id);
#endif
  // Turn COM port ON
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
      true);
  // Register for interrupt
  if(!lsm6dsm_dae_if_available(this))
  {
    lsm6dsm_register_interrupt(this);
  }

#if 0
  // Reset the device if not streaming.
  if(state->fifo_info.fifo_rate == LSM6DSM_ACCEL_ODR_OFF
      &&
      state->common_info.accel_curr_odr == LSM6DSM_ACCEL_ODR_OFF)
  {
    lsm6dsm_reset_device(this,
        LSM6DSM_ACCEL | LSM6DSM_GYRO | LSM6DSM_MOTION_ACCEL | LSM6DSM_SENSOR_TEMP);
  }
#endif
  if(client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
  {
    DBG_INST_PRINT_EX(this, MED, __FILENAME__, __LINE__,
                 "std sensor config");

    // 1. Extract sample, report rates from client_request.
    // 2. Configure sensor HW.
    // 3. sendRequest() for Timer to start/stop in case of polling using timer_data_stream.
    // 4. sendRequest() for Intrerupt register/de-register in case of DRI using interrupt_data_stream.
    // 5. Save the current config information like type, sample_rate, report_rate, etc.
    desired_sample_rate = payload->desired_sample_rate;
    desired_report_rate = payload->desired_report_rate;
    uint8_t desired_rate_idx = 0;

    DBG_INST_PRINT(this, MED, __FILENAME__, __LINE__,
                   "sr*1000=%d rr*1000=%d",
                   (int)(desired_sample_rate*1000),(int)(desired_report_rate*1000));
#if LSM6DSM_AUTO_DEBUG
    SNS_INST_PRINTF(HIGH, this,  "sr*1000=%d rr*1000=%d",(int)(desired_sample_rate*1000),(int)(desired_report_rate*1000));
#endif
    if(state->fifo_info.fifo_enabled & LSM6DSM_ACCEL)
    {
      rv = lsm6dsm_match_odr(LSM6DSM_ACCEL, desired_sample_rate,
          &desired_rate_idx,
          &accel_chosen_sample_rate,
          &accel_chosen_sample_rate_reg_value,
          &accel_num_samples_to_discard);
      if(rv != SNS_RC_SUCCESS)
      {
        SNS_INST_PRINTF(ERROR, this, "A-ODR match err %d", rv);
        lsm6dsm_instance_state *state = (lsm6dsm_instance_state *)this->state->state;

        //Allocate and send immediately.
        sns_sensor_event *event = e_service->api->alloc_event(e_service, this, 0);
        if(NULL != event)
        {
          event->message_id = SNS_STD_MSGID_SNS_STD_ERROR_EVENT;
          event->event_len = 0;
          event->timestamp = sns_get_system_time();
          e_service->api->publish_event(e_service, this, event, &state->accel_info.suid);
        }
        //return rv;
      }
    }

    if(state->fifo_info.fifo_enabled & LSM6DSM_GYRO)
    {
      rv = lsm6dsm_match_odr(LSM6DSM_GYRO, desired_sample_rate,
          &desired_rate_idx,
          &gyro_chosen_sample_rate,
          &gyro_chosen_sample_rate_reg_value,
          &gyro_num_samples_to_discard);
      DBG_INST_PRINT(this, HIGH, __FILENAME__, __LINE__,
                     "G-ODR match. #discard=%d", gyro_num_samples_to_discard);
      if(rv != SNS_RC_SUCCESS)
      {
        SNS_INST_PRINTF(ERROR, this, "G-ODR match err %d", rv);
        lsm6dsm_instance_state *state = (lsm6dsm_instance_state *)this->state->state;

        //Allocate and send immediately.
        sns_sensor_event *event = e_service->api->alloc_event(e_service, this, 0);
        if(NULL != event)
        {
          event->message_id = SNS_STD_MSGID_SNS_STD_ERROR_EVENT;
          event->event_len = 0;
          event->timestamp = sns_get_system_time();
          e_service->api->publish_event(e_service, this, event, &state->gyro_info.suid);
        }
        //return rv;
      }
    }

    if(state->fifo_info.publish_sensors & LSM6DSM_SENSOR_TEMP)
    {
      rv = lsm6dsm_validate_sensor_temp_odr(state);
      if(rv != SNS_RC_SUCCESS)
      {
        SNS_INST_PRINTF(ERROR, this, "T-ODR match err %d", rv);
        lsm6dsm_instance_state *state = (lsm6dsm_instance_state *)this->state->state;
        sns_sensor_event *event = e_service->api->alloc_event(e_service, this, 0);

        if(NULL != event)
        {
          event->message_id = SNS_STD_MSGID_SNS_STD_ERROR_EVENT;
          event->event_len = 0;
          event->timestamp = sns_get_system_time();
          e_service->api->publish_event(e_service, this, event, &state->sensor_temp_info.suid);
        }
        //return rv;
      }
    }
    state->common_info.desired_odr_idx = desired_rate_idx;
    DBG_INST_PRINT(this, MED, __FILENAME__, __LINE__,
                   "A-ODR(cur/new)=0x%x/0x%x",
                   state->common_info.accel_curr_odr, accel_chosen_sample_rate_reg_value);
    if(desired_report_rate > accel_chosen_sample_rate)
    {
      // bad request. Return error or default report_rate to sample_rate
      desired_report_rate = accel_chosen_sample_rate;
    }
    if(desired_report_rate != 0)
    {
    // Round up watermark. Qcom tracker: 39. CR2060232
    // There seems to be an issue in Qcom compiler where float division has issues
    // This is a workaround for that.
      desired_wmk = (uint16_t)((accel_chosen_sample_rate + 0.5*desired_report_rate) / desired_report_rate);
    }
    else
    {
      if(accel_chosen_sample_rate == LSM6DSM_ACCEL_ODR_OFF)
      {
        desired_wmk = 0;
      }
      else
      {
        desired_wmk = 1;
      }
    }

    DBG_INST_PRINT(this, MED, __FILENAME__, __LINE__,
                   "wm(des/cur)=%d/%d",
                   desired_wmk,state->fifo_info.cur_wmk);
    if(LSM6DSM_MAX_FIFO <= desired_wmk)
    {
      desired_wmk = LSM6DSM_MAX_FIFO;
    }

    state->desired_conf.wmk = desired_wmk;
    state->desired_conf.odr = accel_chosen_sample_rate;
    //consider first two bits, doesn't matter temp
    state->desired_conf.enabled_sensors = state->fifo_info.fifo_enabled & 0x03;
    state->desired_conf.md_enabled = state->md_info.enable_md_int;
    state->fifo_info.reconfig_req = false;

    DBG_INST_PRINT(this, MED, __FILENAME__, __LINE__,
                   "des_conf(wm,r,s,md) %d,%d,%d,%d ",
                   state->desired_conf.wmk, state->desired_conf.odr,state->desired_conf.enabled_sensors, state->desired_conf.md_enabled);
    DBG_INST_PRINT(this, MED, __FILENAME__, __LINE__,
                   "cur_conf(wm,r,s,md) %d,%d,%d,%d ",
                   state->current_conf.wmk, state->current_conf.odr,state->current_conf.enabled_sensors, state->current_conf.md_enabled);

    if((state->desired_conf.wmk != state->current_conf.wmk) ||
        (state->desired_conf.odr != state->current_conf.odr) ||
        (state->desired_conf.enabled_sensors != state->current_conf.enabled_sensors))
      state->fifo_info.reconfig_req = true;
    if(!state->desired_conf.enabled_sensors && !state->current_conf.enabled_sensors)
      state->fifo_info.reconfig_req = false;

    DBG_INST_PRINT(this, MED, __FILENAME__, __LINE__,
                   "reconfig_req=%d HB_timeout=%u",
                   state->fifo_info.reconfig_req, (uint32_t)state->heart_beat_timeout);
    if(!state->fifo_info.reconfig_req) {
      //not reconfiguring, just reset config_sensors
      state->config_sensors = 0;
    }
    if(state->md_info.enable_md_int)
    {
      DBG_INST_PRINT(this, HIGH, __FILENAME__, __LINE__,
                     "gated accel config");

      lsm6dsm_set_gated_accel_config(state,
          desired_wmk,
          accel_chosen_sample_rate_reg_value,
          state->fifo_info.fifo_enabled);
    }

    lsm6dsm_set_fifo_config(this,
        desired_wmk,
        accel_chosen_sample_rate_reg_value,
        gyro_chosen_sample_rate_reg_value,
        state->fifo_info.fifo_enabled);

    state->fifo_info.max_requested_flush_ticks = payload->desired_flush_ticks;

    if(!state->self_test_info.test_alive)
    {
      lsm6dsm_send_config_event(this);
      //reconfig at interrupt boundary if wmk=1
      if((state->current_conf.wmk != 1) ||
        !state->fifo_info.reconfig_req ||
        (state->current_conf.md_enabled != state->desired_conf.md_enabled))
      {
        lsm6dsm_reconfig_hw(this);
        lsm6dsm_dump_reg(this, state->fifo_info.fifo_enabled);
      }
      else if((state->sensor_temp_info.desired_sampling_rate_hz !=  state->sensor_temp_info.cur_sampling_rate_hz)
              && ((state->fifo_info.publish_sensors & LSM6DSM_SENSOR_TEMP) ||
                   (state->sensor_temp_info.timer_is_active)))
      {
        lsm6dsm_set_polling_config(this);
      }
    }
    else
    {
      //Postpone the reconfig till Self-test is over
      state->fifo_info.reconfig_req = false;
      state->self_test_info.reconfig_postpone = true;
    }

    // update registry configuration
    if(LSM6DSM_ACCEL == payload->registry_cfg.sensor_type)
    {
      fac_cal_bias = state->accel_registry_cfg.fac_cal_bias;
      fac_cal_corr_mat = &state->accel_registry_cfg.fac_cal_corr_mat;
      registry_persist_version = &state->accel_registry_cfg.registry_instance_version;
    }
    else if(LSM6DSM_GYRO == payload->registry_cfg.sensor_type)
    {
      fac_cal_bias = state->gyro_registry_cfg.fac_cal_bias;
      fac_cal_corr_mat = &state->gyro_registry_cfg.fac_cal_corr_mat;
      registry_persist_version = &state->gyro_registry_cfg.registry_instance_version;
    }
    else if(LSM6DSM_SENSOR_TEMP == payload->registry_cfg.sensor_type)
    {
      fac_cal_bias = state->sensor_temp_registry_cfg.fac_cal_bias;
      fac_cal_corr_mat = &state->sensor_temp_registry_cfg.fac_cal_corr_mat;
      registry_persist_version = &state->sensor_temp_registry_cfg.registry_instance_version;
    }

    if((NULL!= fac_cal_bias && NULL != fac_cal_corr_mat)
       && !(*registry_persist_version > payload->registry_cfg.registry_persist_version))
    {
      sns_memscpy(fac_cal_bias, sizeof(payload->registry_cfg.fac_cal_bias),
                  payload->registry_cfg.fac_cal_bias,
                  sizeof(payload->registry_cfg.fac_cal_bias));

      sns_memscpy(fac_cal_corr_mat, sizeof(payload->registry_cfg.fac_cal_corr_mat),
                  &payload->registry_cfg.fac_cal_corr_mat,
                  sizeof(payload->registry_cfg.fac_cal_corr_mat));

      sns_memscpy(registry_persist_version, sizeof(payload->registry_cfg.registry_persist_version),
                  &payload->registry_cfg.registry_persist_version,
                  sizeof(payload->registry_cfg.registry_persist_version));
    }
  }
  else if(client_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ)
  {

    DBG_INST_PRINT(this, HIGH, __FILENAME__, __LINE__,
                   "std_flush_req fifo_enabled=0x%x c_wmk=%d", state->fifo_info.fifo_enabled, state->fifo_info.cur_wmk);
    if(state->fifo_info.fifo_enabled) {
      state->fifo_info.th_info.flush_client_present = true;
      lsm6dsm_flush_fifo(this);
    }
  }
  else if(client_request->message_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
  {
    /** All self-tests are handled in normal mode. */
    inst_exit_island(this);
    lsm6dsm_set_client_test_config(this);
  }

  // Turn COM port OFF
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
      false);
  return SNS_RC_SUCCESS;
}

/** Public Data Definitions. */

sns_sensor_instance_api lsm6dsm_sensor_instance_api =
{
  .struct_len        = sizeof(sns_sensor_instance_api),
  .init              = &lsm6dsm_inst_init,
  .deinit            = &lsm6dsm_inst_deinit,
  .set_client_config = &lsm6dsm_inst_set_client_config,
  .notify_event      = &lsm6dsm_inst_notify_event
};

