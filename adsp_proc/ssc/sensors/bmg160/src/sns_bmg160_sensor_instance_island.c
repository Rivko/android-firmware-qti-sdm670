/*******************************************************************************
 * Copyright (c) 2018-2019, Bosch Sensortec GmbH
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
#include "sns_mem_util.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_time.h"
#include "sns_types.h"

#include "sns_bmg160_hal.h"
#include "sns_bmg160_sensor.h"
#include "sns_bmg160_sensor_instance.h"

#include "sns_async_com_port.pb.h"
#include "sns_interrupt.pb.h"
#include "sns_timer.pb.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#include "sns_pb_util.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_sync_com_port_service.h"
#include "sns_printf.h"
#include "sns_event_service.h"



#if BMG160_CONFIG_ENABLE_SELF_TEST_FAC
#include "sns_cal.pb.h"
#endif

const  bst_odr_conf_pair bmg160_config_map[] = {
   {
    .odr   = BMG160_ODR_100HZ,
    .regv  = BMG160_ODR_100HZ_DECF20_FB12_REGV,
    .f_odr = 100.0,
   },

   {
    .odr   = BMG160_ODR_200HZ,
    .regv  = BMG160_ODR_200HZ_DECF10_FB23_REGV,
    .f_odr = 200.0,
   },

   {
    .odr   = BMG160_ODR_400HZ,
    .regv  = BMG160_ODR_400HZ_REGV,
    .f_odr = 400.0,
   },

   {
    .odr   = BMG160_ODR_1000HZ,
    .regv  = BMG160_ODR_1000HZ_REGV,
    .f_odr = 1000.0,
   },

   {
    .odr   = BMG160_ODR_2000HZ,
    .regv  = BMG160_ODR_2000HZ_REGV,
    .f_odr = 2000.0,
   },
};




/**
 * find the matching odr
 * @param desired_sample_rate      desired sample rate in hz
 * @param chosen_sample_rate       chosen sample rate in hz
 * @param chosen_reg_value         the regv match the chosen sample rate
 * @param num_samples_to_discard   number of samples to discard
 * @return
 */
static sns_rc bmg160_gyro_match_odr(float desired_sample_rate,
                                      float *chosen_sample_rate,
                                      bmg160_gyro_odr_regv *chosen_reg_value,
                                      uint8_t *num_samples_to_discard
                                      )
{
  uint8_t idx;

  if((BMG160_MAX_ODR < desired_sample_rate)
     ||
     NULL == chosen_sample_rate
     ||
     NULL == chosen_reg_value) {
    return SNS_RC_NOT_SUPPORTED;
  }

  for(idx = 0; idx < ARR_SIZE(bmg160_config_map); idx++)
  {
    if((uint16_t)desired_sample_rate <= bmg160_config_map[idx].odr)
    {
      break;
    }
  }

  if (idx >= ARR_SIZE(bmg160_config_map))
  {
    *chosen_sample_rate = 0.0;
    *chosen_reg_value = BMG160_GYRO_ODR_OFF;
    return SNS_RC_NOT_SUPPORTED;
  }

  *chosen_sample_rate = bmg160_config_map[idx].f_odr;
  *chosen_reg_value = bmg160_config_map[idx].regv;
  *num_samples_to_discard = 0;

  return SNS_RC_SUCCESS;
}

// update the acc fifo data ts

static void bmg160_reset_ts_base(sns_sensor_instance * const inst)
{
  bmg160_instance_state *istate = (bmg160_instance_state*)inst->state->state;
  istate->fifo_info.avail_accurate_ts_last_sample = false;
  istate->fifo_info.ff_isr_cnt = 0;
}



#if BMG160_CONFIG_ENABLE_FIFO
/**
 * process the comport vectors (read/write data)
 * @param vector          comport vector handle
 * @param user_arg        user defined parameter
 */
static void bmg160_process_com_port_vector(sns_port_vector *vector, void *user_arg)
{
  sns_sensor_instance *instance = (sns_sensor_instance *)user_arg;
  bmg160_instance_state *state = (bmg160_instance_state *)instance->state->state;

#if BMG160_ENABLE_LOG_LEVEL_2
  BMG160_INST_PRINTF(MED, instance, "<vector enter> INT time@%u current time@%u",
      state->fifo_info.isr_ts, (uint32_t)sns_get_system_time());
#endif
  if(BMG160_REGA_FIFO_DATA_ADDR == vector->reg_addr)
  {
#if BMG160_CONFIG_ENABLE_FIFO
    sns_time ts_itvl = state->fifo_info.ts_fff_intvl_est;
    if (state->fifo_info.ts_fix_flag)
    {
      ts_itvl = state->fifo_info.ts_itvl_fast_fixed;
    }
#else
    state->fifo_info.timestamp = state->fifo_info.isr_ts;
#endif
    //Vector contains a FIFO buffer read
    bmg160_process_fifo_data_buffer(instance,
                                    state->fifo_info.timestamp,
                                    ts_itvl,
                                    vector->buffer,
                                    vector->bytes);
  }
#if BMG160_ENABLE_LOG_LEVEL_2
  BMG160_INST_PRINTF(MED, instance, "<vector leave> @%u", (uint32_t)sns_get_system_time());
#endif
}
#else

