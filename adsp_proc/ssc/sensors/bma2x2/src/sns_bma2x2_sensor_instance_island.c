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
#include "sns_mem_util.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_time.h"
#include "sns_types.h"

#include "sns_bma2x2_hal.h"
#include "sns_bma2x2_sensor.h"
#include "sns_bma2x2_sensor_instance.h"

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

#if BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
#include "sns_island_service.h"
#endif

const bma2x2_odr_reg_map bma2x2_reg_map[BMA2X2_REG_MAP_TABLE_SIZE] =
{
  {
    .odr = BMA2X2_ODR_0,
    .u_odr = 0,
    .acc_bw_regv = BMA2X2_ACCEL_ODR_OFF,
    .discard_samples = 0
  },
  {
    .odr = (BMA2X2_ODR_16),
    .u_odr = 16,
    .acc_bw_regv = BMA2X2_REGV_BW_7_81HZ,
    .discard_samples = 2
  },
  {
    .odr = (BMA2X2_ODR_32),
    .u_odr = 32,
    .acc_bw_regv = BMA2X2_REGV_BW_15_63HZ,
    .discard_samples = 2
  },
  {
    .odr = (BMA2X2_ODR_63),
    .u_odr = 64,
    .acc_bw_regv = BMA2X2_REGV_BW_31_25HZ,
    .discard_samples = 2
  },
  {
    .odr = BMA2X2_ODR_125,
    .u_odr = 125,
    .acc_bw_regv = BMA2X2_REGV_BW_62_50HZ,
    .discard_samples = 3
  },
  {
    .odr = BMA2X2_ODR_250,
    .u_odr = 250,
    .acc_bw_regv = BMA2X2_REGV_BW_125HZ,
    .discard_samples = 4
  },
  {
    .odr = BMA2X2_ODR_500,
    .u_odr = 500,
    .acc_bw_regv = BMA2X2_REGV_BW_250HZ,
    .discard_samples = 2
  },
  {
    .odr = BMA2X2_ODR_1000,
    .u_odr = 1000,
    .acc_bw_regv = BMA2X2_REGV_BW_500HZ,
    .discard_samples = 2
  },
  {
    .odr = BMA2X2_ODR_2000,
    .u_odr = 2000,
    .acc_bw_regv = BMA2X2_REGV_BW_1000HZ,
    .discard_samples = 2
  },
};


/**
 * derive the resolution from the HW sensor version
 * @param instance
 */
void
bma2x2_set_settle_resolution(sns_sensor_instance *const instance)
{
  bma2x2_instance_state *state = (bma2x2_instance_state *) instance->state->state;
  bma2x2_accel_range_idx range_idx;
  uint32_t BMA2X2_LSB_RANGE_LIST[BMA2X2_ACCEL_RANGE_IDX_MAX] = {
      LSB_RANGE_14BITS, /*! BMA280 */
      LSB_RANGE_12BITS, /*! BMA255 */
      LSB_RANGE_10BITS, /*! BMA250E */
      LSB_RANGE_8BITS   /*! BMA222E */
  };

  range_idx = state->accel_info.acc_res_idx;

  INST_DEBUG(MED, instance, "range idx:%d, hw version:%d",
      range_idx, state->accel_info.hw_ver);

  if (range_idx < BMA2X2_ACCEL_RANGE_IDX_MAX)
  {
    state->accel_info.current_range_resolution =
        G * 4 * (1 << range_idx) / BMA2X2_LSB_RANGE_LIST[state->accel_info.hw_ver];
  }
}

/**
 * try to match the desired acc odr to match the sensor
 * @param desired_sample_rate
 * @param chosen_sample_rate
 * @param chosen_reg_value
 * @param num_samples_to_discard
 * @return
 */
