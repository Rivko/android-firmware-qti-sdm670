/**
 * @file sns_lsm6ds3_sensor_instance_island.c
 *
 * LSM6DS3 Accel virtual Sensor Instance implementation for
 * island mode.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc. All Rights
 * Reserved. Confidential and Proprietary - Qualcomm
 * Technologies, Inc.
 **/

#include "sns_island_service.h"
#include "sns_mem_util.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_event_service.h"

#include "sns_time.h"
#include "sns_types.h"

#include "sns_lsm6ds3_hal.h"
#include "sns_lsm6ds3_sensor.h"
#include "sns_lsm6ds3_sensor_instance.h"

#include "sns_async_com_port.pb.h"
#include "sns_interrupt.pb.h"
#include "sns_timer.pb.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#include "sns_pb_util.h"
#include "sns_sensor_util.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_sync_com_port_service.h"
#include "sns_printf.h"

const odr_reg_map reg_map[LSM6DS3_REG_MAP_TABLE_SIZE] =
{
  {
    .odr = LSM6DS3_ODR_0,
    .accel_odr_reg_value = LSM6DS3_ACCEL_ODR_OFF,
    .gyro_odr_reg_value = LSM6DS3_GYRO_ODR_OFF,
    .discard_samples = 0
  },
  {
    .odr = LSM6DS3_ODR_13,
    .accel_odr_reg_value = LSM6DS3_ACCEL_ODR13,
    .gyro_odr_reg_value = LSM6DS3_GYRO_ODR13,
    .discard_samples = 2
  },
  {
    .odr = LSM6DS3_ODR_26,
    .accel_odr_reg_value = LSM6DS3_ACCEL_ODR26,
    .gyro_odr_reg_value = LSM6DS3_GYRO_ODR26,
    .discard_samples = 2
  },
  {
    .odr = LSM6DS3_ODR_52,
    .accel_odr_reg_value = LSM6DS3_ACCEL_ODR52,
    .gyro_odr_reg_value = LSM6DS3_GYRO_ODR52,
    .discard_samples = 2
  },
  {
    .odr = LSM6DS3_ODR_104,
    .accel_odr_reg_value = LSM6DS3_ACCEL_ODR104,
    .gyro_odr_reg_value = LSM6DS3_GYRO_ODR104,
    .discard_samples = 4
  },
  {
    .odr = LSM6DS3_ODR_208,
    .accel_odr_reg_value = LSM6DS3_ACCEL_ODR208,
    .gyro_odr_reg_value = LSM6DS3_GYRO_ODR208,
    .discard_samples = 5
  },
  {
    .odr = LSM6DS3_ODR_416,
    .accel_odr_reg_value = LSM6DS3_ACCEL_ODR416,
    .gyro_odr_reg_value = LSM6DS3_GYRO_ODR416,
    .discard_samples = 10
  },
  {
    .odr = LSM6DS3_ODR_833,
    .accel_odr_reg_value = LSM6DS3_ACCEL_ODR833,
    .gyro_odr_reg_value = LSM6DS3_GYRO_ODR833,
    .discard_samples = 2
  },
  {
    .odr = LSM6DS3_ODR_1660,
    .accel_odr_reg_value = LSM6DS3_ACCEL_ODR1660,
    .gyro_odr_reg_value = LSM6DS3_GYRO_ODR1660,
    .discard_samples = 2
  },
  {
    .odr = LSM6DS3_ODR_3330,
    .accel_odr_reg_value = LSM6DS3_ACCEL_ODR3330,
    .gyro_odr_reg_value = LSM6DS3_GYRO_ODR1660,
    .discard_samples = 2
  },
  {
    .odr = LSM6DS3_ODR_6660,
    .accel_odr_reg_value = LSM6DS3_ACCEL_ODR6660,
    .gyro_odr_reg_value = LSM6DS3_GYRO_ODR1660,
    .discard_samples = 2
  },
};

