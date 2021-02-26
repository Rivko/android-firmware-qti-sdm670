/*
 * Copyright (c) 2018, ams AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "sns_mem_util.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_types.h"

#include "sns_async_com_port.pb.h"
#include "sns_interrupt.pb.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#include "sns_pb_util.h"
#include "sns_sync_com_port_service.h"
#include "sns_printf.h"

#include "sns_time.h"

#include "sns_tmd2725_hal.h"
#include "sns_tmd2725_sensor.h"
#include "sns_tmd2725_sensor_instance.h"

static void tmd2725_inst_cleanup(sns_sensor_instance *const this, sns_stream_service *stream_mgr)
{
	tmd2725_instance_state *state = (tmd2725_instance_state*)this->state->state;
    UNUSED_VAR(stream_mgr);
    
	sns_sensor_util_remove_sensor_instance_stream(this, &state->interrupt_data_stream);
	sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_data_stream);
    sns_sensor_util_remove_sensor_instance_stream(this, &state->als_timer_data_stream);
    sns_sensor_util_remove_sensor_instance_stream(this, &state->atime_timer_data_stream);
    sns_sensor_util_remove_sensor_instance_stream(this, &state->prox_timer_data_stream);
    sns_sensor_util_remove_sensor_instance_stream(this, &state->max_calib_timer_data_stream);
    state->als_timer_data_stream = NULL;
    state->atime_timer_data_stream = NULL;
    state->prox_timer_data_stream = NULL;
    state->max_calib_timer_data_stream = NULL;
	state->timer_data_stream = NULL;
	state->interrupt_data_stream = NULL;

	if (NULL != state->scp_service)
	{
		state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
		state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
		state->scp_service = NULL;
	}
}

sns_rc tmd2725_inst_init(sns_sensor_instance *const this, sns_sensor_state const *sstate)
{
	tmd2725_instance_state *state = (tmd2725_instance_state*)this->state->state;
	tmd2725_state *sensor_state = (tmd2725_state*)sstate->state;
	sns_service_manager *service_mgr = this->cb->get_service_manager(this);
	sns_stream_service *stream_mgr = (sns_stream_service*)
		service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
	float als_data[2];
	float prox_data[2];
	uint64_t buffer[10];
	pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));
	sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
	sns_rc rv;

	SNS_INST_PRINTF(LOW, this, "TMD2725 tmd2725_inst_ini sensor_state:%x this:%x get_service():%x",
		sensor_state, this, (void*)&service_mgr->get_service);

	state->scp_service = (sns_sync_com_port_service*)
		service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

	state->diag_service =  (sns_diag_service*)
		service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);

	/* Variables initialization */
	state->publish_sensors = 0;
	state->last_publish_sensors = 0;
	state->new_request_sensor = 0;
	state->update_fac_cal_in_registry = false;

	/* Setup stream connections with dependent Sensors */
	stream_mgr->api->create_sensor_instance_stream(stream_mgr,
		this,
		sensor_state->common.irq_suid,
		&state->interrupt_data_stream);

	stream_mgr->api->create_sensor_instance_stream(stream_mgr,
		this,
		sensor_state->common.timer_suid,
		&state->timer_data_stream);
	state->timer_is_active = false;
	stream_mgr->api->create_sensor_instance_stream(stream_mgr,
		this,
		sensor_state->common.timer_suid,
		&state->als_timer_data_stream);
	stream_mgr->api->create_sensor_instance_stream(stream_mgr,
		this,
		sensor_state->common.timer_suid,
		&state->atime_timer_data_stream);
	stream_mgr->api->create_sensor_instance_stream(stream_mgr,
		this,
		sensor_state->common.timer_suid,
		&state->prox_timer_data_stream);
	stream_mgr->api->create_sensor_instance_stream(stream_mgr,
		this,
		sensor_state->common.timer_suid,
		&state->max_calib_timer_data_stream);
	/* Initialize COM port to be used by the Instance */
	sns_memscpy(&state->com_port_info.com_config,
		sizeof(state->com_port_info.com_config),
		&sensor_state->common.com_port_info.com_config,
		sizeof(sensor_state->common.com_port_info.com_config));

	rv = state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
		&state->com_port_info.port_handle);

	if (NULL == state->timer_data_stream ||
		NULL == state->interrupt_data_stream ||
		NULL == state->com_port_info.port_handle ||
		SNS_RC_SUCCESS != rv)
	{
		tmd2725_inst_cleanup(this, stream_mgr);
		return SNS_RC_FAILED;
	}

	/* Copy all Sensor UIDs in instance state */
	sns_memscpy(&state->als_info.als_suid,
		sizeof(state->als_info.als_suid),
		&((sns_sensor_uid)TMD2725_ALS_SUID),
		sizeof(state->als_info.als_suid));
	sns_memscpy(&state->prox_info.prox_suid,
		sizeof(state->prox_info.prox_suid),
		&((sns_sensor_uid)TMD2725_PROX_SUID),
		sizeof(state->prox_info.prox_suid));
	sns_memscpy(&state->timer_suid,
		sizeof(state->timer_suid),
		&sensor_state->common.timer_suid,
		sizeof(sensor_state->common.timer_suid));
	sns_memscpy(&state->irq_suid,
		sizeof(state->irq_suid),
		&sensor_state->common.irq_suid,
		sizeof(sensor_state->common.irq_suid));

	/* Get size of sensor stream event */
	state->encoded_als_event_len = pb_get_encoded_size_sensor_stream_event(als_data, ALS_EVENT_SIZE);
	state->encoded_prox_event_len = pb_get_encoded_size_sensor_stream_event(prox_data, PROX_EVENT_SIZE);

	/* Reset device */
	rv = state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);
	if (SNS_RC_SUCCESS == rv)
	{
		tmd2725_reset_device(state, (TMD2725_ALS|TMD2725_PROX));
	}
	state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);

	/* Initialize IRQ info to be used by the Instance */
	state->irq_info.irq_config = sensor_state->common.irq_config;
	state->irq_info.irq_ready = false;
	state->irq_info.irq_registered = false;

	/* Copy registry configuration */
	sns_memscpy(&state->als_registry_cfg,
				sizeof(sns_registry_phy_sensor_cfg),
				&sensor_state->common.als_registry_cfg,
				sizeof(sns_registry_phy_sensor_cfg));
	sns_memscpy(&state->prox_registry_cfg,
				sizeof(sns_registry_phy_sensor_cfg),
				&sensor_state->common.prox_registry_cfg,
				sizeof(sns_registry_phy_sensor_cfg));

	/* Get coefficient and calibration info for sensors */
	if (sensor_state->common.registry_coefficient_received)
	{
		state->als_info.dgf        = sensor_state->common.coefficient[0];
		state->als_info.b_coef     = sensor_state->common.coefficient[1];
		state->als_info.c_coef     = sensor_state->common.coefficient[2];
		state->als_info.d_coef     = sensor_state->common.coefficient[3];
		state->als_info.use_max    = (bool)sensor_state->common.coefficient[4];
		state->als_info.target_lux = sensor_state->common.coefficient[10];
		SNS_INST_PRINTF(LOW, this, "TMD2725 tmd2725_inst_ini DGF:%d B_Coef:%d C_Coef:%d D_Coef:%d\n",
			state->als_info.dgf, state->als_info.b_coef, state->als_info.c_coef, state->als_info.d_coef);
	}
	else
	{
		state->als_info.dgf        = D_Factor;
		state->als_info.b_coef     = B_Coef;
		state->als_info.c_coef     = C_Coef;
		state->als_info.d_coef     = D_Coef;
		state->als_info.use_max    = USE_MAX;
		state->als_info.target_lux = CALIB_TARGET_LUX;
	}

	if (sensor_state->common.registry_pf_cfg_received)
	{
		state->lux_scale               = sensor_state->common.fac_cal_scale;
		state->lux_bias                = sensor_state->common.fac_cal_bias;
		state->als_fac_cal_version[0]  = sensor_state->common.als_fac_cal_version[0];
		state->als_fac_cal_version[1]  = sensor_state->common.als_fac_cal_version[1];
		state->near_threshold          = sensor_state->common.near_threshold;
		state->far_threshold           = sensor_state->common.far_threshold;
		state->prox_fac_cal_version[0] = sensor_state->common.prox_fac_cal_version[0];
		state->prox_fac_cal_version[1] = sensor_state->common.prox_fac_cal_version[1];
	}
	else
	{
		state->lux_scale      = 1.0;
		state->near_threshold = AMS_PROX_THRESH_FAR;
		state->far_threshold  = AMS_PROX_THRESH_NEAR;
	}

	/* Determine sizes of encoded logs */
	sns_diag_sensor_state_interrupt sensor_state_interrupt =
		sns_diag_sensor_state_interrupt_init_default;
	pb_get_encoded_size(&state->log_interrupt_encoded_size,
		sns_diag_sensor_state_interrupt_fields,
		&sensor_state_interrupt);

	/* Determine size of sns_diag_sensor_state_raw as defined in
	*  sns_diag.proto
	*  sns_diag_sensor_state_raw is a repeated array of samples of
	*  type sns_diag_batch sample. The following determines the
	*  size of sns_diag_sensor_state_raw with a single batch
	*  sample */
 	if (pb_encode_tag(&stream, PB_WT_STRING, sns_diag_sensor_state_raw_sample_tag))
	{
		if(pb_encode_delimited(&stream, sns_diag_batch_sample_fields, &batch_sample))
		{
			state->log_raw_encoded_size = stream.bytes_written;
		}
	}

	return SNS_RC_SUCCESS;
}

