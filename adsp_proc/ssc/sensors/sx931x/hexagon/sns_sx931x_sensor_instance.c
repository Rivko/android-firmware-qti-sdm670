/*******************************************************************************
 * Copyright (c) 2017, Semtech
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Semtech nor the
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
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_time.h"
#include "sns_sensor_event.h"
#include "sns_types.h"

#include "sns_sx931x_hal.h"
#include "sns_sx931x_sensor.h"
#include "sns_sx931x_sensor_instance.h"

#include "sns_interrupt.pb.h"
#include "sns_async_com_port.pb.h"
#include "sns_timer.pb.h"


#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_diag_service.h"
#include "sns_diag.pb.h"
#include "sns_sensor_util.h"
#include "sns_sync_com_port_service.h"



void sx931x_start_sensor_sar_polling_timer(sns_sensor_instance *this)
{
    sx931x_instance_state *state = (sx931x_instance_state *)this->state->state;
    sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
    uint8_t buffer[50] = {0};
    sns_request timer_req =
    {
        .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
        .request    = buffer
    };
    sns_rc rc = SNS_RC_SUCCESS;

    SX931x_INST_PRINTF(LOW, this, "sx931x_start_sensor_sar_polling_timer");

    if(NULL == state->sar_timer_data_stream)
    {
        sns_service_manager *smgr = this->cb->get_service_manager(this);
        sns_stream_service *srtm_svc = (sns_stream_service *)smgr->get_service(smgr, SNS_STREAM_SERVICE);

        rc = srtm_svc->api->create_sensor_instance_stream(srtm_svc,
                this, state->timer_suid, &state->sar_timer_data_stream);
    }

    if(SNS_RC_SUCCESS != rc || NULL == state->sar_timer_data_stream)
    {
        SNS_INST_PRINTF(ERROR, this, "failed timer stream create rc = %d", rc);
        return;
    }

    req_payload.is_periodic = true;
    req_payload.start_time = sns_get_system_time();
    req_payload.timeout_period = state->sar_info.sampling_intvl;
    timer_req.request_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
                            sns_timer_sensor_config_fields, NULL);
    if(timer_req.request_len > 0)
    {
        state->sar_timer_data_stream->api->send_request(state->sar_timer_data_stream, &timer_req);
        state->sar_info.timer_is_active = true;
    }
}



void sx931x_set_sar_polling_config(sns_sensor_instance *const this)
{
    sx931x_instance_state *state = (sx931x_instance_state *)this->state->state;

    SX931x_INST_PRINTF(LOW, this,
                    "sar_info.timer_is_active:%d state->sar_info.sampling_intvl:%u",
                    state->sar_info.timer_is_active,
                    state->sar_info.sampling_intvl);

    if(state->sar_info.sampling_intvl > 0)
    {
        if(!state->sar_info.timer_is_active)
        {
            sx931x_start_sensor_sar_polling_timer(this);
        }
    }
    else if(state->sar_info.timer_is_active)
    {
        state->sar_info.timer_is_active = false;
        sns_sensor_util_remove_sensor_instance_stream(this, &state->sar_timer_data_stream);
    }
}




void sx931x_reconfig_hw(sns_sensor_instance *this, sx931x_sensor_type sensor_type)
{
    sx931x_instance_state *state = (sx931x_instance_state *)this->state->state;
    uint8_t buffer;
    uint32_t xfer_bytes;

    SX931x_INST_PRINTF(LOW, this, "sx931x_reconfig_hw state->config_step = %d",
                    state->config_step);

    SX931x_INST_PRINTF(LOW, this, "enable sensor flag:0x%x publish sensor flag:0x%x",
                    state->deploy_info.enable,
                    state->deploy_info.publish_sensors);

   if(sensor_type == SX9XXX_SAR)
   {
      if(state->deploy_info.enable == 0)
      {      
          //reset chip and disable all phase 
          //enter into low power mode
          sx931x_device_sw_reset(state->scp_service,  state->com_port_info.port_handle);
          if(state->is_dri)
          {
              //disable all irq
              buffer = 0x00;
              state->com_write(state->scp_service, state->com_port_info.port_handle,
                              SX931x_IRQ_ENABLE_REG,
                              &buffer,
                              1,
                              &xfer_bytes,
                              false);
          }
          else
          {
              //update timer
              state->sar_info.sampling_intvl = 0;
              sx931x_set_sar_polling_config(this);
          }
      }
      else
      {
          //sx931x_reset_device(state->scp_service,  state->com_port_info.port_handle);
          if(state->is_dri)
          {
              //register interrupt
              SX931x_INST_PRINTF(LOW, this, "Dri register interrupt");
              if(state->irq_info.irq_ready)
              {
                  sx931x_interrupt_config(state->scp_service, 
                                        state->com_port_info.port_handle, 0x68);
              }
              else
              {
                  sx931x_register_interrupt(this);
              }
          }
          else
          {
              //start timer & enbale phase
              SX931x_INST_PRINTF(LOW, this, "start timer");
              sx931x_set_sar_polling_config(this);
          }
      }
   }
    
  state->config_step = CONFIG_IDLE; /* done with reconfig */
  sx931x_dump_reg(this,state->hw_idx);
}