static sns_rc lsm6ds3_accel_match_odr(float desired_sample_rate,
                                      float *chosen_sample_rate,
                                      lsm6ds3_accel_odr *chosen_reg_value,
                                      uint8_t *num_samples_to_discard)
{
  uint8_t idx;

  if((LSM6DS3_ODR_416 < desired_sample_rate)
     ||
     NULL == chosen_sample_rate
     ||
     NULL == chosen_reg_value)
  {
    return SNS_RC_NOT_SUPPORTED;
  }

  for(idx = 0; idx < ARR_SIZE(reg_map); idx++)
  {
    if(desired_sample_rate <= reg_map[idx].odr)
    {
      break;
    }
  }

  if (idx >= ARR_SIZE(reg_map))
  {
    return SNS_RC_NOT_SUPPORTED;
  }

  *chosen_sample_rate = reg_map[idx].odr;
  *chosen_reg_value = reg_map[idx].accel_odr_reg_value;
  *num_samples_to_discard = reg_map[idx].discard_samples;

  return SNS_RC_SUCCESS;
}

static sns_rc lsm6ds3_gyro_match_odr(float desired_sample_rate,
                                     float *chosen_sample_rate,
                                     lsm6ds3_gyro_odr *chosen_reg_value)
{
  uint8_t idx;

  if((LSM6DS3_ODR_416 < desired_sample_rate)
     ||
     NULL == chosen_sample_rate
     ||
     NULL == chosen_reg_value)
  {
    return SNS_RC_NOT_SUPPORTED;
  }

  for(idx = 0; idx < ARR_SIZE(reg_map); idx++)
  {
    if(desired_sample_rate <= reg_map[idx].odr)
    {
      break;
    }
  }

  if (idx >= ARR_SIZE(reg_map))
  {
    return SNS_RC_NOT_SUPPORTED;
  }

  *chosen_sample_rate = reg_map[idx].odr;
  *chosen_reg_value = reg_map[idx].gyro_odr_reg_value;

  return SNS_RC_SUCCESS;
}

static sns_rc lsm6ds3_validate_sensor_temp_odr(lsm6ds3_instance_state *state)
{
  sns_rc rc = SNS_RC_SUCCESS;
  if(state->sensor_temp_info.sampling_rate_hz <= LSM6DS3_SENSOR_TEMP_ODR_1)
  {
    state->sensor_temp_info.sampling_rate_hz = LSM6DS3_SENSOR_TEMP_ODR_1;
  }
  else if(state->sensor_temp_info.sampling_rate_hz > LSM6DS3_SENSOR_TEMP_ODR_1
          &&
          state->sensor_temp_info.sampling_rate_hz <= LSM6DS3_SENSOR_TEMP_ODR_5)
  {
    state->sensor_temp_info.sampling_rate_hz = LSM6DS3_SENSOR_TEMP_ODR_5;
  }
  else
  {
    state->sensor_temp_info.sampling_intvl = 0;
    rc = SNS_RC_NOT_SUPPORTED;
  }

  if (rc == SNS_RC_SUCCESS)
  {
    state->sensor_temp_info.sampling_intvl =
      sns_convert_ns_to_ticks(1000000000.0 / state->sensor_temp_info.sampling_rate_hz);
  }

  return rc;
}

