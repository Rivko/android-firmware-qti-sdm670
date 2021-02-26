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
#include "sns_sensor_util.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_types.h"

#include "sns_cm32181_hal.h"
#include "sns_cm32181_sensor.h"
#include "sns_cm32181_sensor_instance.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_diag_service.h"
#include "sns_diag.pb.h"
#include "sns_sync_com_port_service.h"
#include "sns_printf.h"

static void inst_cleanup(sns_sensor_instance *const this)
{
	cm32181_instance_state *state = (cm32181_instance_state*)this->state->state;	
	
	sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_data_stream);		

	if (NULL != state->scp_service)
	{
		state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
		state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
		state->scp_service = NULL;
	}
}

/** See sns_sensor_instance_api::init */
sns_rc cm32181_inst_init(sns_sensor_instance *const this, sns_sensor_state const *sstate)
{
	cm32181_instance_state *state = (cm32181_instance_state*)this->state->state;
	cm32181_state *sensor_state = (cm32181_state*)sstate->state;
	float ambient_light_data[2];
	sns_service_manager *service_mgr = this->cb->get_service_manager(this);
	sns_stream_service *stream_mgr = (sns_stream_service*)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
	state->diag_service = (sns_diag_service*)service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);
	state->scp_service = (sns_sync_com_port_service *)service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);
	uint64_t buffer[10];
	pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));
	sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
	uint8_t arr_index = 0;
	float diag_temp[CM32181_NUM_AXES];
	pb_float_arr_arg arg = {.arr = (float*)diag_temp, .arr_len = CM32181_NUM_AXES, .arr_index = &arr_index};
	batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
	batch_sample.sample.arg = &arg;
	
	SNS_INST_PRINTF(MED, this, "cm32181_inst_init: Enter");	

	// Set up stream connections with dependent Sensors
	stream_mgr->api->create_sensor_instance_stream(stream_mgr,
												   this,
												   sensor_state->timer_suid,
												   &state->timer_data_stream);
	
	/** Initialize COM port to be used by the Instance */
	sns_memscpy(&state->com_port_info.com_config,
				sizeof(state->com_port_info.com_config),
				&sensor_state->com_port_info.com_config,
				sizeof(sensor_state->com_port_info.com_config));

	state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
							  &state->com_port_info.port_handle);

	if ((NULL == state->timer_data_stream) || (NULL == state->com_port_info.port_handle))
	{
		inst_cleanup(this);
		return SNS_RC_FAILED;
	}							  

	state->encoded_ambient_light_event_len = pb_get_encoded_size_sensor_stream_event(ambient_light_data, 2);
	
	state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);

	state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);
	
	/** Determine size of sns_diag_sensor_state_raw as defined in
	*  sns_diag.proto
	*  sns_diag_sensor_state_raw is a repeated array of samples of
	*  type sns_diag_batch sample. The following determines the
	*  size of sns_diag_sensor_state_raw with a single batch
	*  sample */
	if (pb_encode_tag(&stream, PB_WT_STRING, sns_diag_sensor_state_raw_sample_tag))
	{
		if (pb_encode_delimited(&stream, sns_diag_batch_sample_fields, &batch_sample))
		{
			state->log_raw_encoded_size = stream.bytes_written;
		}
	}

	SNS_INST_PRINTF(MED, this, "cm32181_inst_init: Exit");	
																			 
	return SNS_RC_SUCCESS;
}

sns_rc cm32181_validate_ambient_light_odr(cm32181_instance_state *state)
{
	sns_rc rc = SNS_RC_SUCCESS;
  
	if (state->ambient_light_info.sampling_rate_hz <= CM32181_ODR)
	{
		state->ambient_light_info.sampling_rate_hz = CM32181_ODR;
	}
	else
	{
		state->ambient_light_info.sampling_intvl = 0;	
		return SNS_RC_NOT_SUPPORTED;
	}

	if (rc == SNS_RC_SUCCESS)
	{
		state->ambient_light_info.sampling_intvl =
			sns_convert_ns_to_ticks(1000000000.0 / state->ambient_light_info.sampling_rate_hz);
	}

	return rc;
}

/** See sns_sensor_instance_api::set_client_config */
sns_rc cm32181_inst_set_client_config(sns_sensor_instance *const this, sns_request const *client_request)
{
	cm32181_instance_state *state = (cm32181_instance_state*)this->state->state;
	sns_rc rv = SNS_RC_SUCCESS;
  
	SNS_INST_PRINTF(MED, this, "cm32181_inst_set_client_config: Enter");

	float desired_sample_rate = 0;
	float desired_report_rate = 0;
	sns_cm32181_ambient_light_req *payload = (sns_cm32181_ambient_light_req*)client_request->request; 

	// Turn COM port ON
	state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);

	// Reset the device if not streaming.
	cm32181_reset_device(state->scp_service,state->com_port_info.port_handle, CM32181_AMBIENT_LIGHT);

	SNS_INST_PRINTF(MED, this, "cm32181_inst_set_client_config: client_request->message_id = %d", client_request->message_id);	
	
	if (client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
	{
		// 1. Extract sample, report rates from client_request.
		// 2. Configure sensor HW.
		// 3. sendRequest() for Timer to start/stop in case of polling using timer_data_stream.
		// 4. sendRequest() for Intrerupt register/de-register in case of DRI using interrupt_data_stream.
		// 5. Save the current config information like type, sample_rate, report_rate, etc.
		if (client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
		{
			desired_sample_rate = payload->sample_rate;
			desired_report_rate = payload->report_rate;
		}
		else // TODO self-test
		{

		}

		if (desired_report_rate > desired_sample_rate)
		{
			// bad request. Return error or default report_rate to sample_rate
			desired_report_rate = desired_sample_rate;
		}	

		rv = cm32181_validate_ambient_light_odr(state);
		if (rv != SNS_RC_SUCCESS)
		{
			// TODO Unsupported rate. Report error using sns_std_error_event.
			SNS_INST_PRINTF(MED, this, "cm32181_inst_set_client_config: ODR match error: %d", rv);		
			//return rv;
		}
		
		if (state->publish_sensors & CM32181_AMBIENT_LIGHT)
		{
			cm32181_set_polling_config(this);
		}

		cm32181_send_config_event(this);		
		
		// update registry configuration
		state->ambient_light_fac_cal = payload->ambient_light_fac_cal;
	}
	else if (client_request->message_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
	{
		cm32181_run_self_test(this);
		state->new_self_test_request = false;
	}

	// Turn COM port OFF
	state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);

	SNS_INST_PRINTF(MED, this, "cm32181_inst_set_client_config: Exit");

	return SNS_RC_SUCCESS;
}

sns_rc cm32181_inst_deinit(sns_sensor_instance *const this)
{
	SNS_INST_PRINTF(MED, this, "cm32181_inst_deinit: Enter");	

	inst_cleanup(this);

	SNS_INST_PRINTF(MED, this, "cm32181_inst_deinit: Exit");	
	
	return SNS_RC_SUCCESS;
}
