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
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_types.h"
#include "sns_sensor_util.h"

#include "sns_lsm6dsm_hal.h"
#include "sns_lsm6dsm_sensor.h"
#include "sns_lsm6dsm_sensor_instance.h"

#include "sns_interrupt.pb.h"
#include "sns_async_com_port.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_cal.pb.h"
#include "sns_pb_util.h"
#include "sns_diag_service.h"
#include "sns_diag.pb.h"
#include "sns_sync_com_port_service.h"
#include "sns_printf.h"

/**
 * Accelerometer LSM6DSM hardware self test settings
 */
#define LSM6DSM_ST_2G_MIN       90      //Unit: mg
#define LSM6DSM_ST_2G_MAX       1700    //Unit: mg

/**
 * Gyroscope LSM6DSM self test settings
 */
#define LSM6DSM_2000DPS_ST_MIN      150  //unit dps.
#define LSM6DSM_2000DPS_ST_MAX      700

extern const odr_reg_map lsm6dsm_odr_map[LSM6DSM_REG_MAP_TABLE_SIZE];

static void enable_hw_self_test(
  sns_sensor_instance *const this,
  lsm6dsm_sensor_type sensor,
  bool enable)
{
  uint8_t buffer;
  uint32_t xfer_bytes;

  if(enable == false)
  {
    buffer = 0x00;
  }
  else if(sensor == LSM6DSM_ACCEL)
  {
    buffer = 0x01;
  }
  else //enable bit for gyro
  {
    buffer = 0x04;
  }
  lsm6dsm_read_modify_write(this,
                            STM_LSM6DSM_REG_CTRL5,
                            &buffer,
                            1,
                            &xfer_bytes,
                            false,
                            0x00);

}

static void lsm6dsm_inst_collect_data(sns_sensor_instance *const this, float * data)
{
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)this->state->state;
  lsm6dsm_sensor_type sensor[1];
  float raw_data[3] = {0,0,0};
  float acc_conv = 1000/G; //m/s2 to mg
  float gyro_conv = 1; //dps
  float inst_conv;
  int8_t idx;
  lsm6dsm_accel_odr curr_odr = LSM6DSM_ODR_52;
  uint16_t self_test_odr = LSM6DSM_ODR_52;

  //handle case were new streaming ODR is less than self test ODR
  if((state->self_test_info.curr_odr >  state->common_info.accel_curr_odr) && (state->common_info.accel_curr_odr != LSM6DSM_ACCEL_ODR_OFF))
  {
    state->self_test_info.curr_odr =  state->common_info.accel_curr_odr;
  }

  for(idx = 0; idx < ARR_SIZE(lsm6dsm_odr_map); idx++)
  {
    if(curr_odr == lsm6dsm_odr_map[idx].accel_odr_reg_value
       &&
       curr_odr != LSM6DSM_ACCEL_ODR_OFF)
    {
      self_test_odr = lsm6dsm_odr_map[idx].odr;
      break;
    }
  }
  sensor[0] = state->self_test_info.sensor;

  //Check if any samples need to be dropped
  if(state->self_test_info.skip_count)
  {
    lsm6dsm_get_data(this,sensor,1,raw_data);
    DBG_INST_PRINT_EX(this, LOW, __FILENAME__, __LINE__, "Discarding this sample read");
    sns_time timeout = sns_convert_ns_to_ticks(((1000 * 1000)/self_test_odr) * 1000 * state->self_test_info.skip_count);
    lsm6dsm_inst_create_timer(this, &state->timer_self_test_data_stream, timeout, false); // Skip this sample as it may be invalid
    state->self_test_info.skip_count=0;
    return;
  }

  if(state->self_test_info.sensor == LSM6DSM_ACCEL)
    inst_conv = acc_conv;
  else
    inst_conv = gyro_conv;

  //Poll data for 25 samples, save total
  lsm6dsm_get_data(this,sensor,1,raw_data);
  state->self_test_info.polling_count++;
  data[0] += raw_data[0]*inst_conv;
  data[1] += raw_data[1]*inst_conv;
  data[2] += raw_data[2]*inst_conv;


  if(state->self_test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
  {
  //Collect sample square sum for Variance evaluation
    state->self_test_info.cumulative_data_post[0] += (raw_data[0]*inst_conv)*(raw_data[0]*inst_conv);
    state->self_test_info.cumulative_data_post[1] += (raw_data[1]*inst_conv)*(raw_data[1]*inst_conv);
    state->self_test_info.cumulative_data_post[2] += (raw_data[2]*inst_conv)*(raw_data[2]*inst_conv);
  }

  if(state->self_test_info.polling_count == SELF_TEST_DATA_COUNT_MAX)
  {
    data[0] /= SELF_TEST_DATA_COUNT_MAX;
    data[1] /= SELF_TEST_DATA_COUNT_MAX;
    data[2] /= SELF_TEST_DATA_COUNT_MAX;
    state->self_test_info.polling_count = 0;
    state->self_test_info.self_test_stage++;
  }
  else
  {
    sns_time timeout = sns_convert_ns_to_ticks(((1000 * 1000)/self_test_odr) * 1000);
    lsm6dsm_inst_create_timer(this, &state->timer_self_test_data_stream, timeout, false); // wait for next odr to read the next sample
  }
}