/** See sns_sensor_instance_api::notify_event */
static sns_rc lsm6ds3_inst_notify_event(sns_sensor_instance *const this)
{
  lsm6ds3_instance_state *state =
         (lsm6ds3_instance_state*)this->state->state;
  sns_interrupt_event irq_event = sns_interrupt_event_init_zero;
  sns_sensor_event *event;

  lsm6ds3_dae_if_process_events(this);

  // Turn COM port ON
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                                           true);
  // Handle interrupts
  if(NULL != state->interrupt_data_stream)
  {
    event = state->interrupt_data_stream->api->peek_input(state->interrupt_data_stream);
    while(NULL != event)
    {
      if (event->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT)
      {
        state->irq_info.irq_ready = true;
        if(state->md_info.enable_md_int)
        {
          lsm6ds3_update_md_intr(this, true, false);
        }
        if(state->fifo_info.publish_sensors & (LSM6DS3_ACCEL | LSM6DS3_GYRO))
        {
          lsm6ds3_enable_fifo_intr(state, state->fifo_info.fifo_enabled);
        }
      }
      else if (event->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT)
      {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                     event->event_len);
        if(pb_decode(&stream, sns_interrupt_event_fields, &irq_event))
        {
          /** lsm6ds3_read_gpio() and lsm6ds3_write_gpio() is example only
          to demonstrate the gpio service. These functions are not needed
          for the functioning of this driver.*/
          /*  Commented it out to unblock island mode */
//          lsm6ds3_read_gpio(this, state->irq_info.irq_config.interrupt_num,
//                            state->irq_info.irq_config.is_chip_pin);

/** GPIO 80 is not dedicated for sensors in case of
 *  SSC_TARGET_HEXAGON_CORE_QDSP6_2_0
 *  Commented it out to unblock island mode */
//#ifndef SSC_TARGET_HEXAGON_CORE_QDSP6_2_0
//          /** GPIO 80 is LDO_EN pin. */
//          lsm6ds3_write_gpio(this, 80, true,
//                             SNS_GPIO_DRIVE_STRENGTH_2_MILLI_AMP,
//                             SNS_GPIO_PULL_TYPE_NO_PULL,
//                             SNS_GPIO_STATE_HIGH);
//#endif
          if(state->md_info.enable_md_int)
          {
            /**
             * 1. Handle MD interrupt: Send MD fired event to client.
             * 2. Disable MD.
             * 3. Start Gated Accel FIFO stream with desired config.
             */
            lsm6ds3_handle_md_interrupt(this, irq_event.timestamp);
            if(state->md_info.md_state.motion_detect_event_type
               == SNS_MOTION_DETECT_EVENT_TYPE_FIRED)
            {
              state->md_info.enable_md_int = false;
              lsm6ds3_update_md_intr(this, false, false);
              lsm6ds3_set_md_config(state, false);
              state->md_info.md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_FIRED;
              if(state->accel_info.gated_client_present)
              {
                lsm6ds3_set_fifo_config(state,
                                        state->md_info.desired_wmk,
                                        state->md_info.desired_odr,
                                        state->gyro_info.curr_odr,//LSM6DS3_GYRO_ODR_OFF,
                                        state->fifo_info.fifo_enabled);
		// No need to reconfigure H/W, When there was a gyro streaming
	        if(!((state->fifo_info.publish_sensors&LSM6DS3_GYRO) == LSM6DS3_GYRO))
	        {
                  lsm6ds3_stop_fifo_streaming(state);
                  lsm6ds3_set_fifo_wmk(this);
                  lsm6ds3_start_fifo_streaming(state);
                  lsm6ds3_enable_fifo_intr(state, state->fifo_info.fifo_enabled);                
                  lsm6ds3_dae_if_start_streaming(this);
		}
                lsm6ds3_send_config_event(this);
              }
              lsm6ds3_dump_reg(this, state->fifo_info.fifo_enabled);
            }
          }

          if(state->fifo_info.cur_wmk > 0
             && state->fifo_info.fifo_rate > LSM6DS3_ACCEL_ODR_OFF)
          {             		
             state->fifo_info.interrupt_cnt++;
			 state->interrupt_fired = true;
             lsm6ds3_handle_fifo_data(this,irq_event.timestamp);
          }
        }
      }
      else
      {
        SNS_INST_PRINTF(ERROR, this, "Received invalid event id=%d",
                                      event->message_id);
      }
      event = state->interrupt_data_stream->api->get_next_input(state->interrupt_data_stream);
    }
  }

  // Handle Async Com Port events
  if(NULL != state->async_com_port_data_stream)
  {
    event = state->async_com_port_data_stream->api->peek_input(state->async_com_port_data_stream);
    while(NULL != event)
    {
      if(event->message_id == SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_ERROR)
      {
        //TODO: Warning;
        SNS_INST_PRINTF(ERROR, this, "Received ASCP error event id=%d",
                                      event->message_id);
      }
      else if(event->message_id == SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW)
      {
        pb_istream_t stream = pb_istream_from_buffer((uint8_t *)event->event, event->event_len);
		state->ascp_in_progress = false;
        sns_ascp_for_each_vector_do(&stream, lsm6ds3_process_com_port_vector, (void *)this);
        // Check if still there is any FIFO data  
        lsm6ds3_handle_fifo_data(this,sns_get_system_time());
        if(state->fifo_flush_in_progress)
        {
          state->fifo_flush_in_progress = false;
          if(state->fifo_info.publish_sensors & LSM6DS3_ACCEL)
          {
            lsm6ds3_send_fifo_flush_done(this, &state->accel_info.suid);
          }
          if(state->fifo_info.publish_sensors & LSM6DS3_GYRO)
          {
            lsm6ds3_send_fifo_flush_done(this, &state->gyro_info.suid);
          }
        }

        if(state->fac_test_in_progress)
        {
          /** All self-tests can be handled in normal mode. */
          state->island_service->api->sensor_instance_island_exit(state->island_service, this);
          lsm6ds3_process_fac_test(this);
        }

       /** lsm6ds3_read_gpio() and lsm6ds3_write_gpio() is example only
          to demonstrate the gpio service. These functions are not needed
          for the functioning of this driver.*/
       /*  Commented it out to unblock island mode */
//       lsm6ds3_read_gpio(this, state->irq_info.irq_config.interrupt_num,
//                         state->irq_info.irq_config.is_chip_pin);

/** GPIO 80 is not dedicated for sensors in case of
 *  SSC_TARGET_HEXAGON_CORE_QDSP6_2_0 
 *  Commented it out to unblock island mode */
 
//#ifndef SSC_TARGET_HEXAGON_CORE_QDSP6_2_0
//       /** GPIO 80 is LDO_EN pin. */
//       lsm6ds3_write_gpio(this, 80, true,
//                          SNS_GPIO_DRIVE_STRENGTH_2_MILLI_AMP,
//                          SNS_GPIO_PULL_TYPE_NO_PULL,
//                          SNS_GPIO_STATE_LOW);
//#endif

      }
      event = state->async_com_port_data_stream->api->get_next_input(state->async_com_port_data_stream);
    }
  }

  // Handle Timer events
  if(NULL != state->timer_data_stream)
  {
    event = state->timer_data_stream->api->peek_input(state->timer_data_stream);
    while(NULL != event)
    {
      pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                     event->event_len);
      if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
      {
        sns_timer_sensor_event timer_event;
        if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
        {
          if(state->fifo_info.publish_sensors & LSM6DS3_SENSOR_TEMP
             &&
             state->sensor_temp_info.timer_is_active
             &&
             state->sensor_temp_info.sampling_intvl > 0)
          {
            lsm6ds3_handle_sensor_temp_sample(this, timer_event.timeout_time);
          }
        }
      }
      else if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_REG_EVENT)
      {
        /** TODO: decode and qse timer_reg_event*/
        LSM6DS3_INST_PRINTF(LOW, this, "TIMER_SENSOR_REG_EVENT");
      }
      else
      {
        LSM6DS3_INST_PRINTF(MED, this, "unknown message_id %d", event->message_id);
      }
      event = state->timer_data_stream->api->get_next_input(state->timer_data_stream);
    }
  }
   if(NULL != state->timer_heart_beat_data_stream)
  {
    event = state->timer_heart_beat_data_stream->api->peek_input(state->timer_heart_beat_data_stream);
    while(NULL != event)
    {
      pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
          event->event_len);
      sns_timer_sensor_event timer_event;
      if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
      {
        if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
        { 
          LSM6DS3_INST_PRINTF(LOW, this, "Heart beat timer fired");
          if(!state->ascp_in_progress)
          {
            if(!(state->fifo_info.publish_sensors & (LSM6DS3_GYRO | LSM6DS3_ACCEL)))
            {
              lsm6ds3_handle_fifo_data(this,sns_get_system_time());
            }else
            {
              sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_heart_beat_data_stream);
              break;
            }
          }
        }
        else
        {
          LSM6DS3_INST_PRINTF(LOW, this, "Heart beat timer message_id:%u", event->message_id);
        }   
      }
	  event = state->timer_heart_beat_data_stream->api->get_next_input(state->timer_heart_beat_data_stream);
    }
  }  
  // Turn COM port OFF
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                                           false);
  return SNS_RC_SUCCESS;
}