sns_rc bma2x2_accel_match_odr(float desired_sample_rate,
                                      float *chosen_sample_rate,
                                      bma2x2_accel_odr_regv *chosen_reg_value,
                                      uint8_t *num_samples_to_discard
                                      )
{
  uint8_t idx;

  if((BMA2X2_MAX_ODR < desired_sample_rate)
     ||
     NULL == chosen_sample_rate
     ||
     NULL == chosen_reg_value)
  {
    return SNS_RC_NOT_SUPPORTED;
  }

  for(idx = 0; idx < ARR_SIZE(bma2x2_reg_map); idx++)
  {
    if((uint16_t)desired_sample_rate <= bma2x2_reg_map[idx].u_odr)
    {
      break;
    }
  }

  if (idx >= ARR_SIZE(bma2x2_reg_map))
  {
    return SNS_RC_NOT_SUPPORTED;
  }

  *chosen_sample_rate = bma2x2_reg_map[idx].odr;
  *chosen_reg_value = bma2x2_reg_map[idx].acc_bw_regv;
  *num_samples_to_discard = bma2x2_reg_map[idx].discard_samples;

  return SNS_RC_SUCCESS;
}


/**
 * validate the temperature sensor
 * @param state instance state handler
 * @return    SNS_RC_SUCCESS on success
 *            other value    any others case
 */