static void send_hw_self_test_result(sns_sensor_instance *const instance)
{

  bool test_passed = false;
  lsm6dsm_instance_state *state =
     (lsm6dsm_instance_state*)instance->state->state;
  sns_sensor_uid *suid_current;

  //Compare the two averages - copy from ddf
  //Check self test limits
  if(state->self_test_info.sensor == LSM6DSM_ACCEL)
  {
    suid_current = &state->accel_info.suid;
    if((LSM6DSM_ST_2G_MIN <= fabsf(state->self_test_info.cumulative_data_post[0]-state->self_test_info.cumulative_data_pre[0]))
        &&(fabsf(state->self_test_info.cumulative_data_post[0]-state->self_test_info.cumulative_data_pre[0]) <= LSM6DSM_ST_2G_MAX)
        &&(LSM6DSM_ST_2G_MIN <= fabsf(state->self_test_info.cumulative_data_post[1]-state->self_test_info.cumulative_data_pre[1]))
        &&(fabsf(state->self_test_info.cumulative_data_post[1]-state->self_test_info.cumulative_data_pre[1])<= LSM6DSM_ST_2G_MAX)
        &&(LSM6DSM_ST_2G_MIN <= fabsf(state->self_test_info.cumulative_data_post[2]-state->self_test_info.cumulative_data_pre[2]))
        &&(fabsf(state->self_test_info.cumulative_data_post[2]-state->self_test_info.cumulative_data_pre[2])<= LSM6DSM_ST_2G_MAX))
      test_passed = true;
    else
      test_passed = false;
  }
  else //gyro
  {
    suid_current = &state->gyro_info.suid;
    if((LSM6DSM_2000DPS_ST_MIN <= fabsf(state->self_test_info.cumulative_data_post[0]-state->self_test_info.cumulative_data_pre[0]))
        &&(fabsf(state->self_test_info.cumulative_data_post[0]-state->self_test_info.cumulative_data_pre[0]) <= LSM6DSM_2000DPS_ST_MAX)
        &&(LSM6DSM_2000DPS_ST_MIN <= fabsf(state->self_test_info.cumulative_data_post[1]-state->self_test_info.cumulative_data_pre[1]))
        &&(fabsf(state->self_test_info.cumulative_data_post[1]-state->self_test_info.cumulative_data_pre[1])<= LSM6DSM_2000DPS_ST_MAX)
        &&(LSM6DSM_2000DPS_ST_MIN <= fabsf(state->self_test_info.cumulative_data_post[2]-state->self_test_info.cumulative_data_pre[2]))
        &&(fabsf(state->self_test_info.cumulative_data_post[2]-state->self_test_info.cumulative_data_pre[2])<= LSM6DSM_2000DPS_ST_MAX))
      test_passed = true;
    else
      test_passed = false;
  }
  DBG_INST_PRINT(instance, MED, __FILENAME__, __LINE__,
                 "Self Test Result=%d", test_passed);


  sns_physical_sensor_test_event physical_sensor_test_event;
  uint8_t data[1] = {0};
  pb_buffer_arg buff_arg = (pb_buffer_arg)
      { .buf = &data, .buf_len = sizeof(data) };

  DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__,
                 "Sending Self Test event");

  physical_sensor_test_event.test_passed = test_passed;
  physical_sensor_test_event.test_type = SNS_PHYSICAL_SENSOR_TEST_TYPE_HW;
  physical_sensor_test_event.test_data.funcs.encode = &pb_encode_string_cb;
  physical_sensor_test_event.test_data.arg = &buff_arg;

  pb_send_event(instance,
                sns_physical_sensor_test_event_fields,
                &physical_sensor_test_event,
                sns_get_system_time(),
                SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
                suid_current);

  state->self_test_info.test_alive = false;
}

static void send_factory_self_test_result(sns_sensor_instance *const instance, bool test_passed)
{

  lsm6dsm_instance_state *state =
     (lsm6dsm_instance_state*)instance->state->state;

  DBG_INST_PRINT(instance, MED, __FILENAME__, __LINE__,
        "Self Test(factory) Result=%d", test_passed);

  sns_physical_sensor_test_event physical_sensor_test_event;
  uint8_t data[1] = {0};
  pb_buffer_arg buff_arg = (pb_buffer_arg)
      { .buf = &data, .buf_len = sizeof(data) };
  sns_sensor_uid *suid_current;

  //update suid
  if(state->self_test_info.sensor == LSM6DSM_ACCEL)
  {
    suid_current = &state->accel_info.suid;
  }
  else
  {
    suid_current = &state->gyro_info.suid;
  }

  lsm6dsm_send_cal_event(instance, state->self_test_info.sensor);

  DBG_INST_PRINT_EX(instance, HIGH, __FILENAME__, __LINE__,
        "Sending Self Test event");

  physical_sensor_test_event.test_passed = test_passed;
  physical_sensor_test_event.test_type = SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY;
  physical_sensor_test_event.test_data.funcs.encode = &pb_encode_string_cb;
  physical_sensor_test_event.test_data.arg = &buff_arg;

  pb_send_event(instance,
                sns_physical_sensor_test_event_fields,
                &physical_sensor_test_event,
                sns_get_system_time(),
                SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
                suid_current);

  state->self_test_info.test_alive = false;
}

static void send_com_self_test_result(sns_sensor_instance *const instance, bool test_passed)
{

  lsm6dsm_instance_state *state =
     (lsm6dsm_instance_state*)instance->state->state;

  DBG_INST_PRINT(instance, MED, __FILENAME__, __LINE__,
        "Self Test(com) Result=%d", test_passed);

  sns_physical_sensor_test_event physical_sensor_test_event;
  uint8_t data[1] = {0};
  pb_buffer_arg buff_arg = (pb_buffer_arg)
      { .buf = &data, .buf_len = sizeof(data) };
  sns_sensor_uid *suid_current;

  //update suid
  if(state->self_test_info.sensor == LSM6DSM_GYRO)
  {
    suid_current = &state->gyro_info.suid;
  }
  else if(state->self_test_info.sensor == LSM6DSM_MOTION_DETECT)
  {
    suid_current = &state->md_info.suid;
  }
  else if(state->self_test_info.sensor == LSM6DSM_SENSOR_TEMP)
  {
    suid_current = &state->sensor_temp_info.suid;
  }
  else
  {
    suid_current = &state->accel_info.suid;
  }

  DBG_INST_PRINT_EX(instance, HIGH, __FILENAME__, __LINE__,
        "Sending Self Test event");

  physical_sensor_test_event.test_passed = test_passed;
  physical_sensor_test_event.test_type = SNS_PHYSICAL_SENSOR_TEST_TYPE_COM;
  physical_sensor_test_event.test_data.funcs.encode = &pb_encode_string_cb;
  physical_sensor_test_event.test_data.arg = &buff_arg;

  pb_send_event(instance,
                sns_physical_sensor_test_event_fields,
                &physical_sensor_test_event,
                sns_get_system_time(),
                SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
                suid_current);

  state->self_test_info.test_alive = false;
  state->health.heart_attack = false;
}