/** See sns_sensor_instance_api::set_client_config */
static sns_rc lsm6ds3_inst_set_client_config(sns_sensor_instance *const this,
                                             sns_request const *client_request)
{
  lsm6ds3_instance_state *state =
                  (lsm6ds3_instance_state*)this->state->state;
  float desired_sample_rate = 0.0;
  float desired_report_rate = 0.0;
  float accel_chosen_sample_rate = 0.0;
  float gyro_chosen_sample_rate = 0.0;
  lsm6ds3_accel_odr accel_chosen_sample_rate_reg_value = LSM6DS3_ACCEL_ODR_OFF;
  lsm6ds3_gyro_odr gyro_chosen_sample_rate_reg_value = LSM6DS3_GYRO_ODR_OFF;
  uint16_t desired_wmk = 0;
  sns_rc rv = SNS_RC_SUCCESS;
  uint8_t num_samples_to_discard = 0;
  sns_lsm6ds3_req *payload = (sns_lsm6ds3_req*)client_request->request;
  float *fac_cal_bias = NULL;
  matrix3 *fac_cal_corr_mat = NULL;
  uint32_t flush_period_buffer_usec = 0;
  bool fifo_odr_change = false;
  // Usefull only, if there is only temp sensor ODR change
  bool temp_odr_change = false;
  sns_service_manager *mgr = this->cb->get_service_manager(this);
  sns_event_service *event_service = (sns_event_service*)mgr->get_service(mgr, SNS_EVENT_SERVICE);
  LSM6DS3_INST_PRINTF(LOW, this, "set_client_cfg msg_id %d", client_request->message_id);

  // Turn COM port ON
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                                           true);
  state->fifo_info.wmk_change = false;
  if(!lsm6ds3_dae_if_available(this))
  {
    lsm6ds3_register_interrupt(this);
  }

  if(client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
  {
    if(state->fifo_info.publish_sensors != 0 && state->fifo_info.fifo_rate != 0)
    {
      lsm6ds3_send_config_event(this); // sends current config
    }
    // 1. Extract sample, report rates from client_request.
    // 2. Configure sensor HW.
    // 3. sendRequest() for Timer to start/stop in case of polling using timer_data_stream.
    // 4. sendRequest() for Intrerupt register/de-register in case of DRI using interrupt_data_stream.
    // 5. Save the current config information like type, sample_rate, report_rate, etc.
    desired_sample_rate = payload->desired_sample_rate;
    desired_report_rate = payload->desired_report_rate;

    LSM6DS3_INST_PRINTF(LOW, this, "rr %d  sr %d", (uint32_t)desired_report_rate, (uint32_t)desired_sample_rate);

    if(desired_report_rate > desired_sample_rate)
    {
      // bad request. Return error or default report_rate to sample_rate
      desired_report_rate = desired_sample_rate;
    }

    if(state->fifo_info.fifo_enabled & LSM6DS3_ACCEL)
    {
      rv = lsm6ds3_accel_match_odr(desired_sample_rate,
                                   &accel_chosen_sample_rate,
                                   &accel_chosen_sample_rate_reg_value,
                                   &num_samples_to_discard);
      if(rv != SNS_RC_SUCCESS)
      {
        SNS_INST_PRINTF(ERROR, this, "accel ODR match error %d", rv);
		//Allocate and send immediately.
        sns_sensor_event *event = event_service->api->alloc_event(event_service, this, 0);
        if(NULL != event)
        {
          event->message_id = SNS_STD_MSGID_SNS_STD_ERROR_EVENT;
          event->event_len = 0;
          event->timestamp = sns_get_system_time();
          event_service->api->publish_event(event_service, this, event, &state->accel_info.suid);
        }
        //return rv;
      }
    }

    if(state->fifo_info.fifo_enabled & LSM6DS3_GYRO)
    {
      rv = lsm6ds3_gyro_match_odr(desired_sample_rate,
                                  &gyro_chosen_sample_rate,
                                  &gyro_chosen_sample_rate_reg_value);    
      if(rv != SNS_RC_SUCCESS)
      {
        SNS_INST_PRINTF(ERROR, this, "gyro ODR match error %d", rv);
		//Allocate and send immediately.
        sns_sensor_event *event = event_service->api->alloc_event(event_service, this, 0);
        if(NULL != event)
        {
          event->message_id = SNS_STD_MSGID_SNS_STD_ERROR_EVENT;
          event->event_len = 0;
          event->timestamp = sns_get_system_time();
          event_service->api->publish_event(event_service, this, event, &state->gyro_info.suid);
        }
        //return rv;
      }
    }

    if(state->fifo_info.publish_sensors & LSM6DS3_SENSOR_TEMP)
    {
      sns_time temp_interval = state->sensor_temp_info.sampling_intvl;
      rv = lsm6ds3_validate_sensor_temp_odr(state);
	  if(temp_interval != state->sensor_temp_info.sampling_intvl)
	  {
	    temp_odr_change = true;
	  }
      if(rv != SNS_RC_SUCCESS)
      {        
        SNS_INST_PRINTF(ERROR, this, "sensor_temp ODR match error %d", rv);
		//Allocate and send immediately.
        sns_sensor_event *event = event_service->api->alloc_event(event_service, this, 0);
        if(NULL != event)
        {
          event->message_id = SNS_STD_MSGID_SNS_STD_ERROR_EVENT;
          event->event_len = 0;
          event->timestamp = sns_get_system_time();
          event_service->api->publish_event(event_service, this, event,
		  	&state->sensor_temp_info.suid);
        }
        //return rv;
      }
    }
    LSM6DS3_INST_PRINTF(LOW, this, "accel odr %u,running odr %u,fifo running odr:%u",
	  accel_chosen_sample_rate_reg_value,state->accel_info.curr_odr,
	  state->fifo_info.configured_odr);
	 
    if(((state->fifo_info.configured_odr != accel_chosen_sample_rate_reg_value)&&
	   (!state->md_info.enable_md_int)) || 
      (state->gyro_info.curr_odr != gyro_chosen_sample_rate_reg_value))
    {
      fifo_odr_change = true;
  	  state->fifo_info.num_samples_to_discard = num_samples_to_discard;

    }
    else
    {
      state->accel_info.num_samples_to_discard = 0;
      state->gyro_info.num_samples_to_discard = 0;
    }
    if(desired_report_rate != 0)
    {
      desired_wmk = (uint16_t)(accel_chosen_sample_rate / desired_report_rate);
    }
    else
    {
      desired_wmk = 1;
    }

    if(LSM6DS3_MAX_FIFO <= desired_wmk)
    {
      desired_wmk = LSM6DS3_MAX_FIFO;
    }
    
    LSM6DS3_INST_PRINTF(LOW, this, "desired_wmk %u,running wmk %u",
	  desired_wmk,state->fifo_info.cur_wmk);
	if(desired_wmk != state->fifo_info.cur_wmk)
	{
	  state->fifo_info.wmk_change = true;
    }	
	// Calculate Heart beat timeout based on ODR and wmk
    state->heart_beat_timeout = 
      sns_convert_ns_to_ticks(1000000000.0 /accel_chosen_sample_rate) * LSM6DS3_HEART_BEAT_ODR_COUNT * desired_wmk ;	
    if(state->md_info.enable_md_int)
    {
      lsm6ds3_set_gated_accel_config(state, desired_wmk,
                                     accel_chosen_sample_rate_reg_value,
                                     state->fifo_info.fifo_enabled);	  
	  // If motion detect is already enabled. No need to re-configure md interrupt
	  if(state->md_info.md_state.motion_detect_event_type 
	  	 != SNS_MOTION_DETECT_EVENT_TYPE_ENABLED)
	  {
        fifo_odr_change = true;
      }
    }
    lsm6ds3_set_fifo_config(state,
                            desired_wmk,
                            accel_chosen_sample_rate_reg_value,
                            gyro_chosen_sample_rate_reg_value,
                            state->fifo_info.fifo_enabled);

    // 2 sample periods of flush period buffer
    flush_period_buffer_usec = (2000000/accel_chosen_sample_rate);
    state->fifo_info.max_requested_flush_ticks = payload->desired_flush_ticks +
       sns_convert_ns_to_ticks(flush_period_buffer_usec * 1000);

    if(LSM6DS3_CONFIG_IDLE == state->config_step &&
       lsm6ds3_dae_if_stop_streaming(this))
    {
      state->config_step = LSM6DS3_CONFIG_STOPPING_STREAM;
    }
	// If there is only Temp ODR change
	if(false == fifo_odr_change)
	{
	  if(true == temp_odr_change)
	  {
	    lsm6ds3_set_polling_config(this);
		lsm6ds3_send_config_event(this);
	  }
	}
    if((LSM6DS3_CONFIG_IDLE == state->config_step) && (true == fifo_odr_change))
    {
      state->fifo_info.wmk_change = false;
      fifo_odr_change = false;
      LSM6DS3_INST_PRINTF(HIGH, this, "choosen_wmk %u,choosen_sample_rate %d",
	    state->fifo_info.cur_wmk, (uint32_t)state->accel_info.curr_odr);
      lsm6ds3_reconfig_hw(this);      
    }
	
    LSM6DS3_INST_PRINTF(LOW, this, "accel odr %u,running odr %u,fifo running odr:%u",
	  accel_chosen_sample_rate_reg_value,state->accel_info.curr_odr,
	  state->fifo_info.configured_odr);
    // update registry configuration
    if(LSM6DS3_ACCEL == payload->registry_cfg.sensor_type
       && payload->registry_cfg.version >= state->accel_registry_cfg.version)
    {
      fac_cal_bias = state->accel_registry_cfg.fac_cal_bias;
      fac_cal_corr_mat = &state->accel_registry_cfg.fac_cal_corr_mat;
      state->accel_registry_cfg.version = payload->registry_cfg.version;
    }
    else if(LSM6DS3_GYRO == payload->registry_cfg.sensor_type
            && payload->registry_cfg.version >= state->gyro_registry_cfg.version)
    {
      fac_cal_bias = state->gyro_registry_cfg.fac_cal_bias;
      fac_cal_corr_mat = &state->gyro_registry_cfg.fac_cal_corr_mat;
      state->gyro_registry_cfg.version = payload->registry_cfg.version;
    }
    else if(LSM6DS3_SENSOR_TEMP == payload->registry_cfg.sensor_type
            && payload->registry_cfg.version >= state->sensor_temp_registry_cfg.version)
    {
      fac_cal_bias = state->sensor_temp_registry_cfg.fac_cal_bias;
      fac_cal_corr_mat = &state->sensor_temp_registry_cfg.fac_cal_corr_mat;
      state->sensor_temp_registry_cfg.version = payload->registry_cfg.version;
    }

    if(NULL!= fac_cal_bias && NULL != fac_cal_corr_mat)
    {
      sns_memscpy(fac_cal_bias, sizeof(payload->registry_cfg.fac_cal_bias),
                  payload->registry_cfg.fac_cal_bias,
                  sizeof(payload->registry_cfg.fac_cal_bias));

      sns_memscpy(fac_cal_corr_mat, sizeof(payload->registry_cfg.fac_cal_corr_mat),
                  &payload->registry_cfg.fac_cal_corr_mat,
                  sizeof(payload->registry_cfg.fac_cal_corr_mat));
    }
  }
  else if(client_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ)
  {
    state->fifo_flush_in_progress = true;
    LSM6DS3_INST_PRINTF(MED, this, "flush req is in progress");
    if(!lsm6ds3_dae_if_flush_samples(this))
    {
      state->fifo_flush_client_present = true;
      lsm6ds3_handle_fifo_data(this,sns_get_system_time());
    }
  }
  else if(client_request->message_id ==
          SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
  {
    /** All self-tests can be handled in normal mode. */
    state->island_service->api->sensor_instance_island_exit(state->island_service, this);
    lsm6ds3_run_self_test(this);
    state->new_self_test_request = false;
  }

  // Turn COM port OFF
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                    false);

  return SNS_RC_SUCCESS;
}

/** Public Data Definitions. */

sns_sensor_instance_api lsm6ds3_sensor_instance_api =
{
  .struct_len        = sizeof(sns_sensor_instance_api),
  .init              = &lsm6ds3_inst_init,
  .deinit            = &lsm6ds3_inst_deinit,
  .set_client_config = &lsm6ds3_inst_set_client_config,
  .notify_event      = &lsm6ds3_inst_notify_event
};