sns_rc tmd2725_inst_deinit(sns_sensor_instance *const this)
{
	sns_service_manager *service_mgr = this->cb->get_service_manager(this);
	sns_stream_service *stream_mgr =
		(sns_stream_service*)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

	tmd2725_inst_cleanup(this, stream_mgr);
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
void tmd2725_send_self_test_event(sns_sensor_instance *instance,
									sns_sensor_uid *uid, bool test_result,
									sns_physical_sensor_test_type type)
{
	uint8_t data[1] = {0};
	pb_buffer_arg buff_arg = (pb_buffer_arg)
		{.buf = &data, .buf_len = sizeof(data)};
	sns_physical_sensor_test_event test_event =
		sns_physical_sensor_test_event_init_default;

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

void tmd2725_run_self_test(sns_sensor_instance *instance)
{
	tmd2725_instance_state *state = (tmd2725_instance_state*)instance->state->state;
	uint8_t buffer[1] = {0};
	bool who_am_i_success = false;
	sns_rc rv = SNS_RC_SUCCESS;

	if (state->als_info.test_info.test_client_present)
	{
		if (SNS_PHYSICAL_SENSOR_TEST_TYPE_COM == state->als_info.test_info.test_type)
		{
			rv = tmd2725_get_who_am_i(state->scp_service,
							state->com_port_info.port_handle,
							&buffer[0]);
			if (SNS_RC_SUCCESS == rv && TMD2725_WHOAMI_VALUE == buffer[0])
			{
				who_am_i_success = true;
			}

			tmd2725_send_self_test_event(instance, &state->als_info.als_suid,
				who_am_i_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM);
		}
		else if(SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY == state->als_info.test_info.test_type)
		{
			/* Update_fac_cal_in_registry is used to demonstrate a registry write operation.*/
			state->update_fac_cal_in_registry = true;
            tmd2725_als_factory_calibration(instance);
		}

		state->als_info.test_info.test_client_present = false;
	}

	if (state->prox_info.test_info.test_client_present)
	{
		if (SNS_PHYSICAL_SENSOR_TEST_TYPE_COM == state->prox_info.test_info.test_type)
		{
			rv = tmd2725_get_who_am_i(state->scp_service,
							state->com_port_info.port_handle,
							&buffer[0]);
			if (SNS_RC_SUCCESS == rv && TMD2725_WHOAMI_VALUE == buffer[0])
			{
				who_am_i_success = true;
			}

			tmd2725_send_self_test_event(instance, &state->prox_info.prox_suid,
				who_am_i_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM);
		}
		else if(SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY == state->prox_info.test_info.test_type)
		{
            tmd2725_prox_factory_calibration(instance);
		}

		state->prox_info.test_info.test_client_present = false;
	}
}