void lsm6dsm_inst_hw_self_test(sns_sensor_instance *const this)
{
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)this->state->state;
  uint32_t timeout = 0;
  lsm6dsm_accel_odr curr_odr = LSM6DSM_ACCEL_ODR52;

  switch (state->self_test_info.self_test_stage)
  {
    case LSM6DSM_SELF_TEST_STAGE_1:
    {
      //Check if sensor is already streaming. If not, turn it ON
      if((state->common_info.accel_curr_odr) && (state->common_info.accel_curr_odr < LSM6DSM_ACCEL_ODR833))
      {
        state->self_test_info.curr_odr =  state->common_info.accel_curr_odr;
        curr_odr = state->self_test_info.curr_odr;
      }
      else
      {
        state->self_test_info.curr_odr = LSM6DSM_ACCEL_ODR52;
      }

      if(state->self_test_info.sensor == LSM6DSM_ACCEL)
      {
        if((state->gyro_info.desired_odr) && !(state->accel_info.desired_odr))
        {
          lsm6dsm_set_fifo_wmk(this);
        }
        lsm6dsm_set_accel_config(this,curr_odr,LSM6DSM_ACCEL_SSTVT_2G,LSM6DSM_ACCEL_RANGE_2G,state->accel_info.bw);
        timeout = 100;
        for(state->self_test_info.odr_idx = 0; state->self_test_info.odr_idx < ARR_SIZE(lsm6dsm_odr_map); state->self_test_info.odr_idx++)
        {
          if(state->self_test_info.curr_odr == lsm6dsm_odr_map[state->self_test_info.odr_idx].accel_odr_reg_value)
          {
            state->self_test_info.skip_count = lsm6dsm_odr_map[state->self_test_info.odr_idx].accel_discard_samples;
            break;
          }
        }
      }
      else
      {
        if((state->accel_info.desired_odr) && !(state->gyro_info.desired_odr))
        {
          lsm6dsm_set_fifo_wmk(this);
        }
        lsm6dsm_set_gyro_config(this,(lsm6dsm_gyro_odr)curr_odr,LSM6DSM_GYRO_SSTVT_2000DPS,STM_LSM6DSM_GYRO_RANGE_2000DPS);
        timeout = 150;
        for(state->self_test_info.odr_idx = 0; state->self_test_info.odr_idx < ARR_SIZE(lsm6dsm_odr_map); state->self_test_info.odr_idx++)
        {
          if(state->self_test_info.curr_odr == lsm6dsm_odr_map[state->self_test_info.odr_idx].gyro_odr_reg_value)
          {
            state->self_test_info.skip_count = lsm6dsm_odr_map[state->self_test_info.odr_idx].gyro_discard_samples;
            break;
          }
        }
      }
      state->self_test_info.self_test_stage = LSM6DSM_SELF_TEST_STAGE_2;

      lsm6dsm_dump_reg(this, state->fifo_info.fifo_enabled);
      sns_time timeout_ticks = sns_convert_ns_to_ticks(timeout * 1000 * 1000);
      lsm6dsm_inst_create_timer(this, &state->timer_self_test_data_stream, timeout_ticks, false); //Settling time for ODR
    }
    break;

    case LSM6DSM_SELF_TEST_STAGE_2:
    {
      lsm6dsm_inst_collect_data(this, state->self_test_info.cumulative_data_pre);
    }
    if(state->self_test_info.self_test_stage == LSM6DSM_SELF_TEST_STAGE_2)
      break;

    case LSM6DSM_SELF_TEST_STAGE_3:
    {
      if(state->self_test_info.sensor == LSM6DSM_ACCEL)
      {
        timeout = 100;
        state->self_test_info.skip_count = lsm6dsm_odr_map[state->self_test_info.odr_idx].accel_discard_samples;
      }
      else //gyro
      {
        timeout = 50;
        state->self_test_info.skip_count = lsm6dsm_odr_map[state->self_test_info.odr_idx].gyro_discard_samples;
      }
      //Set bit for self test
      enable_hw_self_test(this, state->self_test_info.sensor, true);
      sns_time timeout_ticks = sns_convert_ns_to_ticks(timeout * 1000 * 1000);
      lsm6dsm_inst_create_timer(this, &state->timer_self_test_data_stream,
          timeout_ticks, false);
      state->self_test_info.self_test_stage = LSM6DSM_SELF_TEST_STAGE_4;
    }
    break;

    case LSM6DSM_SELF_TEST_STAGE_4:
    {
      lsm6dsm_inst_collect_data(this, state->self_test_info.cumulative_data_post);
    }
    if(state->self_test_info.self_test_stage == LSM6DSM_SELF_TEST_STAGE_4)
      break;
    if(state->self_test_info.self_test_stage == LSM6DSM_SELF_TEST_STAGE_5)
    {
      //Disable Self test bit
      enable_hw_self_test(this, state->self_test_info.sensor, false);
      if(state->self_test_info.sensor == LSM6DSM_ACCEL)
      {
        timeout = 100;
      }
      else
      {
        timeout = 50;
      }
      sns_time timeout_ticks = sns_convert_ns_to_ticks(timeout * 1000 * 1000);
      lsm6dsm_inst_create_timer(this, &state->timer_self_test_data_stream,
          timeout_ticks, false);
      break;
    }

    case LSM6DSM_SELF_TEST_STAGE_5:
    {
      send_hw_self_test_result(this);
      //Disable streaming if started for self test else set the original ODR is more than 833Hz
      if(state->self_test_info.sensor == LSM6DSM_ACCEL)
      {
        state->accel_info.self_test_is_successful = false;
        if(!(state->accel_info.desired_odr) || (state->accel_info.desired_odr > LSM6DSM_ACCEL_ODR833) || (state->accel_info.range != LSM6DSM_ACCEL_RANGE_2G))
        {
          lsm6dsm_set_accel_config(this,state->accel_info.desired_odr,state->accel_info.sstvt,state->accel_info.range,state->accel_info.bw);
          if(state->gyro_info.desired_odr)
          {
            lsm6dsm_set_fifo_wmk(this);
          }
        }
        if((state->gyro_info.desired_odr) && (!state->accel_info.desired_odr))
        {
          state->gyro_info.num_samples_to_discard += lsm6dsm_odr_map[state->common_info.desired_odr_idx].gyro_discard_samples;
        }
      }
      else if(state->self_test_info.sensor == LSM6DSM_GYRO)
      {
        state->gyro_info.self_test_is_successful = false;
        if(!(state->gyro_info.desired_odr) || (state->gyro_info.desired_odr > LSM6DSM_ACCEL_ODR833) || (state->gyro_info.range != STM_LSM6DSM_GYRO_RANGE_2000DPS))
        {
          lsm6dsm_set_gyro_config(this,state->gyro_info.desired_odr,state->gyro_info.sstvt,state->gyro_info.range);
          if(state->accel_info.desired_odr)
          {
            lsm6dsm_set_fifo_wmk(this);
          }
        }
        if((state->accel_info.desired_odr) && (!state->gyro_info.desired_odr))
        {
          state->accel_info.num_samples_to_discard += lsm6dsm_odr_map[state->common_info.desired_odr_idx].accel_discard_samples;
        }
      }
      sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_self_test_data_stream);
      //Disable all self test flags
      state->self_test_info.polling_count = 0;
      state->self_test_info.curr_odr = LSM6DSM_ACCEL_ODR52;
      state->health.heart_attack = false;
      sns_memset(state->self_test_info.cumulative_data_pre, 0, sizeof(state->self_test_info.cumulative_data_pre));
      sns_memset(state->self_test_info.cumulative_data_post, 0, sizeof(state->self_test_info.cumulative_data_post));
    }
    default:
    {
    }
    break;
  }

}