static void sx931x_send_com_test_event(sns_sensor_instance *instance,
                                       sns_sensor_uid *uid, bool test_result)
{
    uint8_t data[1] = {0};
    pb_buffer_arg buff_arg = (pb_buffer_arg)
    {
        .buf = &data, .buf_len = sizeof(data)
    };
    sns_physical_sensor_test_event test_event = sns_physical_sensor_test_event_init_default;

    test_event.test_passed = test_result;
    test_event.test_type = SNS_PHYSICAL_SENSOR_TEST_TYPE_COM;
    test_event.test_data.funcs.encode = &pb_encode_string_cb;
    test_event.test_data.arg = &buff_arg;

    pb_send_event(instance,
                  sns_physical_sensor_test_event_fields,
                  &test_event,
                  sns_get_system_time(),
                  SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
                  uid);
}

void sx931x_run_self_test(sns_sensor_instance *instance)
{
    sx931x_instance_state *state = (sx931x_instance_state *)instance->state->state;
    sns_rc rv = SNS_RC_SUCCESS;
    uint8_t buffer = 0;
    bool who_am_i_success = false;

    rv = sx931x_get_who_am_i(state->scp_service,
                             state->com_port_info.port_handle,
                             &buffer);
    if(rv == SNS_RC_SUCCESS && buffer == SX931x_WHOAMI_VALUE)
    {
        who_am_i_success = true;
    }

    if(state->sar_info.test_info.test_client_present)
    {
        if(state->sar_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
        {
            sx931x_send_com_test_event(instance, &state->sar_info.suid, who_am_i_success);
        }
        else if(state->sar_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
        {
            // Handle factory test.
        }
        state->sar_info.test_info.test_client_present = false;
    }
}

void sx931x_send_config_event(sns_sensor_instance *const instance)
{
    sx931x_instance_state *state = (sx931x_instance_state *)instance->state->state;
    sns_std_sensor_physical_config_event phy_sensor_config =
        sns_std_sensor_physical_config_event_init_default;

    char operating_mode[] = "NORMAL";

    pb_buffer_arg op_mode_args;

    op_mode_args.buf = &operating_mode[0];
    op_mode_args.buf_len = sizeof(operating_mode);

    phy_sensor_config.has_sample_rate = true;
    phy_sensor_config.has_water_mark = false;
    phy_sensor_config.water_mark = 1;
    phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
    phy_sensor_config.operation_mode.arg = &op_mode_args;
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.has_resolution = true;
    phy_sensor_config.range_count = 2;
    phy_sensor_config.has_stream_is_synchronous = true;
    phy_sensor_config.stream_is_synchronous = false;

    if (state->deploy_info.publish_sensors & SX9XXX_SAR)
    {
        phy_sensor_config.sample_rate = state->sar_info.sampling_rate_hz;
        phy_sensor_config.has_active_current = true;
        phy_sensor_config.active_current = 720;
        phy_sensor_config.resolution = SX931x_SENSOR_SAR_RESOLUTION;
        phy_sensor_config.range_count = 2;
        phy_sensor_config.range[0] = SX931x_SENSOR_SAR_RANGE_MIN;
        phy_sensor_config.range[1] = SX931x_SENSOR_SAR_RANGE_MAX;

        pb_send_event(instance,
                      sns_std_sensor_physical_config_event_fields,
                      &phy_sensor_config,
                      sns_get_system_time(),
                      SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                      &state->sar_info.suid);
    }
}

static sns_rc sx931x_validate_sensor_sar_odr(sns_sensor_instance *this)
{
    sns_rc rc = SNS_RC_SUCCESS;
    sx931x_instance_state *state = (sx931x_instance_state *)this->state->state;

    SX931x_INST_PRINTF(LOW, this, "sar odr = %d", (int8_t)state->sar_info.sampling_rate_hz);
    if(state->sar_info.sampling_rate_hz > SMT_ODR_0
       && state->sar_info.sampling_rate_hz <= SMT_ODR_1)
    {
        state->sar_info.sampling_rate_hz = SMT_ODR_1;
    }
    else if(state->sar_info.sampling_rate_hz > SMT_ODR_1
            && state->sar_info.sampling_rate_hz <= SMT_ODR_5)
    {
        state->sar_info.sampling_rate_hz = SMT_ODR_5;
    }
    else if(state->sar_info.sampling_rate_hz > SMT_ODR_5
            && state->sar_info.sampling_rate_hz <= SMT_ODR_10)
    {
        state->sar_info.sampling_rate_hz = SMT_ODR_10;
    }
    else if(state->sar_info.sampling_rate_hz > SMT_ODR_10)
    {
        state->sar_info.sampling_rate_hz = SMT_ODR_25;
    }
    else
    {
        state->sar_info.sampling_intvl = 0;
        state->sar_info.timer_is_active = 0;
        SX931x_INST_PRINTF(LOW, this, "close sar sensor = %d, timer_is_active =%d",
                        (uint32_t)state->sar_info.sampling_rate_hz, state->sar_info.timer_is_active);
        rc = SNS_RC_NOT_SUPPORTED;
    }

    if (rc == SNS_RC_SUCCESS)
    {
        state->sar_info.sampling_intvl = sns_convert_ns_to_ticks(1000000000.0 / state->sar_info.sampling_rate_hz);
        SX931x_INST_PRINTF(LOW, this, "sar timer_value = %u", (uint32_t)state->sar_info.sampling_intvl);
    }

    return rc;
}



static void inst_cleanup(sx931x_instance_state *state, sns_stream_service *stream_mgr)
{
    sx931x_dae_if_deinit(state, stream_mgr);

    if(NULL != state->async_com_port_data_stream)
    {
        stream_mgr->api->remove_stream(stream_mgr, state->async_com_port_data_stream);
        state->async_com_port_data_stream = NULL;
    }
    if(NULL != state->sar_timer_data_stream)
    {
        stream_mgr->api->remove_stream(stream_mgr, state->sar_timer_data_stream);
        state->sar_timer_data_stream = NULL;
    }

    if(NULL != state->scp_service)
    {
        state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
        state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
        state->scp_service = NULL;
    }
}


/** See sns_sensor_instance_api::init */
sns_rc sns_see_sx931x_inst_init(sns_sensor_instance *const this,
                                sns_sensor_state const *sstate)
{
    sx931x_instance_state *state = (sx931x_instance_state *) this->state->state;
    sx931x_state *sensor_state = (sx931x_state *) sstate->state;
    float stream_data[4] = {0};
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_mgr = (sns_stream_service *)
                                     service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

    uint64_t buffer[10];
    pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));
    sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
    uint8_t arr_index = 0;
    float diag_temp[SX931x_NUM_AXES];
    pb_float_arr_arg arg = {.arr = (float *)diag_temp, .arr_len = SX931x_NUM_AXES,
                            .arr_index = &arr_index
                           };
    batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
    batch_sample.sample.arg = &arg;


    uint32_t hw_id = sensor_state->hw_idx;
    state->scp_service = (sns_sync_com_port_service *)
                         service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

    SX931x_INST_PRINTF(LOW, this, "<sns_see_if__  init> from sensor:0x%x", sensor_state->sensor);
    /************Setup stream connections with dependent Sensors*************************/
    stream_mgr->api->create_sensor_instance_stream(stream_mgr,
            this,
            sensor_state->acp_suid,
            &state->async_com_port_data_stream);
    stream_mgr->api->create_sensor_instance_stream(stream_mgr,
            this,
            sensor_state->irq_suid,
            &state->interrupt_data_stream);

    /**************Initialize COM port to be used by the Instance**************************/
    sns_memscpy(&state->com_port_info.com_config, sizeof(state->com_port_info.com_config),
                &sensor_state->com_port_info.com_config,
                sizeof(sensor_state->com_port_info.com_config));

    state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
            &state->com_port_info.port_handle);

    if(NULL == state->async_com_port_data_stream ||
       NULL == state->interrupt_data_stream ||
       NULL == state->com_port_info.port_handle)
    {
        inst_cleanup(state, stream_mgr);
        return SNS_RC_FAILED;
    }

    /**********************Copy all Sensor UIDs in instance state ************************/
    if(hw_id == 0)
    {
        sns_memscpy(&state->sar_info.suid,
                sizeof(state->sar_info.suid),
            &((sns_sensor_uid)SAR_SUID_0),
                sizeof(state->sar_info.suid));

        sns_memscpy(&state->timer_suid,
                sizeof(state->timer_suid),
                &sensor_state->timer_suid,
                sizeof(sensor_state->timer_suid));
    }