#endif


/**
 * sensor temperature odr validation api
 * @param state    the instance state handler
 * @return  SNS_RC_SUCCESS   on success
 *          others value     failure
 */
static sns_rc bmg160_validate_sensor_temp_odr(bmg160_instance_state *state)
{
  sns_rc rc = SNS_RC_SUCCESS;

  if (state->sensor_temp_info.sample_rate_req <= BMG160_SENSOR_TEMP_ODR_ZERO)
  {
    state->sensor_temp_info.sampling_rate_hz = BMG160_SENSOR_TEMP_ODR_ZERO / 2.0;
  }
  else if(state->sensor_temp_info.sample_rate_req <= BMG160_SENSOR_TEMP_ODR_1)
  {
    state->sensor_temp_info.sampling_rate_hz = BMG160_SENSOR_TEMP_ODR_1;
  }
  else if(state->sensor_temp_info.sample_rate_req > BMG160_SENSOR_TEMP_ODR_1
          &&
          state->sensor_temp_info.sampling_rate_hz <= BMG160_SENSOR_TEMP_ODR_5)
  {
    state->sensor_temp_info.sample_rate_req = BMG160_SENSOR_TEMP_ODR_5;
  }
  else
  {
    state->sensor_temp_info.sampling_intvl = 0;
    rc = SNS_RC_NOT_SUPPORTED;
  }

  if (rc == SNS_RC_SUCCESS)
  {
    if (state->sensor_temp_info.sampling_rate_hz > BMG160_SENSOR_TEMP_ODR_ZERO)
    {
      state->sensor_temp_info.sampling_intvl =
          sns_convert_ns_to_ticks(1000000000.0 / state->sensor_temp_info.sampling_rate_hz);
    }
    else
    {
      state->sensor_temp_info.sampling_intvl = 0;
    }
  }

  return rc;
}

static void bmg160_inst_handle_isr_event(sns_sensor_instance * const this)
{
  bmg160_instance_state *state =
         (bmg160_instance_state*)this->state->state;
  sns_interrupt_event irq_event = sns_interrupt_event_init_zero;
  sns_sensor_event *event;

  /* Handle interrupts*/
  if(NULL != state->interrupt_data_stream)
  {
    event = state->interrupt_data_stream->api->peek_input(state->interrupt_data_stream);
    while(NULL != event)
    {
      if (event->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT)
      {
        state->irq_info.irq_ready = true;

        if(state->fifo_info.publish_sensors & (BMG160_GYRO))
        {
          bmg160_enable_data_intr(state, state->fifo_info.fifo_enabled);
        }
        BMG160_INST_PRINTF(MED, this, "isr register event");
      }
      else if (event->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT)
      {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                     event->event_len);
        if(pb_decode(&stream, sns_interrupt_event_fields, &irq_event))
        {
#if BMG160_CONFIG_ENABLE_FIFO
          if(state->fifo_info.cur_wml > 0
             && state->fifo_info.fifo_rate < BMG160_GYRO_ODR_OFF)
#endif
          {
            bmg160_handle_interrupt_event(this, &irq_event,
                                          BMG160_FIFO_READ_CTX_TYPE_WMI);
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

}

#if BMG160_CONFIG_ENABLE_FIFO
static void bmg160_inst_handle_fifo_wml(sns_sensor_instance * const this)
{
  bmg160_instance_state *state =
         (bmg160_instance_state*)this->state->state;
  sns_sensor_event *event;

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
        //reset the reference
        bmg160_reset_ts_base(this);
      }
      else if(event->message_id == SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW)
      {
        pb_istream_t stream = pb_istream_from_buffer((uint8_t *)event->event, event->event_len);
        state->fifo_info.ascp_in_progress = false;

        sns_ascp_for_each_vector_do(&stream, bmg160_process_com_port_vector, (void *)this);

        // Check if still there is any FIFO data
        bmg160_handle_interrupt_event(this, NULL,
                                      BMG160_FIFO_READ_CTX_TYPE_WMI_RECHECK);

        if(state->fifo_info.ff_flush_in_proc)
        {
          state->fifo_info.ff_flush_in_proc = false;
          if(state->fifo_info.publish_sensors & BMG160_GYRO)
          {
            bmg160_send_fifo_flush_done(this, &state->gyro_info.suid);
          }
        }

      }
      event = state->async_com_port_data_stream->api->get_next_input(state->async_com_port_data_stream);
    }
  }
}
#endif

static void bmg160_inst_handle_ds_timer_event(sns_sensor_instance * const this)
{
  bmg160_instance_state *state =
         (bmg160_instance_state*)this->state->state;
  sns_sensor_event *event;
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
          if(state->fifo_info.publish_sensors & BMG160_SENSOR_TEMP
             &&
             state->sensor_temp_info.timer_is_active
             &&
             state->sensor_temp_info.sampling_intvl > 0)
          {
            bmg160_handle_sensor_temp_sample(this, timer_event.timeout_time);
          }
        }
      }
      else if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_REG_EVENT)
      {
        /** TODO: decode and qse timer_reg_event*/
        BMG160_INST_PRINTF(LOW, this, "TIMER_SENSOR_REG_EVENT");
      }
      else
      {
        BMG160_INST_PRINTF(MED, this, "unknown message_id %d", event->message_id);
      }
      event = state->timer_data_stream->api->get_next_input(state->timer_data_stream);
    }
  }

}