void lsm6dsm_inst_factory_self_test(sns_sensor_instance *const this)
{
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)this->state->state;
  uint32_t timeout = 0;
  lsm6dsm_accel_odr curr_odr = LSM6DSM_ACCEL_ODR52;
  float fac_cal_bias[3] = {0};
  float bias_thresholds[3] = {0};
  float variance[3] = {0};
  float variance_threshold = 0;

  switch (state->self_test_info.self_test_stage)
  {
    case LSM6DSM_SELF_TEST_STAGE_1:
    {
      //Check if sensor is already streaming. If not, turn it ON
      if((state->common_info.accel_curr_odr) && (state->common_info.accel_curr_odr < LSM6DSM_ACCEL_ODR833))
      {
        state->self_test_info.curr_odr =  state->common_info.accel_curr_odr;
        curr_odr = state->self_test_info.curr_odr;
      }
      else
      {
        state->self_test_info.curr_odr = LSM6DSM_ACCEL_ODR52;
      }

      if(state->accel_info.self_test_is_successful)
      {
        state->self_test_info.sensor = LSM6DSM_ACCEL;
        if((state->gyro_info.desired_odr) && !(state->accel_info.desired_odr))
        {
          lsm6dsm_set_fifo_wmk(this);
        }
        lsm6dsm_set_accel_config(this,curr_odr,LSM6DSM_ACCEL_SSTVT_2G,LSM6DSM_ACCEL_RANGE_2G,state->accel_info.bw);
        timeout = 100;
        for(state->self_test_info.odr_idx = 0; state->self_test_info.odr_idx < ARR_SIZE(lsm6dsm_odr_map); state->self_test_info.odr_idx++)
        {
          if(state->self_test_info.curr_odr == lsm6dsm_odr_map[state->self_test_info.odr_idx].accel_odr_reg_value)
          {
            state->self_test_info.skip_count = lsm6dsm_odr_map[state->self_test_info.odr_idx].accel_discard_samples;
            break;
          }
        }
      }
      else
      {
        state->self_test_info.sensor = LSM6DSM_GYRO;
        if((state->accel_info.desired_odr) && !(state->gyro_info.desired_odr))
        {
          lsm6dsm_set_fifo_wmk(this);
        }
        lsm6dsm_set_gyro_config(this,(lsm6dsm_gyro_odr)curr_odr,LSM6DSM_GYRO_SSTVT_2000DPS,STM_LSM6DSM_GYRO_RANGE_2000DPS);
        timeout = 150;
        for(state->self_test_info.odr_idx = 0; state->self_test_info.odr_idx < ARR_SIZE(lsm6dsm_odr_map); state->self_test_info.odr_idx++)
        {
          if(state->self_test_info.curr_odr == lsm6dsm_odr_map[state->self_test_info.odr_idx].gyro_odr_reg_value)
          {
            state->self_test_info.skip_count = lsm6dsm_odr_map[state->self_test_info.odr_idx].gyro_discard_samples;
            break;
          }
        }
      }
      state->self_test_info.self_test_stage = LSM6DSM_SELF_TEST_STAGE_2;

      lsm6dsm_dump_reg(this, state->fifo_info.fifo_enabled);
      sns_time timeout_ticks = sns_convert_ns_to_ticks(timeout * 1000 * 1000);
      lsm6dsm_inst_create_timer(this, &state->timer_self_test_data_stream, timeout_ticks, false); //Settling time for ODR
    }
    break;

    case LSM6DSM_SELF_TEST_STAGE_2:
    {
      lsm6dsm_inst_collect_data(this, state->self_test_info.cumulative_data_pre);
    }
    if(state->self_test_info.self_test_stage == LSM6DSM_SELF_TEST_STAGE_2)
      break;

    case LSM6DSM_SELF_TEST_STAGE_3:
    {
      int i;
      bool test_pass=true;

      DBG_INST_PRINT_EX(this, HIGH, __FILENAME__, __LINE__, "Sample Sums: %d %d %d",
                      (int32_t)state->self_test_info.cumulative_data_pre[0],
                      (int32_t)state->self_test_info.cumulative_data_pre[1],
                      (int32_t)state->self_test_info.cumulative_data_pre[2]);
      DBG_INST_PRINT_EX(this, HIGH, __FILENAME__, __LINE__, "Sample square sums: %d %d %d",
                      (int32_t)state->self_test_info.cumulative_data_post[0],
                      (int32_t)state->self_test_info.cumulative_data_post[1],
                      (int32_t)state->self_test_info.cumulative_data_post[2]);

      for(i = 0; i < TRIAXIS_NUM; i++)
      {
        float varT = (state->self_test_info.cumulative_data_pre[i]*SELF_TEST_DATA_COUNT_MAX) * (state->self_test_info.cumulative_data_pre[i]*SELF_TEST_DATA_COUNT_MAX);

        variance[i] = (state->self_test_info.cumulative_data_post[i]
                                            - (varT / SELF_TEST_DATA_COUNT_MAX)) / SELF_TEST_DATA_COUNT_MAX;

        if(state->self_test_info.sensor == LSM6DSM_ACCEL)
        {
          variance_threshold = 383.0; // 383(m/s2)2

          //Calculate bias based on orientation
          if(state->self_test_info.cumulative_data_pre[i] > 200)
          {
            state->self_test_info.cumulative_data_pre[i] = state->self_test_info.cumulative_data_pre[i] - 1000.0f;
            bias_thresholds[i] = 210 * G / 1000; // 210 m/s2
          }
          else  if(state->self_test_info.cumulative_data_pre[i] < -200)
          {
            state->self_test_info.cumulative_data_pre[i] = state->self_test_info.cumulative_data_pre[i] + 1000.0f;
            bias_thresholds[i] = 210 * G / 1000; // 210 m/s2
          }
          else
          {
            bias_thresholds[i] = 180 * G / 1000; // 180 m/s2
          }

          fac_cal_bias[state->axis_map[i].opaxis] = (state->axis_map[i].invert ? -1.0 : 1.0) * state->self_test_info.cumulative_data_pre[state->axis_map[i].ipaxis]*G/1000.0f;
        }
        else if(state->self_test_info.sensor == LSM6DSM_GYRO)
        {
          variance_threshold = 64.0; // 64(rad/sec)2
          fac_cal_bias[state->axis_map[i].opaxis] = (state->axis_map[i].invert ? -1.0 : 1.0) * state->self_test_info.cumulative_data_pre[state->axis_map[i].ipaxis]*PI/(180.0f);
          bias_thresholds[i] = 40 * PI / 180; //40 rad/sec
        }

        // Check variance to determine whether device is stationary
        if(variance[i] > variance_threshold)
        {
          // device is not stationary
          test_pass = false;
          SNS_INST_PRINTF(ERROR, this, "FAILED device not stationary var[%u]=%u  %u",
                          i, (uint32_t)variance[i],
                          (uint32_t)variance_threshold);
          break;
        }

        // Check biases are in defined limits
        if(fabsf(fac_cal_bias[i]) > bias_thresholds[i])
        {
          test_pass = false;
          SNS_INST_PRINTF(ERROR, this, "FAILED bias very large.");
          break;
        }

        // Check for zero variance
        if(variance[i] == 0.0)
        {
           test_pass = false;
           SNS_INST_PRINTF(ERROR, this, "FAILED zero variance");
           break;
        }
      }

      if(!test_pass)
      {
        send_factory_self_test_result(this, false);
      }
      else
      {
        //Calculate bias values
        if(state->self_test_info.sensor == LSM6DSM_ACCEL)
        {
          //Calculate bias based on orientation
          for(i=0;i<TRIAXIS_NUM;i++)
          {
            state->accel_registry_cfg.fac_cal_bias[state->axis_map[i].opaxis] = fac_cal_bias[state->axis_map[i].opaxis];
          }

          state->accel_registry_cfg.registry_instance_version++;
          DBG_INST_PRINT_EX(this, LOW, __FILENAME__, __LINE__, "Debug: Accel Bias values*1000: X=%d Y=%d Z=%d",
                         (int32_t)(state->accel_registry_cfg.fac_cal_bias[0]*1000),
                         (int32_t)(state->accel_registry_cfg.fac_cal_bias[1]*1000),
                         (int32_t)(state->accel_registry_cfg.fac_cal_bias[2]*1000));
        }
        else if(state->self_test_info.sensor == LSM6DSM_GYRO)
        {
          for(i=0;i<TRIAXIS_NUM;i++)
          {
            state->gyro_registry_cfg.fac_cal_bias[state->axis_map[i].opaxis] = fac_cal_bias[state->axis_map[i].opaxis];
          }
          state->gyro_registry_cfg.registry_instance_version++;
          DBG_INST_PRINT_EX(this, LOW, __FILENAME__, __LINE__, "Debug: Gyro Bias values*1000: X=%d Y=%d Z=%d",
                         (int32_t)(state->gyro_registry_cfg.fac_cal_bias[0]*1000),
                         (int32_t)(state->gyro_registry_cfg.fac_cal_bias[1]*1000),
                         (int32_t)(state->gyro_registry_cfg.fac_cal_bias[2]*1000));
        }

        //Set flag to enable write registory with bias values from sensor set_client_request
        state->self_test_info.update_registry = true;

        //write_output_to_registry(this);
        //Send result
        send_factory_self_test_result(this, true);
      }
      //Disable streaming if started for self test else set the original ODR if more than 833Hz
      if(state->self_test_info.sensor == LSM6DSM_ACCEL)
      {
        state->accel_info.self_test_is_successful = false;
        if(!(state->accel_info.desired_odr) || (state->accel_info.desired_odr > LSM6DSM_ACCEL_ODR833) || (state->accel_info.range != LSM6DSM_ACCEL_RANGE_2G))
        {
          lsm6dsm_set_accel_config(this,state->accel_info.desired_odr,state->accel_info.sstvt,state->accel_info.range,state->accel_info.bw);
          if(state->gyro_info.desired_odr)
          {
            lsm6dsm_set_fifo_wmk(this);
          }
        }
        if((state->gyro_info.desired_odr) && (!state->accel_info.desired_odr))
        {
          state->gyro_info.num_samples_to_discard += lsm6dsm_odr_map[state->common_info.desired_odr_idx].gyro_discard_samples;
        }
      }
      else if(state->self_test_info.sensor == LSM6DSM_GYRO)
      {
        state->gyro_info.self_test_is_successful = false;
        if(!(state->gyro_info.desired_odr) || (state->gyro_info.desired_odr > LSM6DSM_ACCEL_ODR833) || (state->gyro_info.range != STM_LSM6DSM_GYRO_RANGE_2000DPS))
        {
          lsm6dsm_set_gyro_config(this,state->gyro_info.desired_odr,state->gyro_info.sstvt,state->gyro_info.range);
          if(state->accel_info.desired_odr)
          {
            lsm6dsm_set_fifo_wmk(this);
          }
        }
        if((state->accel_info.desired_odr) && (!state->gyro_info.desired_odr))
        {
          state->accel_info.num_samples_to_discard += lsm6dsm_odr_map[state->common_info.desired_odr_idx].accel_discard_samples;
        }
      }
      sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_self_test_data_stream);
      
      //Disable all self test flags
      state->self_test_info.polling_count = 0;
      state->self_test_info.curr_odr = LSM6DSM_ACCEL_ODR52;
      state->health.heart_attack = false;
      sns_memset(state->self_test_info.cumulative_data_pre, 0, sizeof(state->self_test_info.cumulative_data_pre));
      sns_memset(state->self_test_info.cumulative_data_post, 0, sizeof(state->self_test_info.cumulative_data_post));
    }
    default:
    {
    }
    break;
  }

}

