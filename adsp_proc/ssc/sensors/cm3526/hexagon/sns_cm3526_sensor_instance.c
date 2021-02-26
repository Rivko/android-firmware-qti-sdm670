 /********************************************************************************
* Copyright (c) 2017, Vishay Capella Microsystems
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of Vishay Capella Microsystems nor the
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
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_types.h"
#include "sns_cm3526_hal.h"
#include "sns_cm3526_sensor.h"
#include "sns_cm3526_sensor_instance.h"
#include "sns_timer.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_diag_service.h"
#include "sns_diag.pb.h"
#include "sns_sync_com_port_service.h"
#include "sns_sensor_util.h"
#include "sns_printf.h"
																			 
/** See sns_sensor_instance_api::init */
sns_rc cm3526_inst_init(sns_sensor_instance *const this, sns_sensor_state const *sstate)
{
  cm3526_instance_state *state = (cm3526_instance_state*)this->state->state;
  cm3526_state *sensor_state = (cm3526_state*)sstate->state;
  float uv_data[5];
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr = (sns_stream_service*)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
  state->diag_service = (sns_diag_service*)service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);
  state->scp_service = (sns_sync_com_port_service *)service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);
  uint64_t buffer[10];
  pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float diag_temp[CM3526_NUM_AXES];
  pb_float_arr_arg arg = {.arr = (float*)diag_temp, .arr_len = CM3526_NUM_AXES,
    .arr_index = &arr_index};
  batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
  batch_sample.sample.arg = &arg;

  // Set up stream connections with dependent Sensors
  stream_mgr->api->create_sensor_instance_stream(stream_mgr,
  											     this,
												 sensor_state->common.timer_suid,
												 &state->timer_data_stream);

  state->encoded_uv_event_len = pb_get_encoded_size_sensor_stream_event(uv_data, 5);

  /** Initialize COM port to be used by the Instance */
  sns_memscpy(&state->com_port_info.com_config,
	  		  sizeof(state->com_port_info.com_config),
			  &sensor_state->common.com_port_info.com_config,
			  sizeof(sensor_state->common.com_port_info.com_config));

  state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
							  &state->com_port_info.port_handle);

  state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);

  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
																		 false);

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
																			 
  return SNS_RC_SUCCESS;
}

sns_rc cm3526_validate_uv_odr(cm3526_instance_state *state)
{
  sns_rc rc = SNS_RC_SUCCESS;  
  if (state->uv_info.sampling_rate_hz <= CM3526_ODR)
  {
	state->uv_info.sampling_rate_hz = CM3526_ODR;
  }
  else
  {
	state->uv_info.sampling_intvl = 0;	
    return SNS_RC_NOT_SUPPORTED;
  }

	if (rc == SNS_RC_SUCCESS)
	{
		state->uv_info.sampling_intvl =
			sns_convert_ns_to_ticks(1000000000.0 / state->uv_info.sampling_rate_hz);
	}

	return rc;
}

void cm3526_start_uv_polling_timer(sns_sensor_instance *this)
{
  cm3526_instance_state *state = (cm3526_instance_state*)this->state->state;
  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  size_t req_len;
  uint8_t buffer[50];
  sns_memset(buffer, 0, sizeof(buffer));
  req_payload.is_periodic = true;
  req_payload.start_time = sns_get_system_time();
  req_payload.timeout_period = state->uv_info.sampling_intvl;

  req_len = pb_encode_request(buffer,
	  						  sizeof(buffer),
							  &req_payload,
                sns_timer_sensor_config_fields, NULL);
  if (req_len > 0)
  {
	sns_request timer_req =	{ .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
							  .request_len = req_len,
							  .request = buffer };
    state->timer_data_stream->api->send_request(state->timer_data_stream, &timer_req);
	state->uv_info.timer_is_active = true;	
  }
}