/** See sns_sensor_instance_api::notify_event */
static sns_rc bmg160_inst_notify_event(sns_sensor_instance *const this)
{
  bmg160_instance_state *state =
         (bmg160_instance_state*)this->state->state;

#if BMG160_ENABLE_LOG_LEVEL_1
  sns_time ts_s = sns_get_system_time();
  BMG160_INST_PRINTF(LOW, this, "<bmg160_iif_ inst_notify_event>");
#endif

  /* Turn COM port ON*/
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);

  // handle ISR
  bmg160_inst_handle_isr_event(this);

  // handle fifo wml data
#if BMG160_CONFIG_ENABLE_FIFO
  bmg160_inst_handle_fifo_wml(this);
#endif

  // Handle Timer events
  bmg160_inst_handle_ds_timer_event(this);

#if BMG160_CONFIG_ENABLE_SELF_TEST_FAC
  if(state->fac_test_in_progress)
  {
    /** All self-tests can be handled in normal mode. */
#if BMG160_CONFIG_ENABLE_ISLAND_MODE
    state->island_service->api->sensor_instance_island_exit(state->island_service, this);
#endif
    bmg160_process_fac_test(this);
  }
#endif

  // Turn COM port OFF
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                                           false);

#if BMG160_ENABLE_LOG_LEVEL_1
  sns_time ts_e = sns_get_system_time();
  BMG160_INST_PRINTF(LOW, this, "inst_iff_ event ts_elp:<%u, %u, %u, %u>",
        (uint32_t)(ts_s >> 32), (uint32_t)(ts_s),
        (uint32_t)(ts_e >> 32), (uint32_t)(ts_e));
#endif
  return SNS_RC_SUCCESS;
}



/**
 * Access all the requests
 * @param istate    instance state handler
 *
 * @return  none
 */