void lsm6dsm_inst_com_self_test(sns_sensor_instance *const this)
{
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)this->state->state;
  sns_rc rv = SNS_RC_SUCCESS;
  uint8_t buffer = 0;
  bool who_am_i_success = false;

  rv = lsm6dsm_get_who_am_i(state->scp_service,
                            state->com_port_info.port_handle,
                            &buffer);

  if(rv == SNS_RC_SUCCESS
     &&
     buffer == LSM6DSM_WHOAMI_VALUE)
  {
    who_am_i_success = true;
  }

  //Send result
  send_com_self_test_result(this, who_am_i_success);
  //Disable all self test flags
  state->self_test_info.polling_count = 0;
  state->self_test_info.curr_odr = LSM6DSM_ACCEL_ODR52;
}

static void inst_cleanup(sns_sensor_instance *const this,
                         lsm6dsm_instance_state *state)
{
  // QC: Use util to remove stream
  DBG_INST_PRINT_EX(this, HIGH, __FILENAME__, __LINE__,
      "inst_cleanup:: #samples=%u/%u/%u/%u",
      state->num_accel_samples, state->num_gyro_samples,
      state->num_temp_samples, state->num_md_ints);

  // TODO: Is this OK to do here? Introduced by DAE sensor, since we could be
  // waiting for response from DAE before writing to HW -- but we don't have that chance.
  if(NULL != state->com_port_info.port_handle)
  {
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);
  }
  lsm6dsm_set_fifo_config(this, 0, 0, 0, 0 );
  lsm6dsm_reconfig_hw(this);
  if(NULL != state->com_port_info.port_handle)
  {
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);
  }

  sns_sensor_util_remove_sensor_instance_stream(this, &state->interrupt_data_stream);
  sns_sensor_util_remove_sensor_instance_stream(this, &state->async_com_port_data_stream);
  sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_sensor_temp_data_stream);
  sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_md_data_stream);
  sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_self_test_data_stream);
  sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_heart_beat_data_stream);


  if(NULL != state->scp_service)
  {
    state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
    state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
    state->scp_service = NULL;
  }
  lsm6dsm_dae_if_deinit(this);
}