sns_rc bma2x2_validate_sensor_temp_odr(bma2x2_instance_state *state)
{
  sns_rc rc = SNS_RC_SUCCESS;

  if (state->sensor_temp_info.sampling_rate_hz <= BMA2X2_SENSOR_TEMP_ODR_ZERO)
  {
    state->sensor_temp_info.sampling_rate_hz = BMA2X2_SENSOR_TEMP_ODR_ZERO / 2.0;
  }
  else if(state->sensor_temp_info.sampling_rate_hz <= BMA2X2_SENSOR_TEMP_ODR_1)
  {
    state->sensor_temp_info.sampling_rate_hz = BMA2X2_SENSOR_TEMP_ODR_1;
  }
  else if(state->sensor_temp_info.sampling_rate_hz > BMA2X2_SENSOR_TEMP_ODR_1
          &&
          state->sensor_temp_info.sampling_rate_hz <= BMA2X2_SENSOR_TEMP_ODR_5)
  {
    state->sensor_temp_info.sampling_rate_hz = BMA2X2_SENSOR_TEMP_ODR_5;
  }
  else
  {
    state->sensor_temp_info.sampling_intvl = 0;
    rc = SNS_RC_NOT_SUPPORTED;
  }

  if (rc == SNS_RC_SUCCESS)
  {
    if (state->sensor_temp_info.sampling_rate_hz > BMA2X2_SENSOR_TEMP_ODR_ZERO)
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

static void bma2x2_inst_update_registry(sns_sensor_instance * const this,
    sns_bma2x2_req *payload)
{
  bma2x2_instance_state *state =
         (bma2x2_instance_state*)this->state->state;
  float *fac_cal_bias = NULL;
  matrix3 *fac_cal_corr_mat = NULL;

  if(BMA2X2_ACCEL == payload->registry_cfg.sensor_type
     && payload->registry_cfg.version >= state->accel_registry_cfg.version)
  {
    fac_cal_bias = state->accel_registry_cfg.fac_cal_bias;
    fac_cal_corr_mat = &state->accel_registry_cfg.fac_cal_corr_mat;
    state->accel_registry_cfg.version = payload->registry_cfg.version;
  }
  else if(BMA2X2_SENSOR_TEMP == payload->registry_cfg.sensor_type
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


void bma2x2_inst_assess_overall_req(sns_sensor_instance *const inst)
{
  bma2x2_instance_state *state = (bma2x2_instance_state*)inst->state->state;
  float accel_chosen_sample_rate = 0.0;
  bma2x2_accel_odr_regv accel_chosen_sample_rate_reg_value = BMA2X2_ACCEL_ODR_OFF;
  uint16_t desired_wmk = 0;
  sns_rc rv = SNS_RC_SUCCESS;
  uint8_t num_samples_to_discard;
  uint16_t idx_filter_dely = 0;
  uint8_t hw_chg = 0;

  INST_DEBUG(LOW, inst, "inst# acc: req_sr:%d, req_rr:%d, res:%d",
              (uint16_t)(state->accel_info.sample_rate_req),
              (uint16_t)(state->accel_info.report_rate_req),
              state->accel_info.acc_res_idx);

  if(state->fifo_info.fifo_enabled & BMA2X2_ACCEL)
  {
    bma2x2_set_settle_resolution(inst);
  }
  rv = bma2x2_accel_match_odr(state->accel_info.sample_rate_req,
                                &accel_chosen_sample_rate,
                                 &accel_chosen_sample_rate_reg_value,
                                 &num_samples_to_discard
                                 );
  if(rv != SNS_RC_SUCCESS)
  {
    // TODO Unsupported rate. Report error using sns_std_error_event.
    SNS_INST_PRINTF(ERROR, inst, "accel ODR match error %d", rv);
    //return rv;
  }
#if BMA2X2_ENABLE_FILTER_OUT_FILTER_DELAY
  const uint16_t us_filter_delay[] = {
      64000,
      32000,
      16000,
      8000,
      4000,
      2000,
      1000,
      500,
  };

  if (accel_chosen_sample_rate_reg_value >= BMA2X2_REGV_BW_7_81HZ)
  {
    idx_filter_dely =
        (uint16_t) (((uint16_t)accel_chosen_sample_rate_reg_value - BMA2X2_REGV_BW_7_81HZ));
  }

  state->accel_info.ticks_filter_delay =
        (uint32_t) (bma2x2_convert_us2ticks(us_filter_delay[idx_filter_dely]));
#else
  state->accel_info.ticks_filter_delay = 0;
#endif

  {
    rv = bma2x2_validate_sensor_temp_odr(state);
    if(rv != SNS_RC_SUCCESS)
    {
      // TODO Unsupported rate. Report error using sns_std_error_event.
      SNS_INST_PRINTF(ERROR, inst, "sensor_temp ODR match error %d", rv);
      //return rv;
    }
  }

#if BMA2X2_CONFIG_ENABLE_FIFO
  if(state->accel_info.curr_odr_regv != accel_chosen_sample_rate_reg_value)
  {
    state->accel_info.num_samples_to_discard = num_samples_to_discard;
    state->fifo_info.sample_cnt_to_skip_odrc = BMA2X2_CONFIG_SKIP_SAMPLE_CNT_ODRC + 1;
    // hw odr checking
    hw_chg |= 0x01;
  }
  else
  {
    state->accel_info.num_samples_to_discard = 0;
  }

  // TODO2 hw fifo flush needed
  if (hw_chg && (state->accel_info.curr_odr_regv != BMA2X2_ACCEL_ODR_OFF))
  {
    if (!state->low_latency_req)
    {
      // fifo flush before do a new configuration
      bma2x2_hal_fifo_drain(inst, true, BMA2X2_FIFO_FLUSH_TRIGGER_HW_CHANGE);
    }
  }

  // XXX I have already did a test to force the working mode as low latency for 1hour and found that
  // the driver worked fine
  if (accel_chosen_sample_rate > BMA2X2_REGV_BW_250HZ)
  {
      state->low_latency_req = 1;
      desired_wmk = 1;
  }
  else
  {
      state->low_latency_req = 0;
      if(state->accel_info.report_rate_req != 0)
      {
        desired_wmk = (uint16_t)((accel_chosen_sample_rate + 0.5) / state->accel_info.report_rate_req);
        desired_wmk = BST_MAX(desired_wmk, 1);
      }
      else
      {
        desired_wmk = 0;
      }
      // the system can't handle 250hz data rate so,
      // put the odr 250hz with fifo wml = 2 to underclocking the INT frequency
      if ((accel_chosen_sample_rate + 0.5) > BMA2X2_MAX_SF_FIFO_ODR)
      {
        desired_wmk *= (uint16_t)((accel_chosen_sample_rate + 0.5) / BMA2X2_MAX_SF_FIFO_ODR);
      }

      if(BMA2X2_MAX_FIFO <= desired_wmk)
      {
        desired_wmk = BMA2X2_MAX_FIFO;
      }
  }

#else
  desired_wmk = 1;
#endif

  if (state->fifo_info.fifo_enabled & BMA2X2_ACCEL)
  {
    sns_time ts_curr = sns_get_system_time();
    state->fifo_info.ts_ff_end = ts_curr;

    state->fifo_info.ts_itvl_ideal = sns_convert_ns_to_ticks(1e9 / accel_chosen_sample_rate);
    state->fifo_info.ts_itvl_est   = state->fifo_info.ts_itvl_ideal;
    // actually the first frame will come later from the est after the configuration
    state->accel_info.ff_wml = desired_wmk;

#if BMA2X2_CONFIG_ENABLE_HEARTBEAT_TIMER
    //heart beat
    state->heart_beat_timeout =
      sns_convert_ns_to_ticks(1e9 /accel_chosen_sample_rate) * BMA2X2_HEART_BEAT_ODR_COUNT * desired_wmk ;
#endif

    INST_DEBUG(MED, inst, "desired wml:%d sample r:%d ts itvl ideal:%u",
        state->accel_info.ff_wml,
        (uint32_t) accel_chosen_sample_rate,
        (uint32_t) state->fifo_info.ts_itvl_ideal);

  }

  if(state->md_info.enable_md_int)
  {
    bma2x2_hal_set_gated_accel_config(state,
                                   desired_wmk,
                                   accel_chosen_sample_rate_reg_value,
                                   state->fifo_info.fifo_enabled);
    if(state->md_info.md_state.motion_detect_event_type
        != SNS_MOTION_DETECT_EVENT_TYPE_ENABLED)
    {
      hw_chg |= 0x02;
    }
  }

  // hw wml checking
  if (desired_wmk != state->fifo_info.cur_wml)
  {
    hw_chg |= 0x04;
  }

  bma2x2_hal_set_fifo_config(state,
                             desired_wmk,
                             accel_chosen_sample_rate_reg_value,
                             state->fifo_info.fifo_enabled);

  state->accel_info.hw_acc_need_recfg |= hw_chg;
  // 2 sample periods of flush period buffer

#if BMA2X2_CONFIG_ENABLE_DAE
  if(BMA2X2_CONFIG_IDLE == state->config_step
      &&
     bma2x2_dae_if_stop_streaming(this)
  )
  {
    state->config_step = BMA2X2_CONFIG_STOPPING_STREAM;
  }
#endif

}



#if BMA2X2_CONFIG_ENABLE_FIFO
/**
 * process process the data vector
 * @param vector            comport vector
 * @param user_arg          user defined pointer
 */
static void bma2x2_process_vector_event(sns_port_vector *vector, void *user_arg)
{
  sns_sensor_instance *instance = (sns_sensor_instance *)user_arg;

  if(BMA2X2_REGA_FIFO_DATA_OUTPUT_REG == vector->reg_addr)
  {
    //Vector contains a FIFO buffer read
    bma2x2_instance_state *state = (bma2x2_instance_state *)instance->state->state;

    if(vector->bytes >= 1 &&
        state->fifo_info.sampling_intvl > 0)
    {
#if BMA2X2_CONFIG_ENABLE_FIFO
      sns_time ts_itvl = state->fifo_info.ts_itvl_est;
      if (state->fifo_info.ff_itvl_need_fast_fix)
      {
        //fast fix triggered
        ts_itvl = state->fifo_info.ts_itvl_fast_fixed;
      }
#else
      state->fifo_info.ts_ff_start = state->fifo_info.ts_irq;
#endif
      bma2x2_hal_process_fifo_data_buffer(instance,
                                       state->fifo_info.ts_ff_start,
                                       ts_itvl,
                                       vector->buffer,
                                       vector->bytes);
    }
  }
}
#endif

static void bma2x2_handle_isr_event(sns_sensor_instance * this)
{
  bma2x2_instance_state *state =
         (bma2x2_instance_state*)this->state->state;
  sns_sensor_event *event;
  sns_interrupt_event irq_event = sns_interrupt_event_init_zero;

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
          bma2x2_hal_update_md_intr(this, true, false);
        }
        if(state->fifo_info.publish_sensors & (BMA2X2_ACCEL))
        {
          bma2x2_enable_fifo_intr(state, state->fifo_info.fifo_enabled);
        }
      }
      else if (event->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT)
      {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                     event->event_len);
        if(pb_decode(&stream, sns_interrupt_event_fields, &irq_event))
        {
          if(state->md_info.enable_md_int)
          {
            /**
             * 1. Handle MD interrupt: Send MD fired event to client.
             * 2. Disable MD.
             * 3. Start Gated Accel FIFO stream with desired config.
             */
            bma2x2_hal_handle_md_interrupt(this, irq_event.timestamp);
            if(state->md_info.md_state.motion_detect_event_type
               == SNS_MOTION_DETECT_EVENT_TYPE_FIRED)
            {
              state->md_info.enable_md_int = false;
              bma2x2_hal_update_md_intr(this, false, false);
              bma2x2_hal_set_md_config(this, false);
              state->md_info.md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_FIRED;
              if(state->accel_info.gated_client_present)
              {
                bma2x2_hal_set_fifo_config(state,
                                        state->md_info.desired_wmk,
                                        state->md_info.desired_odr,
                                        state->fifo_info.fifo_enabled);
                if(!(state->fifo_info.publish_sensors & (BMA2X2_ACCEL)))
                {
#if BMA2X2_CONFIG_ENABLE_FIFO
                  bma2x2_stop_fifo_streaming(state);
                  bma2x2_start_fifo_streaming(state);
#endif
                  bma2x2_enable_fifo_intr(state, state->fifo_info.fifo_enabled);
                  bma2x2_hal_send_config_event(this);
#if BMA2X2_CONFIG_ENABLE_DAE
                  bma2x2_dae_if_start_streaming(this);
#endif
                }
              }
            }
          }

#if BMA2X2_CONFIG_ENABLE_FIFO
          if(state->fifo_info.cur_wml > 0
             && state->fifo_info.fifo_rate > BMA2X2_ACCEL_ODR_OFF)
#endif
          {
            bma2x2_hal_handle_fifo_event(this, &irq_event,
                                         BMA2X2_FIFO_READ_CTX_TYPE_WMI);
          }
        }
      }
      else
      {
        INST_DEBUG(ERROR, this, "Received invalid event id=%d", event->message_id);
      }
      event = state->interrupt_data_stream->api->get_next_input(state->interrupt_data_stream);
    }
  }
}

