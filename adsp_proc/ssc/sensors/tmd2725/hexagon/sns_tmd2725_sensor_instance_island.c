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

#include "sns_island_service.h"
#include "sns_mem_util.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_time.h"
#include "sns_types.h"

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

#include "sns_tmd2725_hal.h"
#include "sns_tmd2725_sensor.h"
#include "sns_tmd2725_sensor_instance.h"

static float lux_sum = 0;
static uint8_t count;

void tmd2725_register_interrupt(sns_sensor_instance *this)
{
	tmd2725_instance_state *state = (tmd2725_instance_state*)this->state->state;
	if (!state->irq_info.irq_registered)
	{
		sns_data_stream* data_stream = state->interrupt_data_stream;
		uint8_t buffer[20];
		sns_request irq_req =
		{
			.message_id = SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REQ,
			.request    = buffer
		};

		irq_req.request_len = pb_encode_request(buffer,
			sizeof(buffer),
			&state->irq_info.irq_config,
			sns_interrupt_req_fields,
			NULL);
		if (irq_req.request_len > 0)
		{
			data_stream->api->send_request(data_stream, &irq_req);
			state->irq_info.irq_registered = true;
		}
	}
}

void tmd2725_start_polling_timer(sns_sensor_instance *const this,
									sns_time timeout_ticks)
{
	tmd2725_instance_state *state = (tmd2725_instance_state*)this->state->state;
	sns_service_manager *service_mgr = this->cb->get_service_manager(this);
	sns_stream_service *stream_mgr = (sns_stream_service*)
	service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

	sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
	size_t req_len;
	uint8_t buffer[20];
	req_payload.is_periodic = true;
	req_payload.start_time = sns_get_system_time() - timeout_ticks;
    req_payload.has_start_config = true;
    req_payload.start_config.early_start_delta = 0;
    req_payload.start_config.late_start_delta = timeout_ticks;
	req_payload.timeout_period = timeout_ticks;

	if (NULL == state->timer_data_stream)
	{
		stream_mgr->api->create_sensor_instance_stream(stream_mgr,
			this,
			state->timer_suid,
			&state->timer_data_stream);
	}

	req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
		sns_timer_sensor_config_fields, NULL);

	if(req_len > 0 && NULL != state->timer_data_stream)
	{
		sns_request timer_req =
			{.message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
			.request = buffer, .request_len = req_len};
		state->timer_data_stream->api->send_request(state->timer_data_stream, &timer_req);
	}
	else
	{
		SNS_INST_PRINTF(ERROR, this, "TMD2725 polling timer req encode error");
	}
}

static void tmd2725_handle_interrupt_event(sns_sensor_instance *const this)
{
    tmd2725_instance_state *state = (tmd2725_instance_state*)this->state->state;
    sns_sensor_event *event;
    /* Handle interrupts */
    if (NULL != state->interrupt_data_stream)
    {
        event = state->interrupt_data_stream->api->peek_input(state->interrupt_data_stream);
        while (NULL != event)
        {
            if (event->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT)
            {
                state->irq_info.irq_ready = true;
                SNS_INST_PRINTF(LOW, this, "TMD2725 tmd2725_inst_notify_event SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT");
            }
            else if (SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT == event->message_id)
            {
                sns_interrupt_event irq_event = sns_interrupt_event_init_zero;
                pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);
                if (pb_decode(&stream, sns_interrupt_event_fields, &irq_event))
                {
                    tmd2725_process_sensor_data(this);
                    if (!state->als_info.test_info.test_client_present &&
                        !state->prox_info.test_info.test_client_present)
                    {
                        if ((state->publish_sensors&TMD2725_ALS) && state->als_to_report)
                        {
                            tmd2725_handle_sensor_als_sample(this, irq_event.timestamp);
                        }
                        if ((state->publish_sensors&TMD2725_PROX) && state->prox_to_report)
                        {
                            tmd2725_handle_sensor_prox_sample(this, irq_event.timestamp);
                        }
                    }
                }
                SNS_INST_PRINTF(LOW, this, "TMD2725 tmd2725_inst_notify_event SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT\n");
            }
            else
            {
                SNS_INST_PRINTF(ERROR, this, "TMD2725 Received invalid event id=%d", event->message_id);
            }
            event = state->interrupt_data_stream->api->get_next_input(state->interrupt_data_stream);
        }
    }
}