/** See sns_sensor_instance_api::init */
sns_rc lsm6dsm_inst_init(sns_sensor_instance *const this,
    sns_sensor_state const *sstate)
{
  lsm6dsm_instance_state *state =
              (lsm6dsm_instance_state*)this->state->state;
  lsm6dsm_state *sensor_state =
              (lsm6dsm_state*)sstate->state;
  float data[3];
  float temp_data[1];
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
  uint64_t buffer[10];
  pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float diag_temp[LSM6DSM_NUM_AXES];
  pb_float_arr_arg arg = {.arr = (float*)diag_temp, .arr_len = LSM6DSM_NUM_AXES,
    .arr_index = &arr_index};
  batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
  batch_sample.sample.arg = &arg;
  //this would be stored when the state is initialized
  uint32_t hw_id = sensor_state->hw_idx;

  state->scp_service = (sns_sync_com_port_service*)
              service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

  /**---------Setup stream connections with dependent Sensors---------*/
  stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                 this,
                                                 sensor_state->irq_suid,
                                                 &state->interrupt_data_stream);

  stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                 this,
                                                 sensor_state->acp_suid,
                                                 &state->async_com_port_data_stream);

  state->timer_self_test_data_stream = NULL;
  state->timer_md_data_stream = NULL;
  state->timer_sensor_temp_data_stream = NULL;
  state->timer_heart_beat_data_stream = NULL;

  /** Initialize COM port to be used by the Instance */
  sns_memscpy(&state->com_port_info.com_config,
              sizeof(state->com_port_info.com_config),
              &sensor_state->com_port_info.com_config,
              sizeof(sensor_state->com_port_info.com_config));

  state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
                                              &state->com_port_info.port_handle);

  if(NULL == state->interrupt_data_stream ||
     NULL == state->async_com_port_data_stream ||
     NULL == state->com_port_info.port_handle)
  {
    inst_cleanup(this, state);
    return SNS_RC_FAILED;
  }

  /**----------- Copy all Sensor UIDs in instance state -------------*/
  if(hw_id == 0) {
    sns_memscpy(&state->accel_info.suid,
        sizeof(state->accel_info.suid),
        &((sns_sensor_uid)ACCEL_SUID_0),
        sizeof(state->accel_info.suid));
    sns_memscpy(&state->gyro_info.suid,
        sizeof(state->gyro_info.suid),
        &((sns_sensor_uid)GYRO_SUID_0),
        sizeof(state->gyro_info.suid));
    sns_memscpy(&state->md_info.suid,
        sizeof(state->md_info.suid),
        &((sns_sensor_uid)MOTION_DETECT_SUID_0),
        sizeof(state->md_info.suid));
    sns_memscpy(&state->sensor_temp_info.suid,
        sizeof(state->sensor_temp_info.suid),
        &((sns_sensor_uid)SENSOR_TEMPERATURE_SUID_0),
        sizeof(state->sensor_temp_info.suid));
    sns_memscpy(&state->timer_suid,
        sizeof(state->timer_suid),
        &(sensor_state->timer_suid),
        sizeof(sensor_state->timer_suid));
  }