#if  BMA2X2_CONFIG_ENABLE_FIFO
static void bma2x2_handle_fifo_wml_isr(sns_sensor_instance * const this)
{
  bma2x2_instance_state *state =
         (bma2x2_instance_state*)this->state->state;
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
      }
      else if(event->message_id == SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW)
      {
        pb_istream_t stream = pb_istream_from_buffer((uint8_t *)event->event, event->event_len);
        // clear async in process
        state->fifo_info.ff_ascp_in_progress = false;

        sns_ascp_for_each_vector_do(&stream, bma2x2_process_vector_event, (void *)this);
        // Check if still there is any FIFO data
        bma2x2_hal_handle_fifo_event(this, NULL,
            BMA2X2_FIFO_READ_CTX_TYPE_WMI_RECHECK);

        if(state->fifo_info.ff_flush_in_proc)
        {
          // flush flag
          state->fifo_info.ff_flush_in_proc = false;
          if(state->fifo_info.publish_sensors & BMA2X2_ACCEL)
          {
            bma2x2_hal_send_fifo_flush_done(this, &state->accel_info.suid);
          }
        }

        if(state->fac_test_in_progress)
        {
#if BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
          /** All self-tests can be handled in normal mode. */
          state->island_service->api->sensor_instance_island_exit(state->island_service, this);
#endif
          bma2x2_process_fac_test(this);
        }
      }
      event = state->async_com_port_data_stream->api->get_next_input(state->async_com_port_data_stream);
    }
  }
}
#endif