#if DUAL_SENSOR_SUPPORT
    else
    {
        sns_memscpy(&state->sar_info.suid,
            sizeof(state->sar_info.suid),
            &((sns_sensor_uid)SAR_SUID_1),
            sizeof(state->sar_info.suid));

        sns_memscpy(&state->timer_suid,
            sizeof(state->timer_suid),
            &sensor_state->timer_suid,
            sizeof(sensor_state->timer_suid));
    }
#endif
    state->hw_idx = hw_id;
    /*************************Init sar sensor **************************************/
    state->interface = sensor_state->com_port_info.com_config.bus_instance;
    state->op_mode = NORMAL_MODE;/*default working mode*/
    state->com_read = sx931x_com_read_wrapper;/* com read function*/
    state->com_write = sx931x_com_write_wrapper;/*com write function*/
    state->encoded_imu_event_len = pb_get_encoded_size_sensor_stream_event(stream_data, 4);

    state->diag_service =  (sns_diag_service *)
                           service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);
    state->scp_service =  (sns_sync_com_port_service *)
                          service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);
    state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);
    /*************************Initialize IRQ info to be used by the Instance****************/
    /*register interrupt*/
    state->is_dri = sensor_state->is_dri;
    state->irq_info.irq_config = sensor_state->irq_config;
    state->irq_info.irq_ready = false;
    /*****************************for test****************************************/ 
    sx931x_reset_device(state->scp_service, state->com_port_info.port_handle, state->hw_idx);
 
    /****************************end test****************************************/
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);

    /*********************** Configure the Async Com Port ****************************/
    {
        sns_data_stream *data_stream = state->async_com_port_data_stream;
        sns_com_port_config *com_config = &sensor_state->com_port_info.com_config;
        uint8_t pb_encode_buffer[100];
        sns_request async_com_port_request =
        {
            .message_id  = SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_CONFIG,
            .request     = &pb_encode_buffer
        };

        state->ascp_config.bus_type          = (com_config->bus_type == SNS_BUS_I2C) ?
                                               SNS_ASYNC_COM_PORT_BUS_TYPE_I2C : SNS_ASYNC_COM_PORT_BUS_TYPE_SPI;
        state->ascp_config.slave_control     = com_config->slave_control;
        state->ascp_config.reg_addr_type     = SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_8_BIT;
        state->ascp_config.min_bus_speed_kHz = com_config->min_bus_speed_KHz;
        state->ascp_config.max_bus_speed_kHz = com_config->max_bus_speed_KHz;
        state->ascp_config.bus_instance      = com_config->bus_instance;

        async_com_port_request.request_len = pb_encode_request(pb_encode_buffer,
                                             sizeof(pb_encode_buffer),
                                             &state->ascp_config,
                                             sns_async_com_port_config_fields,
                                             NULL);
        data_stream->api->send_request(data_stream, &async_com_port_request);
    }

    /******************Determine sizes of encoded logs *******************************/
    sns_diag_sensor_state_interrupt sensor_state_interrupt =
        sns_diag_sensor_state_interrupt_init_default;

    pb_get_encoded_size(&state->log_interrupt_encoded_size, sns_diag_sensor_state_interrupt_fields,
                        &sensor_state_interrupt);

    /***************************************************************************
    	* Determine size of sns_diag_sensor_state_raw as defined in
     *  sns_diag.proto
     *  sns_diag_sensor_state_raw is a repeated array of samples of
     *  type sns_diag_batch sample. The following determines the
     *  size of sns_diag_sensor_state_raw with a single batch
     *  sample
     ***************************************************************************/
    if(pb_encode_tag(&stream, PB_WT_STRING, sns_diag_sensor_state_raw_sample_tag))
    {
        if(pb_encode_delimited(&stream, sns_diag_batch_sample_fields, &batch_sample))
        {
            state->log_raw_encoded_size = stream.bytes_written;
        }
    }
    //sx931x_dae_if_init(this, stream_mgr, &sensor_state->dae_suid);
    SX931x_INST_PRINTF(LOW, this, "<sns_see_if__ init> success");
    return SNS_RC_SUCCESS;
}