#if DUAL_SENSOR_SUPPORT
  else {
    sns_memscpy(&state->accel_info.suid,
        sizeof(state->accel_info.suid),
        &((sns_sensor_uid)ACCEL_SUID_1),
        sizeof(state->accel_info.suid));
    sns_memscpy(&state->gyro_info.suid,
        sizeof(state->gyro_info.suid),
        &((sns_sensor_uid)GYRO_SUID_1),
        sizeof(state->gyro_info.suid));
    sns_memscpy(&state->md_info.suid,
        sizeof(state->md_info.suid),
        &((sns_sensor_uid)MOTION_DETECT_SUID_1),
        sizeof(state->md_info.suid));
    sns_memscpy(&state->sensor_temp_info.suid,
        sizeof(state->sensor_temp_info.suid),
        &((sns_sensor_uid)SENSOR_TEMPERATURE_SUID_1),
        sizeof(state->sensor_temp_info.suid));
    sns_memscpy(&state->timer_suid,
        sizeof(state->timer_suid),
        &(sensor_state->timer_suid),
        sizeof(sensor_state->timer_suid));
  }
#endif

  state->hw_idx = hw_id;
  state->config_sensors = 0;
  /**-------------------------Init FIFO State-------------------------*/
  state->fifo_info.fifo_enabled = 0;
  state->fifo_info.inst_publish_sensors = 0;
  state->fifo_info.fifo_rate = LSM6DSM_ACCEL_ODR_OFF;
  state->fifo_info.cur_wmk = 0;
  state->fifo_info.max_requested_wmk = 0;
  state->fifo_info.interrupt_cnt = 0;
  state->fifo_info.timer_active = false;
  state->accel_sample_counter = 0;
  state->gyro_sample_counter = 0;

  /**-------------------------Init Accel State-------------------------*/
  state->common_info.accel_curr_odr = LSM6DSM_ACCEL_ODR_OFF;
  state->accel_info.sstvt = lsm6dsm_accel_resolutions[sensor_state->accel_resolution_idx]*1000;
  state->accel_info.range = lsm6dsm_accel_ranges[sensor_state->accel_resolution_idx];
  state->accel_info.bw = LSM6DSM_ACCEL_BW50;
  state->accel_info.lp_mode = false;
  sns_memset(state->accel_info.opdata_raw, 0, sizeof(state->accel_info.opdata_raw));
  state->accel_info.opdata_status = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;

  /**-------------------------Init Gyro State-------------------------*/
  state->common_info.gyro_curr_odr = LSM6DSM_GYRO_ODR_OFF;
  state->gyro_info.sstvt = lsm6dsm_gyro_resolutions[sensor_state->gyro_resolution_idx];
  state->gyro_info.range = lsm6dsm_gyro_ranges[sensor_state->gyro_resolution_idx];
  sns_memset(state->gyro_info.opdata_raw, 0, sizeof(state->gyro_info.opdata_raw));
  state->gyro_info.opdata_status = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;

  /**-------------------------Init MD State-------------------------*/
  state->md_info.cur_md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_DISABLED;
  state->md_info.prev_md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_DISABLED;

  /**-------------------------Init Current Configure State-------------------------*/
  state->current_conf.odr = 0;
  state->current_conf.fifo_odr = 0;
  state->current_conf.wmk = 0;
  state->current_conf.enabled_sensors = 0;
  state->current_conf.md_enabled = false;

  /**-------------------------Init Desired Configure State-------------------------*/
  state->desired_conf.odr = 0;
  state->desired_conf.fifo_odr = 0;
  state->desired_conf.wmk = 0;
  state->desired_conf.enabled_sensors = 0;
  state->desired_conf.md_enabled = false;

  /**-------------------------Init Self Test State-------------------------*/
  sns_memset(state->self_test_info.cumulative_data_pre, 0, sizeof(state->self_test_info.cumulative_data_pre));
  sns_memset(state->self_test_info.cumulative_data_post, 0, sizeof(state->self_test_info.cumulative_data_post));
  state->self_test_info.polling_count = 0;
  state->self_test_info.self_test_stage = LSM6DSM_SELF_TEST_STAGE_1;
  state->self_test_info.skip_count = 0;
  state->self_test_info.test_type = SNS_PHYSICAL_SENSOR_TEST_TYPE_HW;
  state->self_test_info.update_registry = false;
  state->self_test_info.test_alive = false;
  state->self_test_info.reconfig_postpone = false;

  state->health.heart_attack = false;
  state->health.heart_attack_cnt = 0;
  state->encoded_imu_event_len = pb_get_encoded_size_sensor_stream_event(data, 3);
  state->encoded_sensor_temp_event_len = pb_get_encoded_size_sensor_stream_event(temp_data, 1);

  state->diag_service =  (sns_diag_service*)
      service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);

  state->scp_service =  (sns_sync_com_port_service*)
      service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

  state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);

  lsm6dsm_reset_device( this, LSM6DSM_ACCEL | LSM6DSM_GYRO | LSM6DSM_MOTION_DETECT | LSM6DSM_SENSOR_TEMP);

  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                                           false);
  /** Initialize IRQ info to be used by the Instance */
  state->irq_info.irq_config     = sensor_state->irq_config;
  state->irq_info.irq_registered = false;
  state->irq_info.irq_ready      = false;

  /** Initialize com config to be used by the Instance */
  {
    sns_com_port_config *com_config = &sensor_state->com_port_info.com_config;
    state->ascp_config.bus_type          = (com_config->bus_type == SNS_BUS_I2C) ?
      SNS_ASYNC_COM_PORT_BUS_TYPE_I2C : SNS_ASYNC_COM_PORT_BUS_TYPE_SPI;
    state->ascp_config.slave_control     = com_config->slave_control;
    state->ascp_config.reg_addr_type     = SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_8_BIT;
    state->ascp_config.min_bus_speed_kHz = com_config->min_bus_speed_KHz;
    state->ascp_config.max_bus_speed_kHz = com_config->max_bus_speed_KHz;
    state->ascp_config.bus_instance      = com_config->bus_instance;
  }

  state->ascp_req_count = 0;
  /** Configure the Async Com Port */
  {
    sns_data_stream* data_stream = state->async_com_port_data_stream;
    uint8_t pb_encode_buffer[100];
    sns_request async_com_port_request =
    {
      .message_id  = SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_CONFIG,
      .request     = &pb_encode_buffer
    };

    async_com_port_request.request_len =
      pb_encode_request(pb_encode_buffer,
                        sizeof(pb_encode_buffer),
                        &state->ascp_config,
                        sns_async_com_port_config_fields,
                        NULL);
    data_stream->api->send_request(data_stream, &async_com_port_request);
  }

  /** Copy down axis conversion settings */
  sns_memscpy(state->axis_map,  sizeof(sensor_state->axis_map),
              sensor_state->axis_map, sizeof(sensor_state->axis_map));

  /** Initialize factory calibration */
  state->accel_registry_cfg.fac_cal_corr_mat.e00 = 1.0;
  state->accel_registry_cfg.fac_cal_corr_mat.e11 = 1.0;
  state->accel_registry_cfg.fac_cal_corr_mat.e22 = 1.0;
  state->gyro_registry_cfg.fac_cal_corr_mat.e00 = 1.0;
  state->gyro_registry_cfg.fac_cal_corr_mat.e11 = 1.0;
  state->gyro_registry_cfg.fac_cal_corr_mat.e22 = 1.0;
  state->sensor_temp_registry_cfg.fac_cal_corr_mat.e00 = 1.0;
  state->sensor_temp_registry_cfg.fac_cal_corr_mat.e11 = 1.0;
  state->sensor_temp_registry_cfg.fac_cal_corr_mat.e22 = 1.0;

  /** Determine sizes of encoded logs */
  sns_diag_sensor_state_interrupt sensor_state_interrupt =
        sns_diag_sensor_state_interrupt_init_default;
  pb_get_encoded_size(&state->log_interrupt_encoded_size,
                      sns_diag_sensor_state_interrupt_fields,
                      &sensor_state_interrupt);

  /** Determine size of sns_diag_sensor_state_raw as defined in
   *  sns_diag.proto
   *  sns_diag_sensor_state_raw is a repeated array of samples of
   *  type sns_diag_batch sample. The following determines the
   *  size of sns_diag_sensor_state_raw with a single batch
   *  sample */
  if(pb_encode_tag(&stream, PB_WT_STRING,
                    sns_diag_sensor_state_raw_sample_tag))
  {
    if(pb_encode_delimited(&stream, sns_diag_batch_sample_fields,
                               &batch_sample))
    {
      state->log_raw_encoded_size = stream.bytes_written;
    }
  }
  //get the temp sensor log encoded size
  arg.arr_len = 1;
  if(pb_encode_tag(&stream, PB_WT_STRING,
        sns_diag_sensor_state_raw_sample_tag))
  {
    if(pb_encode_delimited(&stream, sns_diag_batch_sample_fields,
          &batch_sample))
    {
      state->log_temp_raw_encoded_size = stream.bytes_written;
    }
  }

  lsm6dsm_dae_if_init(this, stream_mgr, &sensor_state->dae_suid);

  return SNS_RC_SUCCESS;
}