static void bma2x2_handle_timer_event(sns_sensor_instance * const this)
{
  bma2x2_instance_state *state =
         (bma2x2_instance_state*)this->state->state;
  sns_sensor_event *event;

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
          if((state->fifo_info.publish_sensors & BMA2X2_SENSOR_TEMP)
             &&
             state->sensor_temp_info.timer_is_active
             &&
             state->sensor_temp_info.sampling_intvl > 0)
          {
            bma2x2_hal_handle_sensor_temp_sample(this);
          }
        }
      }
      else if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_REG_EVENT)
      {
        /** TODO: decode and qse timer_reg_event*/
        INST_DEBUG(LOW, this, "TIMER_SENSOR_REG_EVENT");
      }
      else
      {
        INST_DEBUG(LOW, this, "unknown message_id %d", event->message_id);
      }
      event = state->timer_data_stream->api->get_next_input(state->timer_data_stream);
    }
  }
}

#if BMA2X2_CONFIG_ENABLE_HEARTBEAT_TIMER
static void bma2x2_inst_handle_heart_beat_timer_event(sns_sensor_instance *const inst)
{
  bma2x2_instance_state * state = (bma2x2_instance_state *) inst->state->state;
  sns_sensor_event *event;

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
            INST_DEBUG(LOW, inst, "Heart beat timer fired");
            if(!state->fifo_info.ff_ascp_in_progress)
              bma2x2_hal_handle_fifo_event(inst,
                     NULL, BMA2X2_FIFO_READ_CTX_TYPE_HEARTBEAT);
          }
          else
          {
            INST_DEBUG(LOW, inst, "Heart beat timer message_id:%u", event->message_id);
          }
        }
      event = state->timer_heart_beat_data_stream->api->get_next_input(state->timer_heart_beat_data_stream);
      }
    }

}
#endif