/** See sns_sensor_instance_api::set_client_config */
sns_rc cm3526_inst_set_client_config(sns_sensor_instance *const this, sns_request const *client_request)
{
  cm3526_instance_state *state = (cm3526_instance_state*)this->state->state;

  state->client_req_id = client_request->message_id;
  float desired_sample_rate = 0;
  float desired_report_rate = 0;
  sns_rc rv = SNS_RC_SUCCESS;
  sns_cm3526_uv_req *payload = (sns_cm3526_uv_req*)client_request->request; 

  // Turn COM port ON
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);

  // Reset the device if not streaming.
  cm3526_reset_device(state->scp_service,state->com_port_info.port_handle, CM3526_UV);

  if (client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
  {
    // 1. Extract sample, report rates from client_request.
    // 2. Configure sensor HW.
    // 3. sendRequest() for Timer to start/stop in case of polling using timer_data_stream.
    // 4. sendRequest() for Intrerupt register/de-register in case of DRI using interrupt_data_stream.
    // 5. Save the current config information like type, sample_rate, report_rate, etc.
    desired_sample_rate = payload->sample_rate;
    desired_report_rate = payload->report_rate;

    if (desired_report_rate > desired_sample_rate)
    {
      // bad request. Return error or default report_rate to sample_rate
      desired_report_rate = desired_sample_rate;
    }

    rv = cm3526_validate_uv_odr(state);
    if (rv != SNS_RC_SUCCESS)
    {
      // TODO Unsupported rate. Report error using sns_std_error_event.
      //return rv;
    }

    state->uv_req.sample_rate = desired_sample_rate;

	// Start 1Hz Timer for UV sensor
	if (state->uv_info.sampling_intvl > 0)
    {
	  if (!state->uv_info.timer_is_active)
	  {
		cm3526_start_uv_polling_timer(this);
	  }
    }
    else if (state->uv_info.timer_is_active)
    {
	  state->uv_info.timer_is_active = false;
    }

    // update registry configuration
	state->uv_fac_cal = payload->uv_fac_cal;
  }
  else if (state->client_req_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
  {
	cm3526_run_self_test(this);
	state->new_self_test_request = false;
  }
  // Turn COM port OFF
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);

  return SNS_RC_SUCCESS;
}

sns_rc cm3526_inst_deinit(sns_sensor_instance *const this)
{
	cm3526_instance_state *state = (cm3526_instance_state*)this->state->state;
	sns_sensor_util_remove_sensor_instance_stream(this,&state->timer_data_stream);
	if(NULL != state->scp_service)
	{
	  state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
	  state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
      state->scp_service = NULL;
	}
	return SNS_RC_SUCCESS;
}

/**
 * Sends a self-test completion event.
 *
 * @param[i] instance  Instance reference
 * @param[i] uid       Sensor UID
 *
 * @return none
 */
static void cm3526_send_self_test_event(sns_sensor_instance *instance,
                                        sns_sensor_uid *uid, bool test_result,
                                        sns_physical_sensor_test_type type)
{
  uint8_t data[1] = {0};
  pb_buffer_arg buff_arg = (pb_buffer_arg){ .buf = &data, .buf_len = sizeof(data) };
  sns_physical_sensor_test_event test_event = sns_physical_sensor_test_event_init_default;

  test_event.test_passed = test_result;
  test_event.test_type = type;
  test_event.test_data.funcs.encode = &pb_encode_string_cb;
  test_event.test_data.arg = &buff_arg;

  pb_send_event(instance,
                sns_physical_sensor_test_event_fields,
                &test_event,
                sns_get_system_time(),
                SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
                uid);
}

/** See sns_cm3526_sensor_instance.h */
void cm3526_run_self_test(sns_sensor_instance *instance)
{
  cm3526_instance_state *state = (cm3526_instance_state*)instance->state->state;
  sns_rc rv = SNS_RC_SUCCESS;
  uint8_t buffer = 0;
  bool who_am_i_success = false;

  rv = cm3526_get_who_am_i(state->scp_service,
                            state->com_port_info.port_handle,
                            &buffer);
  if(rv == SNS_RC_SUCCESS &&	buffer == CM3526_WHOAMI_VALUE)
  {
    who_am_i_success = true;
  }

  if(state->uv_info.test_info.test_client_present)
  {
	if(state->uv_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      cm3526_send_self_test_event(instance, &state->uv_info.suid,
                                   who_am_i_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM);
    }
	else if(state->uv_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      // Handle factory test. The driver may choose to reject any new
      // streaming/self-test requests when factory test is in progress.
    }
    state->uv_info.test_info.test_client_present = false;
  }
}