static void tmd2725_handle_timer_event(sns_sensor_instance *const this)
{
    tmd2725_instance_state *state = (tmd2725_instance_state*)this->state->state;
	/* Handle Timer events */
	if (NULL != state->timer_data_stream)
	{
	    sns_sensor_event *event;
		event = state->timer_data_stream->api->peek_input(state->timer_data_stream);
		while (NULL != event)
		{
			pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);

			if (SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT == event->message_id)
			{
				sns_timer_sensor_event timer_event;
				if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
				{
					tmd2725_process_sensor_data(this);
					if (!state->als_info.test_info.test_client_present &&
						!state->prox_info.test_info.test_client_present)
					{
						if (!state->als_registry_cfg.is_dri && (state->publish_sensors&TMD2725_ALS))
						{
							tmd2725_handle_sensor_als_sample(this, timer_event.timeout_time);
						}
						if (!state->prox_registry_cfg.is_dri && (state->publish_sensors&TMD2725_PROX))
						{
							tmd2725_handle_sensor_prox_sample(this, timer_event.timeout_time);
						}
					}
				}
			}
			else if (SNS_TIMER_MSGID_SNS_TIMER_SENSOR_REG_EVENT == event->message_id)
			{
				/* TODO: decode and qse timer_reg_event*/
				SNS_INST_PRINTF(LOW, this, "TMD2725 TIMER_SENSOR_REG_EVENT");
			}
			else
			{
				SNS_INST_PRINTF(MED, this, "TMD2725 unknown message_id %d", event->message_id);
			}
			event = state->timer_data_stream->api->get_next_input(state->timer_data_stream);
		}
	}

    if (NULL != state->als_timer_data_stream) 
    {
        sns_sensor_event *event;
		event = state->als_timer_data_stream->api->peek_input(state->als_timer_data_stream);
		while (NULL != event)
		{
			pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);

			if (SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT == event->message_id)
			{
				sns_timer_sensor_event timer_event;
				if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
				{
                    sns_rc rv;
                    uint8_t buffer;
                    uint32_t xfer_bytes;
                    sns_port_vector enable_vec;
                    uint8_t enable_val = PON|AEN;
                    enable_vec.buffer = &enable_val;
                    enable_vec.bytes = 1;
                    enable_vec.is_write = true;
                    enable_vec.reg_addr = TMD2725_ENABLE_REG;
                    sns_port_vector status_vec;
                    uint8_t status_val = 0xFF;
                    status_vec.buffer = &status_val;
                    status_vec.bytes = 1;
                    status_vec.is_write = true;
                    status_vec.reg_addr = TMD2725_STATUS_REG;
                    sns_port_vector vectors[] = {enable_vec, status_vec}; 
                    rv = tmd2725_com_rw_wrapper(state->scp_service, 
                                state->com_port_info.port_handle, 
                                vectors, 
                                ARRAY_SIZE(vectors), 
                                &xfer_bytes);
                	if (SNS_RC_SUCCESS != rv)
                	{
                		goto als_fac_cal_exit;
                	}
                    tmd2725_inst_i2c_read_byte(state, TMD2725_ATIME_REG, &buffer);
            		uint8_t atime = (float)(buffer+1) * ATIME_PER_STEP_X100 / 100.0f;
                    tmd2725_start_timer(this, &state->atime_timer_data_stream, 
                        sns_convert_ns_to_ticks((uint64_t)(atime+10)*1000*1000));
                    count = 0;
					lux_sum = 0;

                als_fac_cal_exit:
                    if (SNS_RC_SUCCESS != rv && 
                        SNS_RC_SUCCESS != tmd2725_inst_i2c_write_byte(state, TMD2725_ENABLE_REG, state->enable_backup))
                	{
                		rv = SNS_RC_FAILED;
                	}
                }
            }
            event = state->als_timer_data_stream->api->get_next_input(state->als_timer_data_stream);
        }
    }
    
    if (NULL != state->atime_timer_data_stream) 
    {
        sns_sensor_event *event;
		event = state->atime_timer_data_stream->api->peek_input(state->atime_timer_data_stream);
		while (NULL != event)
		{
			pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);

			if (SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT == event->message_id)
			{
				sns_timer_sensor_event timer_event;
				if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
				{
                    sns_rc rv = SNS_RC_SUCCESS;
                    uint8_t buffer;
                	/* Accumulate the calculated lux */
               		tmd2725_inst_i2c_read_byte(state, TMD2725_ATIME_REG, &buffer);
               		uint8_t atime = (float)(buffer+1) * ATIME_PER_STEP_X100 / 100.0f;
               		tmd2725_start_timer(this, &state->atime_timer_data_stream, 
                            sns_convert_ns_to_ticks((uint64_t)(atime+10)*1000*1000));

            		state->als_to_report = false;
            		tmd2725_get_lux(state, false);
            		if (state->als_to_report)
            		{
            			lux_sum += state->als_info.lux;
            			count++;
            		}

                	/* Calculate the new lux_scale to match the target Lux */
					if (3 == count)
					{
	                	lux_sum /= 3;
	                	state->lux_scale = state->als_info.target_lux / (lux_sum / state->lux_scale);
	                    if (SNS_RC_SUCCESS != tmd2725_inst_i2c_write_byte(state, TMD2725_ENABLE_REG, state->enable_backup))
	                	{
	                		rv = SNS_RC_FAILED;
	                	}
	                    if (SNS_RC_SUCCESS == rv)
	                    {
	                        state->als_fac_cal_version[0]++;
	                        tmd2725_send_self_test_event(this, &state->als_info.als_suid,
	                            true, SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY);
	                    } 
	                    else 
	                    {
	                        tmd2725_send_self_test_event(this, &state->als_info.als_suid,
	                            false, SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY);
	                    }
                    }
                }
            }
            event = state->atime_timer_data_stream->api->get_next_input(state->atime_timer_data_stream);
        }
    }
        
    if (NULL != state->prox_timer_data_stream)
    {
        sns_sensor_event *event;
        event = state->prox_timer_data_stream->api->peek_input(state->prox_timer_data_stream);
        while (NULL != event)
        {
            pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);
        
            if (SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT == event->message_id)
            {
                sns_timer_sensor_event timer_event;
                if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
                {
                    sns_rc rv = SNS_RC_SUCCESS;
                    uint32_t xfer_bytes;
                    sns_port_vector status_vec;
                    uint8_t status_val = 0xFF;
                    status_vec.buffer = &status_val;
                    status_vec.bytes = 1;
                    status_vec.is_write = true;
                    status_vec.reg_addr = TMD2725_STATUS_REG;
                    sns_port_vector calibcfg_vec;
                    uint8_t calibcfg_val = BINSRCH_TARGET_15 | CALIBCFG_RESERVED | AUTO_OFFSET_ADJ | PROX_DATA_AVG_4;
                    calibcfg_vec.buffer = &calibcfg_val;
                    calibcfg_vec.bytes = 1;
                    calibcfg_vec.is_write = true;
                    calibcfg_vec.reg_addr = TMD2725_CALIBCFG_REG;
                    sns_port_vector calib_vec;
                    uint8_t calib_val = START_OFFSET_CALIB;
                    calib_vec.buffer = &calib_val;
                    calib_vec.bytes = 1;
                    calib_vec.is_write = true;
                    calib_vec.reg_addr = TMD2725_CALIB_REG;
                    sns_port_vector vectors[] = {status_vec, calibcfg_vec, calib_vec}; 
                    rv = tmd2725_com_rw_wrapper(state->scp_service, 
                                state->com_port_info.port_handle, 
                                vectors, 
                                ARRAY_SIZE(vectors), 
                                &xfer_bytes);
                  	if (SNS_RC_SUCCESS != rv)
                	{
                		goto prox_fac_cal_exit;
                	}
                    tmd2725_start_timer(this, &state->max_calib_timer_data_stream, TMD2725_MAX_CALIB_TIME*1000*1000);
                prox_fac_cal_exit:
                	if (SNS_RC_SUCCESS != rv && SNS_RC_SUCCESS != tmd2725_inst_i2c_write_byte(state, TMD2725_ENABLE_REG, state->enable_backup))
                	{
                		rv = SNS_RC_FAILED;
                	}
                }
            }
            event = state->prox_timer_data_stream->api->get_next_input(state->prox_timer_data_stream);
        }
    }
        
    if (NULL != state->max_calib_timer_data_stream)
    {
        SNS_INST_PRINTF(LOW, this, "TMD2725 max_calib_timer_data_stream");
        sns_sensor_event *event;
        event = state->max_calib_timer_data_stream->api->peek_input(state->max_calib_timer_data_stream);
        while (NULL != event)
        {
            pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);
        
            if (SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT == event->message_id)
            {
                sns_timer_sensor_event timer_event;
                if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
                {
                    sns_rc rv = SNS_RC_SUCCESS;
                    uint8_t status = 0;
                    uint8_t offset[2] = {0};;
                    rv = tmd2725_inst_i2c_read_byte(state, TMD2725_STATUS_REG, &status);
                    if (SNS_RC_SUCCESS != rv)
                    {
                        goto prox_fac_cal_exit_2;
                    }
                    if (CINT == (status&CINT))
                    {
                        uint32_t xfer_bytes;
                        sns_port_vector poffset_vec;
                        poffset_vec.buffer = &offset[0];
                        poffset_vec.bytes = 2;
                        poffset_vec.is_write = false;
                        poffset_vec.reg_addr = TMD2725_POFFSETL_REG;
                        rv = tmd2725_com_rw_wrapper(state->scp_service, 
                                    state->com_port_info.port_handle, 
                                    &poffset_vec, 
                                    1, 
                                    &xfer_bytes);
                        if (SNS_RC_SUCCESS != rv)
                        {
                            goto prox_fac_cal_exit_2;
                        }
                    
                        rv = tmd2725_inst_i2c_write_byte(state, TMD2725_STATUS_REG, CINT);
                        if (SNS_RC_SUCCESS != rv)
                        {
                            goto prox_fac_cal_exit;
                        }
                    }
                    
                    if (0 == offset[1] && offset[0] > TMD2725_MAX_POFFSET)
                    {
                        rv = SNS_RC_FAILED;
                    }
                prox_fac_cal_exit_2:
                	if (SNS_RC_SUCCESS != tmd2725_inst_i2c_write_byte(state, TMD2725_ENABLE_REG, state->enable_backup))
                	{
                		rv = SNS_RC_FAILED;
                	}

                    tmd2725_send_self_test_event(this, &state->prox_info.prox_suid,
                         SNS_RC_SUCCESS  == rv ? true : false, SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY);

                }
            }
            event = state->max_calib_timer_data_stream->api->get_next_input(state->max_calib_timer_data_stream);
        }
    }
}
static sns_rc tmd2725_inst_notify_event(sns_sensor_instance *const this)
{
	tmd2725_instance_state *state = (tmd2725_instance_state*)this->state->state;

	SNS_INST_PRINTF(LOW, this, "TMD2725 tmd2725_inst_notify_event");

	/* Turn COM port ON */
	state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);
    tmd2725_handle_interrupt_event(this);
    tmd2725_handle_timer_event(this);
	/* Turn COM port OFF */
	state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);
	return SNS_RC_SUCCESS;
}