sns_rc sns_see_sx931x_inst_deinit(sns_sensor_instance *const this)
{
    sx931x_instance_state *state = (sx931x_instance_state *) this->state->state;
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_mgr = (sns_stream_service *)
                                     service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    SX931x_INST_PRINTF(LOW, this, "<sns_see_sx931x_inst_deinit>");
    inst_cleanup(state, stream_mgr);
    return SNS_RC_SUCCESS;
}

 
/** See sns_sensor_instance_api::set_client_config */
sns_rc sns_see_sx931x_inst_set_client_config(sns_sensor_instance *const this,
                                             sns_request const *client_request)
{
  sx931x_instance_state *state = (sx931x_instance_state *) this->state->state;
  state->client_req_id = client_request->message_id;
  float desired_sample_rate = 0;
  float desired_report_rate = 0;
  sx931x_power_mode op_mode = INVALID_WORK_MODE;
  sx931x_sensor_type sensor_type = SX9XXX_SENSOR_INVALID;
  sns_sx931x_cfg_req *payload = (sns_sx931x_cfg_req *)client_request->request;
  sns_rc rv = SNS_RC_SUCCESS;

  SX931x_INST_PRINTF(LOW, this, "<sns_see_if__  set_client_config>");

  /* Turn COM port ON, *physical* */
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);
  if (client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
  {
      // 1. Extract sample, report rates from client_request.
      // 2. Configure sensor HW.
      // 3. sendRequest() for Timer to start/stop in case of polling using timer_data_stream.
      // 4. sendRequest() for Intrerupt register/de-register in case of DRI using interrupt_data_stream.
      // 5. Save the current config information like type, sample_rate, report_rate, etc.
      desired_sample_rate = payload->sample_rate;
      desired_report_rate = payload->report_rate;
      sensor_type = payload->sensor_type;
      op_mode = payload->op_mode;
      if (desired_report_rate > desired_sample_rate)
      {
          /* bad request. Return error or default report_rate to sample_rate */
          desired_report_rate = desired_sample_rate;
      }

      if (sensor_type == SX9XXX_SAR)
      {
          rv = sx931x_validate_sensor_sar_odr(this);
          if(rv != SNS_RC_SUCCESS && desired_sample_rate != 0)
          {
            // TODO Unsupported rate. Report error using sns_std_error_event.
            SNS_INST_PRINTF(ERROR, this, "sensor_pressure ODR match error %d", rv);
          }
      }

      if (CONFIG_IDLE == state->config_step && sx931x_dae_if_stop_streaming(this))
      {
          state->config_step = CONFIG_STOPPING_STREAM;
      }
      if (state->config_step == CONFIG_IDLE)
      {
          sx931x_reconfig_hw(this, sensor_type);
      }
      sx931x_send_config_event(this);
  }
  else if(client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG)
  {

  }
  else if (client_request->message_id ==
           SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
  {
      sx931x_run_self_test(this);
      state->new_self_test_request = false;
  }

  /* Turn COM port OFF */
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);
  
  return SNS_RC_SUCCESS;
}