void bmg160_inst_assess_overall_req(
    sns_sensor_instance *const inst)
{
  if (NULL == inst) {
    return ;
  }
  bmg160_instance_state *istate = (bmg160_instance_state*)inst->state->state;
  sns_rc              rc;
  uint8_t num_samples_to_discard;
  bmg160_gyro_odr_regv regv_odr = BMG160_GYRO_ODR_OFF;
  uint16_t desired_wmk = 0;
  uint8_t hw_gyro_chg = 0;
  //bool hw_temp_chg = false;

  uint16_t us_filter_delay[] = {
      1250,
      2500,
      4000,
      9000,
      18000,
      35000,
      6000,
      11000,
  };

  //gyro_info
  if (istate->gyro_info.sample_rate_req > 0)
  {
    istate->gyro_info.sample_rate_req = SNS_MAX(istate->gyro_info.sample_rate_req, BMG160_MIN_ODR);
  }
  rc = bmg160_gyro_match_odr (
      istate->gyro_info.sample_rate_req, &istate->gyro_info.sample_rate_matched,
      &regv_odr, &num_samples_to_discard);
  if (rc != SNS_RC_SUCCESS)
  {
    sns_service_manager *mgr = inst->cb->get_service_manager(inst);
    sns_event_service *event_service = (sns_event_service*)mgr->get_service(mgr, SNS_EVENT_SERVICE);
    sns_sensor_event *event = event_service->api->alloc_event(event_service, inst, 0);
    BMG160_INST_PRINTF(ERROR, inst, "gyro ODR match error %d", rc);
    //Allocate and send immediately.
    if(NULL != event)
    {
      event->message_id = SNS_STD_MSGID_SNS_STD_ERROR_EVENT;
      event->event_len = 0;
      event->timestamp = sns_get_system_time();
      event_service->api->publish_event(event_service, inst, event, &istate->gyro_info.suid);
    }
    //return rv;
  }

#if BMG160_ENABLE_FILTER_OUT_FILTER_DELAY
  if (regv_odr != BMG160_GYRO_ODR_OFF)
  {
    istate->gyro_info.ticks_filter_delay =
        (uint32_t)(bmg160_convert_us2ticks(us_filter_delay[regv_odr]));
  }
  else
  {
    istate->gyro_info.ticks_filter_delay = 0;
  }
#else
  istate->gyro_info.ticks_filter_delay = 0;
#endif

  if (regv_odr != istate->gyro_info.curr_odr_regv)
  {
    //  hw changed, need flush
    // TODO2 need optimize
    if ((!istate->low_latency_req) &&
        (istate->gyro_info.curr_odr_regv != BMG160_GYRO_ODR_OFF))
    {
      bmg160_hal_fifo_drain(inst, true, BMG160_FIFO_FLUSH_TRIGGER_HW_CHANGE);
    }


    if (regv_odr != BMG160_GYRO_ODR_OFF)
    {
      if (regv_odr != istate->gyro_info.curr_odr_regv) {
        istate->gyro_info.curr_odr_regv = regv_odr;
        istate->fifo_info.ts_fff_intvl_ideal =
            sns_convert_ns_to_ticks(1e9 / istate->gyro_info.sample_rate_matched);
        istate->fifo_info.ts_fff_intvl_est = istate->fifo_info.ts_fff_intvl_ideal;
        istate->sample_cnt_to_skip_odrc = BMG160_CONFIG_SKIP_SAMPLE_CNT_ODRC + 1;
        bmg160_reset_ts_base(inst);
        hw_gyro_chg |= 0x01;
      }
    }
  }

  if (istate->gyro_info.report_rate_req > istate->gyro_info.sample_rate_req)
  {
    istate->gyro_info.report_rate_req = istate->gyro_info.sample_rate_req;
    BMG160_INST_PRINTF(ERROR, inst, "WARNING acc rr lt sr");
  }

  if (BST_IS_FLOAT_ZERO(istate->gyro_info.report_rate_req))
  {
    desired_wmk = 0;
  }
  else
  {
#if BMG160_CONFIG_ENABLE_FIFO
    //if (istate->gyro_info.sample_rate_matched > 110.0) //this is a test
    if (istate->gyro_info.sample_rate_matched > BMG160_HIGHEST_ODR)
    {
      // drdy mode
      // low latency request, water mark level is always is 1
       istate->low_latency_req = 1;
       desired_wmk = 1;
    }
    else
    {
      //fifo mode
      istate->low_latency_req = 0;
      desired_wmk = (uint16_t) ((istate->gyro_info.sample_rate_matched + 0.5) / istate->gyro_info.report_rate_req);
      if (istate->gyro_info.sample_rate_matched > BMG160_GYRO_ODR_100HZ)
      {
        desired_wmk = ((uint32_t) (istate->gyro_info.sample_rate_matched + 1.0) / 100) * desired_wmk;
      }
      if (desired_wmk > BMG160_MAX_FIFO_LEVEL)
      {
        desired_wmk = BMG160_MAX_FIFO_LEVEL;
      }
    }
    // should group these estimation reset
    // this is the estimation, in practice the 1st sample fired ts is much larger than this
    istate->fifo_info.ts_1st_sample_est = sns_get_system_time() +
                desired_wmk * istate->fifo_info.ts_fff_intvl_ideal;
#else
    desired_wmk = 1;
    istate->fifo_info.cur_wml = desired_wmk;
#endif
  }

  BMG160_INST_PRINTF(LOW, inst, "sr:%d, regv:%d, wml:%d, low latency flag:%d",
                      (uint32_t)istate->gyro_info.sample_rate_matched,
                      regv_odr,
                      desired_wmk,
                      istate->low_latency_req);

  // sensor temperature
  rc = bmg160_validate_sensor_temp_odr(istate);
  if(rc != SNS_RC_SUCCESS)
  {
    // TODO Unsupported rate. Report error using sns_std_error_event.
    BMG160_INST_PRINTF(ERROR, inst, "sensor_temp ODR match error %d", rc);
    //return rv;
  }
#if BMG160_CONFIG_ENABLE_FIFO
  // if fifo wml change ?
  if (desired_wmk != istate->fifo_info.cur_wml)
  {
    hw_gyro_chg |= 0x02;
  }
  bmg160_set_fifo_config(istate, desired_wmk, regv_odr,
                         istate->fifo_info.fifo_enabled);
#endif

  istate->gyro_info.hw_gyro_need_refcg = hw_gyro_chg;
}