/**
 * notify event on the instance
 * @param this   instance handler
 * @return       SNS_RC_SUCCESS on success
 *               others value   any others case
 */
static sns_rc bma2x2_inst_notify_event(sns_sensor_instance *const this)
{
  bma2x2_instance_state *state =
         (bma2x2_instance_state*)this->state->state;

#if BMA2X2_CONFIG_ENABLE_LOG_LEVEL_1
  sns_time ts_s = sns_get_system_time();
  INST_DEBUG(LOW, this, "inst_iif_ evt s:<%u,%u>", (uint32_t)(ts_s >> 32),
                                              (uint32_t)ts_s);
#endif

#if BMA2X2_CONFIG_ENABLE_DAE
  bma2x2_dae_if_process_events(this);
#endif

  // Turn COM port ON
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                                           true);
  // handle the irq
  bma2x2_handle_isr_event(this);

#if BMA2X2_CONFIG_ENABLE_FIFO
  // handle async event
  bma2x2_handle_fifo_wml_isr(this);
#endif

  // <timer>
  bma2x2_handle_timer_event(this);

  // heart beat timer
#if BMA2X2_CONFIG_ENABLE_HEARTBEAT_TIMER
  bma2x2_inst_handle_heart_beat_timer_event(this);
#endif

#if !BMA2X2_ENABLE_FORCE_WAIT
  // handle register configuration timer
  bma2x2_hal_handle_reg_cfg_timer_event(this);
#endif

  // Turn COM port OFF
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                                           false);

#if BMA2X2_CONFIG_ENABLE_LOG_LEVEL_1
  sns_time ts_e = sns_get_system_time();
  INST_DEBUG(LOW, this, "inst_iif_ evt e:<%u,%u>", (uint32_t)(ts_e >> 32),
                                              (uint32_t)ts_e);
#endif
  return SNS_RC_SUCCESS;
}



/** See sns_sensor_instance_api::set_client_config */
sns_rc bma2x2_inst_set_client_config(sns_sensor_instance *const this,
                                     sns_request const *client_request)
{
  bma2x2_instance_state *state = (bma2x2_instance_state*)this->state->state;
  sns_bma2x2_req *payload = (sns_bma2x2_req*)client_request->request;
  bma2x2_sensor_type sensor_type;

  INST_DEBUG(LOW, this, "<sns_see_iif_> instance set_client_config");

  /* Turn COM port ON*/
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);