sns_rc lsm6dsm_inst_deinit(sns_sensor_instance *const this)
{
  lsm6dsm_instance_state *state =
                  (lsm6dsm_instance_state*)this->state->state;
  inst_cleanup(this, state);

  return SNS_RC_SUCCESS;
}

void lsm6dsm_set_client_test_config(sns_sensor_instance *this)
{
  lsm6dsm_instance_state *state = (lsm6dsm_instance_state*)this->state->state;

  // 1. Extract test type from client_request.
  // 2. Configure sensor HW for test type.
  // 3. send_request() for Timer Sensor in case test needs polling/waits.
  // 4. Factory test is TBD.
  if(state->accel_info.self_test_is_successful)
    state->self_test_info.sensor = LSM6DSM_ACCEL;
  else if(state->gyro_info.self_test_is_successful)
    state->self_test_info.sensor = LSM6DSM_GYRO;
  else if(state->md_info.self_test_is_successful)
    state->self_test_info.sensor = LSM6DSM_MOTION_DETECT;
  else if(state->sensor_temp_info.self_test_is_successful)
    state->self_test_info.sensor = LSM6DSM_SENSOR_TEMP;

  state->self_test_info.test_alive = true;
  DBG_INST_PRINT_EX(this, HIGH, __FILENAME__, __LINE__,
      "Self test start, Test Type = %d, %d",
      state->self_test_info.test_type, state->self_test_info.sensor);
  //start HW self test
  if(state->self_test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_HW)
  {
    state->self_test_info.self_test_stage = LSM6DSM_SELF_TEST_STAGE_1;
    lsm6dsm_inst_hw_self_test(this);
  }
  else if(state->self_test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
  {
    state->self_test_info.self_test_stage = LSM6DSM_SELF_TEST_STAGE_1;
    lsm6dsm_inst_factory_self_test(this);
  }
  else if(state->self_test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
  {
    lsm6dsm_inst_com_self_test(this);
  }
  else
  {
    DBG_INST_PRINT_EX(this, HIGH, __FILENAME__, __LINE__,
                 "Unsupported test, Test Type = %d", state->self_test_info.test_type);
    state->self_test_info.test_alive = false;
  }
}