static sns_rc tmd2725_inst_set_client_config(sns_sensor_instance *const this,
									sns_request const *client_request)
{
	tmd2725_instance_state *state = (tmd2725_instance_state*)this->state->state;
	float desired_sample_rate = 0.0f;
	float desired_report_rate = 0.0f;
	uint8_t enable = 0;
	uint8_t intenab = 0;
	sns_tmd2725_req *payload = (sns_tmd2725_req*)client_request->request;

	SNS_INST_PRINTF(LOW, this, "TMD2725 set_client_cfg msg_id %d", client_request->message_id);

	/* Turn COM port ON */
	state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);

	if (SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG == client_request->message_id)
	{
		// 1. Extract sample, report rates from client_request.
		// 2. Configure sensor HW.
		// 3. sendRequest() for Timer to start/stop in case of polling using timer_data_stream.
		// 4. sendRequest() for Intrerupt register/de-register in case of DRI using interrupt_data_stream.
		// 5. Save the current config information like type, sample_rate, report_rate, etc.
		desired_sample_rate = payload->desired_sample_rate;
		desired_report_rate = payload->desired_report_rate;

		if (0 == state->publish_sensors)
		{
			tmd2725_modify_enable(state, 0xFF, 0);
			if (state->als_registry_cfg.is_dri || state->prox_registry_cfg.is_dri)
			{
				tmd2725_modify_intenab(state, 0xFF, 0);
			}
		}
		else
		{
			enable = PON;
			if (state->first_prox)
			{
				if (state->prox_registry_cfg.is_dri)
				{
					intenab = CIEN;
				}
			}
			else
			{
				if (state->publish_sensors&TMD2725_ALS)
				{
					enable |= AEN;
				}
				if (state->publish_sensors&TMD2725_PROX)
				{
					enable |= PEN;
				}
				if (PEN == (enable&(AEN|PEN)))
				{
					enable |= WEN;
				}

				if (state->als_registry_cfg.is_dri && (state->publish_sensors&TMD2725_ALS))
				{
					intenab |= AIEN;
				}
				if (state->prox_registry_cfg.is_dri && (state->publish_sensors&TMD2725_PROX))
				{
					intenab |= PIEN;
				}
			}

			/* Register interrupt if some sensor is working in DRI mode */
			if ((state->als_registry_cfg.is_dri && (state->publish_sensors&TMD2725_ALS)) ||
				(state->prox_registry_cfg.is_dri && (state->publish_sensors&TMD2725_PROX)))
			{
				tmd2725_register_interrupt(this);
				tmd2725_modify_intenab(state, (PIEN|AIEN|CIEN), intenab);
				if (state->new_request_sensor&TMD2725_ALS)
				{
					tmd2725_set_als_pers(state, 0);
				}
				if (state->new_request_sensor&TMD2725_PROX)
				{
					tmd2725_set_prox_pers(state, 0);
				}
				state->new_request_sensor = 0;
				SNS_INST_PRINTF(LOW, this, "TMD2725 set_client_cfg register interrupt\n");
			}

			/* Register timer if some sensor is working in polling mode */
			if ((!state->als_registry_cfg.is_dri && (state->publish_sensors&TMD2725_ALS)) ||
				(!state->prox_registry_cfg.is_dri && (state->publish_sensors&TMD2725_PROX)))
			{
				SNS_INST_PRINTF(LOW, this, "TMD2725 set_client_cfg timer_is_active:%d\n", state->timer_is_active);
				if (!state->timer_is_active)
				{
					tmd2725_start_polling_timer(this, sns_convert_ns_to_ticks(TMD2725_POLLING_MS * 1000 * 1000));
					state->timer_is_active = true;
				}
			}
			else
			{
				sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_data_stream);
				state->timer_data_stream = NULL;
				state->timer_is_active = false;
			}

			/* Enable proximity first to response to first proximity event faster */
			tmd2725_modify_enable(state, (PEN|PON), enable);
			tmd2725_modify_enable(state, (WEN|AEN), enable);
			if (state->first_prox)
			{
				tmd2725_start_calibration(state);
			}
		}

		state->last_publish_sensors = state->publish_sensors;
		tmd2725_send_config_event(this);
	}
	else if (client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG)
	{
	}
	else if (client_request->message_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
	{
		/* All self-tests can be handled in normal mode. */
		//state->island_service->api->sensor_instance_island_exit(state->island_service, this);
		tmd2725_run_self_test(this);
		state->new_self_test_request = false;
	}

	/* Turn COM port OFF */
	state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);

	return SNS_RC_SUCCESS;
}

/* Public Data Definitions. */
sns_sensor_instance_api tmd2725_sensor_instance_api =
{
	.struct_len        = sizeof(sns_sensor_instance_api),
	.init              = &tmd2725_inst_init,
	.deinit            = &tmd2725_inst_deinit,
	.notify_event      = &tmd2725_inst_notify_event,
	.set_client_config = &tmd2725_inst_set_client_config,
};