#if BMA2X2_CONFIG_ENABLE_DAE
  if(!bma2x2_dae_if_available(this))
  {
    bma2x2_hal_register_interrupt(this);
  }
#else
  bma2x2_hal_register_interrupt(this);
#endif
  if(client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
  {
    if(state->fifo_info.publish_sensors != 0 && state->fifo_info.fifo_rate != 0)
    {
      bma2x2_hal_send_config_event(this); // sends current configuration
    }

    sensor_type               = payload->registry_cfg.sensor_type;

    INST_DEBUG(LOW, this, "icfg @sensor:%d", sensor_type);

    // access all the request
    bma2x2_inst_assess_overall_req(this);


#if BMA2X2_CONFIG_ENABLE_DAE
    if(BMA2X2_CONFIG_IDLE == state->config_step
        &&
       bma2x2_dae_if_stop_streaming(this)
    )
    {
      state->config_step = BMA2X2_CONFIG_STOPPING_STREAM;
    }
#endif

    if(state->config_step == BMA2X2_CONFIG_IDLE)
    {
#if BMA2X2_CONFIG_ENABLE_HEARTBEAT_TIMER
      state->heart_beat_timestamp = sns_get_system_time();
#endif
      INST_DEBUG(MED, this, "hw recfg:0x%02x", (state->accel_info.hw_acc_need_recfg));
      if (state->accel_info.hw_acc_need_recfg)
      {
        bma2x2_hal_reconfig_hw(this);
        // consume
        state->accel_info.hw_acc_need_recfg = 0;
        // rebase
        bma2x2_reset_ts_base(state);
      }
    }

    //dump registers
    bma2x2_hal_dump_reg(this);

    bma2x2_hal_send_config_event(this);

    // update registry configuration
    bma2x2_inst_update_registry(this, payload);

  }
  else if(client_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ)
  {
    bool dae_flush_done = false;
#if BMA2X2_CONFIG_ENABLE_DAE
    dae_flush_done = bmg160_dae_if_flush_samples(this);
#endif
    if (!dae_flush_done)
    {
#if BMA2X2_CONFIG_ENABLE_FIFO
      if (state->low_latency_req)
      {
        // data ready
        state->fifo_info.ff_flush_in_proc = false;
        bma2x2_hal_send_fifo_flush_done(this, &state->accel_info.suid);
      }
      else
      {
        if (!state->fifo_info.ff_flush_in_proc)
        {
#if BMA2X2_CONFIG_ENABLE_FLUSH_SYNC_READ
          bma2x2_hal_fifo_drain(this, true, BMA2X2_FIFO_FLUSH_TRIGGER_CLIENTS);
#else
          bma2x2_hal_fifo_drain(this, false, BMA2X2_FIFO_FLUSH_TRIGGER_CLIENTS);
#endif
        }
        else
        {
          INST_DEBUG(HIGH, this, "fifo_flush already in proc: %d", state->fifo_info.ff_flush_trigger);
        }
      }
#else
      state->fifo_info.ff_flush_in_proc = false;
      bma2x2_hal_send_fifo_flush_done(this, &state->accel_info.suid);
#endif
    }
  }
  else if(client_request->message_id ==
          SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
  {
#if BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
    /** All self-tests can be handled in normal mode. */
    state->island_service->api->sensor_instance_island_exit(state->island_service, this);
#endif
    bma2x2_run_self_test(this);
    state->new_self_test_request = false;
  }

  // Turn COM port OFF
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);

  return SNS_RC_SUCCESS;
}


sns_sensor_instance_api bma2x2_sensor_instance_api =
{
  .struct_len            = sizeof(sns_sensor_instance_api),
  .init                  = &bma2x2_inst_init,
  .deinit                = &bma2x2_inst_deinit,
  .set_client_config     = &bma2x2_inst_set_client_config,
  .notify_event          = &bma2x2_inst_notify_event
};