/** See sns_sensor_instance_api::set_client_config */
sns_rc bmg160_inst_set_client_config(sns_sensor_instance *const this,
                                             sns_request const *client_request)
{
  bmg160_instance_state *state =
                  (bmg160_instance_state*)this->state->state;
  sns_bmg160_req *payload = (sns_bmg160_req*)client_request->request;
  float *fac_cal_bias = NULL;
  matrix3 *fac_cal_corr_mat = NULL;

  BMG160_INST_PRINTF(LOW, this, "<bmg160_iif_ set_client_config>");
#if BMG160_ENABLE_LOG_LEVEL_1
  BMG160_INST_PRINTF(LOW, this, "<bmg160_iif_ cl req msg id:%d>", client_request->message_id);
#endif

  /* Turn COM port ON*/
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);

  bmg160_hal_register_interrupt(this);


  if(client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
  {
    if (state->fifo_info.publish_sensors &&
        state->fifo_info.fifo_rate != BMG160_GYRO_ODR_OFF)
    {
      bmg160_send_config_event(this);
    }

    bmg160_inst_assess_overall_req(this);

    if(state->config_step == BMG160_CONFIG_IDLE)
    {
      BMG160_INST_PRINTF(MED, this, "hw recfg:%x", state->gyro_info.hw_gyro_need_refcg);
      if (state->gyro_info.hw_gyro_need_refcg)
      {
        // odr or wml changed, need re-config the hw
        bmg160_hal_reconfig_hw(this);
      }
      else
      {
        //dump again to cover unconfigure case
        BMG160_INST_PRINTF(LOW, this, "no need configure? just dump registers");
        bmg160_dump_reg(this);
      }
    }

    bmg160_send_config_event(this);

    // update registry configuration
    if(BMG160_GYRO == payload->registry_cfg.sensor_type
            && payload->registry_cfg.version >= state->gyro_registry_cfg.version)
    {
      fac_cal_bias = state->gyro_registry_cfg.fac_cal_bias;
      fac_cal_corr_mat = &state->gyro_registry_cfg.fac_cal_corr_mat;
      state->gyro_registry_cfg.version = payload->registry_cfg.version;
    }
    else if(BMG160_SENSOR_TEMP == payload->registry_cfg.sensor_type
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
    bool dae_flush_done = false;
    if (!dae_flush_done)
    {
#if BMG160_CONFIG_ENABLE_FIFO
      if (!state->fifo_info.ff_flush_in_proc)
      {
        if (state->low_latency_req)
        {
          // drdy mode
          bmg160_send_fifo_flush_done (this, &state->gyro_info.suid);
        }
        else
        {
#if BMG160_CONFIG_ENABLE_FLUSH_SYNC
        bmg160_hal_fifo_drain(this, true, BMG160_FIFO_FLUSH_TRIGGER_CLIENTS);
#else
        bmg160_hal_fifo_drain(this, false, BMG160_FIFO_FLUSH_TRIGGER_CLIENTS);
#endif
        }
      }
      else
      {
        BMG160_INST_PRINTF(MED, this, "ff flush in process:%d",
                           state->fifo_info.ff_flush_trigger);
      }
#else
      bmg160_send_fifo_flush_done (this, &state->gyro_info.suid);
#endif
    }
  }
  else if(client_request->message_id ==
          SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
  {
    /** All self-tests can be handled in normal mode. */
#if BMG160_CONFIG_ENABLE_ISLAND_MODE
    state->island_service->api->sensor_instance_island_exit(state->island_service, this);
#endif
    bmg160_run_self_test(this);
    state->new_self_test_request = false;
  }

  // Turn COM port OFF
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                    false);

  return SNS_RC_SUCCESS;
}

/** Public Data Definitions. */

sns_sensor_instance_api bmg160_sensor_instance_api =
{
  .struct_len        = sizeof(sns_sensor_instance_api),
  .init              = bmg160_inst_init,
  .deinit            = bmg160_inst_deinit,
  .set_client_config = bmg160_inst_set_client_config,
  .notify_event      = bmg160_inst_notify_event
};

