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

#include <string.h>
#include "sns_attribute_util.h"
#include "sns_diag_service.h"
#include "sns_event_service.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_sync_com_port_service.h"
#include "sns_types.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_motion_detect.pb.h"
#include "sns_pb_util.h"
#include "sns_std.pb.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_suid.pb.h"
#include "sns_timer.pb.h"
#include "sns_registry.pb.h"
#include "sns_printf.h"

#include "sns_tmd2725_hal.h"
#include "sns_tmd2725_sensor.h"

static sns_sensor_uid const* tmd2725_als_get_sensor_uid(sns_sensor const *const this)
{
	UNUSED_VAR(this);
	static const sns_sensor_uid sensor_uid = TMD2725_ALS_SUID;

	return &sensor_uid;
}

static sns_sensor_uid const* tmd2725_prox_get_sensor_uid(sns_sensor const *const this)
{
	UNUSED_VAR(this);
	static const sns_sensor_uid sensor_uid = TMD2725_PROX_SUID;

	return &sensor_uid;
}

/**
 * Returns decoded request message for type
 * sns_std_sensor_config.
 *
 * @param[in] in_request   Request as sotred in client_requests
 *                         list.
 * @param decoded_request  Standard decoded message.
 * @param decoded_payload  Decoded stream request payload.
 *
 * @return bool true if decode is successful else false
 */
static bool tmd2725_get_decoded_std_request(sns_sensor const *this, sns_request const *in_request,
									sns_std_request *decoded_request,
									sns_std_sensor_config *decoded_payload)
{
	pb_istream_t stream;
	pb_simple_cb_arg arg =
		{.decoded_struct = decoded_payload, .fields = sns_std_sensor_config_fields};
	decoded_request->payload = (struct pb_callback_s)
		{.funcs.decode = &pb_decode_simple_cb, .arg = &arg};
	stream = pb_istream_from_buffer(in_request->request,
		in_request->request_len);
	if (!pb_decode(&stream, sns_std_request_fields, decoded_request))
	{
		SNS_PRINTF(ERROR, this, "TMD2725 decode error");
		return false;
	}
	return true;
}

/**
 * Parses through all starndard streaming requests and deduces
 * best HW config for the inertial sensor type.
 *
 * @param[i] this                 Sensor reference
 * @param[i] instance             Instance reference
 * @param[i] sensor_type          sensor type
 * @param[o] chosen_sample_rate   chosen sample rate in Hz
 * @param[o] chosen_report_rate   chosen report rate in Hz 
 * @param[o] client_present        
 * @param[o] num_clients          
 */
static void tmd2725_get_device_config(sns_sensor *this,
									sns_sensor_instance *instance,
									tmd2725_sensor_type sensor_type,
									float *chosen_sample_rate,
									float *chosen_report_rate,
									bool *client_present,
									int32_t *num_clients)
{
	UNUSED_VAR(this);
	sns_sensor_uid suid;
	sns_request const *request;

	if (TMD2725_ALS == sensor_type)
	{
		sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid)TMD2725_ALS_SUID), sizeof(sns_sensor_uid));
	}
	else if (TMD2725_PROX == sensor_type)
	{
		sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid)TMD2725_PROX_SUID), sizeof(sns_sensor_uid));
	}

	*chosen_report_rate = 0.0f;
	*chosen_sample_rate = 0.0f;
	*client_present = false;
	*num_clients = 0;

	/* Parse through existing requests and get fastest sample
	 *  rate and report rate requests. */
	for (request = instance->cb->get_client_request(instance, &suid, true);
		NULL != request;
		request = instance->cb->get_client_request(instance, &suid, false))
	{
		sns_std_request decoded_request;
		sns_std_sensor_config decoded_payload = {0};
		SNS_PRINTF(LOW, this, "TMD2725 tmd2725_get_device_config message_id=%d\n", request->message_id);

		if (SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG == request->message_id)
		{
			if (tmd2725_get_decoded_std_request(this, request, &decoded_request, &decoded_payload))
			{
				*chosen_sample_rate = SNS_MAX(*chosen_sample_rate, decoded_payload.sample_rate);
				*chosen_report_rate = *chosen_sample_rate;
				*client_present = true;
				(*num_clients)++;
			}
		}
	    else if (SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG == request->message_id)
	    {
			SNS_PRINTF(MED, this, "TMD2725 get_device_config: sensor_type %d (ON_CHANGE)", sensor_type);
			*chosen_sample_rate = 0;
			*chosen_report_rate = 0;
			*client_present = true;
			(*num_clients)++;
	    }
	}
}

/**
 * Set standard streaming config for the instance.
 *
 * @param[i] this        Sensor reference
 * @param[i] instance    Sensor Instance reference
 * @param[i] chosen_report_rate   chosen report rate in Hz
 * @param[i] chosen_sample_rate   chosen sample rate in Hz 
 * @param[i] message_id           input message ID 
 *
 * @return none
 */
static void tmd2725_set_inst_config(sns_sensor *this,
									sns_sensor_instance *instance,
									float chosen_report_rate,
									float chosen_sample_rate,
									uint32_t message_id)
{
	sns_tmd2725_req new_client_config;
	sns_request config;

	new_client_config.desired_report_rate = chosen_report_rate;
	new_client_config.desired_sample_rate = chosen_sample_rate;

	config.message_id = message_id;
	config.request_len = sizeof(sns_tmd2725_req);
	config.request = &new_client_config;

	this->instance_api->set_client_config(instance, &config);
}

void tmd2725_reval_instance_config(sns_sensor *this,
									sns_sensor_instance *instance)
{
	/**
	* 1. Get best ALS/Prox Config.
	* 2. Decide best Instance Config based on above outputs.
	*/
	float chosen_sample_rate = 0.0f;
	float chosen_report_rate = 0.0f;
	float sample_rate = 0.0f;
	float report_rate = 0.0f;
	bool client_present = false;
	int32_t num_clients;
	tmd2725_instance_state *inst_state = (tmd2725_instance_state*)instance->state->state;

	tmd2725_get_device_config(this, instance, TMD2725_ALS, &chosen_sample_rate,
		&chosen_report_rate, &client_present, &num_clients);

	SNS_PRINTF(LOW, this, "TMD2725 ALS: rr %u sr %u num_clients %d",
		(uint32_t)chosen_report_rate, (uint32_t)chosen_sample_rate, num_clients);

	if (client_present)
	{
		/* First ALS is special */
		if (0 == (inst_state->publish_sensors&TMD2725_ALS))
		{
			inst_state->first_als = true;
		}
		inst_state->publish_sensors |= TMD2725_ALS;
	}
	else
	{
		inst_state->publish_sensors &= ~TMD2725_ALS;
		inst_state->first_als = false;
	}

	tmd2725_get_device_config(this, instance, TMD2725_PROX, &sample_rate,
		&report_rate, &client_present, &num_clients);

	SNS_PRINTF(LOW, this, "TMD2725 PROX: rr %u sr %u num_clients %d",
		(uint32_t)chosen_report_rate, (uint32_t)chosen_sample_rate, num_clients);

	if (client_present)
	{
		chosen_sample_rate = SNS_MAX(chosen_sample_rate, sample_rate);
		chosen_report_rate = SNS_MAX(chosen_report_rate, report_rate);

		/* First prox is special */
		if (0 == (inst_state->publish_sensors&TMD2725_PROX))
		{
			inst_state->first_prox = true;
		}
		inst_state->publish_sensors |= TMD2725_PROX;
	}
	else
	{
		inst_state->publish_sensors &= ~TMD2725_PROX;
		inst_state->first_prox = false;
	}

	tmd2725_set_inst_config(this,
		instance,
		chosen_report_rate,
		chosen_sample_rate,
		SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);
}

/**
 * Sets instance config to run a self test.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 *
 * @return none
 */
void tmd2725_set_self_test_inst_config(sns_sensor *this,
									sns_sensor_instance *instance)
{
	sns_request config;

	config.message_id = SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG;
	config.request_len = 0;
	config.request = NULL;

	this->instance_api->set_client_config(instance, &config);
}

void tmd2725_start_power_rail_timer(sns_sensor *const this,
									sns_time timeout_ticks,
									tmd2725_power_rail_pending_state pwr_rail_pend_state)
{
	tmd2725_state *state = (tmd2725_state*)this->state->state;

	sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
	size_t req_len;
	uint8_t buffer[20];
	sns_memset(buffer, 0, sizeof(buffer));
	req_payload.is_periodic = false;
	req_payload.start_time = sns_get_system_time();
	req_payload.timeout_period = timeout_ticks;

	if (NULL == state->timer_stream)
	{
		sns_service_manager *smgr = this->cb->get_service_manager(this);
		sns_stream_service *stream_svc = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);
		stream_svc->api->create_sensor_stream(stream_svc, this, state->common.timer_suid,
			&state->timer_stream);
	}

	req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
			sns_timer_sensor_config_fields, NULL);
	if (req_len > 0 && NULL != state->timer_stream)
	{
		sns_request timer_req =
			{.message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
			.request = buffer, .request_len = req_len};
		state->timer_stream->api->send_request(state->timer_stream, &timer_req);
		state->power_rail_pend_state = pwr_rail_pend_state;
	}
	else
	{
		SNS_PRINTF(ERROR, this, "TMD2725 timer req encode error");
	}
}

sns_rc tmd2725_sensor_notify_event(sns_sensor *const this)
{
	tmd2725_state *state = (tmd2725_state*)this->state->state;
	sns_rc rv = SNS_RC_SUCCESS;
	sns_sensor_event *event;

	SNS_PRINTF(LOW, this, "TMD2725 tmd2725_sensor_notify_event");

	if (state->fw_stream)
	{
		if (sns_sensor_uid_compare(&state->common.irq_suid, &((sns_sensor_uid){{0}}))
			|| sns_sensor_uid_compare(&state->common.timer_suid, &((sns_sensor_uid){{0}}))
			|| sns_sensor_uid_compare(&state->common.reg_suid, &((sns_sensor_uid){{0}})))
		{
			/* All SUID events can be handled in normal mode. */
			//state->island_service->api->sensor_island_exit(state->island_service, this);

			tmd2725_process_suid_events(this);

			if (!sns_sensor_uid_compare(&state->common.reg_suid, &((sns_sensor_uid){{0}})))
			{
				tmd2725_request_registry(this);
			}
		}

		/* Check if the SUID sensor stream can be removed. */
		if (!sns_sensor_uid_compare(&state->common.reg_suid, &((sns_sensor_uid){{0}})))
		{
			/* Non-als sensors only request for registry SUID. */
			if (TMD2725_ALS != state->sensor)
			{
		  		sns_sensor_util_remove_sensor_stream(this, &state->fw_stream);
			}

			if (!sns_sensor_uid_compare(&state->common.irq_suid, &((sns_sensor_uid){{0}}))
				&& !sns_sensor_uid_compare(&state->common.timer_suid, &((sns_sensor_uid){{0}})))
			{
				sns_sensor_util_remove_sensor_stream(this, &state->fw_stream);
			}
		}
	}

	/* Handle a Timer Sensor event */
	if (NULL != state->timer_stream)
	{
		event = state->timer_stream->api->peek_input(state->timer_stream);
		while (NULL != event)
		{
			pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);
			sns_timer_sensor_event timer_event;

			if (SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT == event->message_id)
			{
				if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
				{
					if (TMD2725_POWER_RAIL_PENDING_INIT == state->power_rail_pend_state)
					{
						/* Initial HW discovery is OK to run in normal mode. */
						//state->island_service->api->sensor_island_exit(state->island_service, this);

						state->common.hw_is_present = tmd2725_discover_hw(this);

						if (state->common.hw_is_present)
						{
							tmd2725_publish_available(this);
							tmd2725_update_sibling_sensors(this);
						}
						else
						{
							rv = SNS_RC_INVALID_LIBRARY_STATE;
							SNS_PRINTF(LOW, this, "TMD2725 HW absent");
						}
						state->power_rail_pend_state = TMD2725_POWER_RAIL_PENDING_NONE;
					}
					else if (TMD2725_POWER_RAIL_PENDING_SET_CLIENT_REQ == state->power_rail_pend_state)
					{
						sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
						if (NULL != instance)
						{
							tmd2725_instance_state *inst_state = (tmd2725_instance_state*) instance->state->state;
							inst_state->instance_is_ready_to_configure = true;
							inst_state->new_request_sensor = state->sensor;
							tmd2725_reval_instance_config(this, instance);
							if (inst_state->new_self_test_request)
							{
								tmd2725_set_self_test_inst_config(this, instance);
								SNS_PRINTF(LOW, this, "TMD2725 notify event tmd2725_set_self_test_inst_config\n");
							}
						}
						state->power_rail_pend_state = TMD2725_POWER_RAIL_PENDING_NONE;
					}
				}
				else
				{
					SNS_PRINTF(ERROR, this, "TMD2725 pb_decode error");
				}
			}
			event = state->timer_stream->api->get_next_input(state->timer_stream);
		}
		/* Free up timer stream if not needed anymore */
		if (TMD2725_POWER_RAIL_PENDING_NONE == state->power_rail_pend_state)
		{
			sns_sensor_util_remove_sensor_stream(this, &state->timer_stream);
		}
	}

	if (NULL != state->reg_data_stream)
	{
		event = state->reg_data_stream->api->peek_input(state->reg_data_stream);
		while (NULL != event)
		{
			/* All registry events can be handled in normal mode. */
			//state->island_service->api->sensor_island_exit(state->island_service, this);
			tmd2725_sensor_process_registry_event(this, event);

			event = state->reg_data_stream->api->get_next_input(state->reg_data_stream);
		}
	}

	if (!sns_sensor_uid_compare(&state->common.timer_suid, &((sns_sensor_uid){{0}})) &&
		state->common.registry_pf_cfg_received && state->common.registry_coefficient_received &&
		!state->common.start_hw_detect)
	{
		/* Initial HW detection sequence is OK to run in normal mode. */
		//state->island_service->api->sensor_island_exit(state->island_service, this);

		tmd2725_start_hw_detect_sequence(this);
	}
	return rv;
}

/**
 * Turns power rails off
 *
 * @paramp[i] this   Sensor reference
 *
 * @return none
 */
static void tmd2725_turn_rails_off(sns_sensor *this)
{
	sns_sensor *sensor;

	for (sensor = this->cb->get_library_sensor(this, true);
		NULL != sensor;
		sensor = this->cb->get_library_sensor(this, false))
	{
		tmd2725_state *sensor_state = (tmd2725_state*)sensor->state->state;
		if (SNS_RAIL_OFF != sensor_state->common.rail_config.rail_vote)
		{
			sensor_state->common.rail_config.rail_vote = SNS_RAIL_OFF;
			sensor_state->pwr_rail_service->api->sns_vote_power_rail_update(sensor_state->pwr_rail_service,
				sensor,
				&sensor_state->common.rail_config,
				NULL);
		}
	}
}

/**
 * Decodes self test requests.
 *
 * @param[i] this              Sensor reference
 * @param[i] request           Encoded input request
 * @param[o] decoded_request   Decoded standard request
 * @param[o] test_config       decoded self test request
 *
 * @return bool True if decoding is successfull else false.
 */
static bool tmd2725_get_decoded_self_test_request(sns_sensor const *this,
									sns_request const *request,
									sns_std_request *decoded_request,
									sns_physical_sensor_test_config *test_config)
{
	pb_istream_t stream;
	pb_simple_cb_arg arg =
		{.decoded_struct = test_config,
		.fields = sns_physical_sensor_test_config_fields};
	decoded_request->payload = (struct pb_callback_s)
		{.funcs.decode = &pb_decode_simple_cb, .arg = &arg};
	stream = pb_istream_from_buffer(request->request, request->request_len);
	if (!pb_decode(&stream, sns_std_request_fields, decoded_request))
	{
		SNS_PRINTF(ERROR, this, "TMD2725 decode error");
		return false;
	}
	return true;
}

/**
 * Decodes a physical sensor self test request and updates
 * instance state with request info.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 * @param[i] new_request Encoded request
 *
 * @return True if request is valid else false
 */
static bool tmd2725_extract_self_test_info(sns_sensor *this,
									sns_sensor_instance *instance,
									struct sns_request const *new_request)
{
	sns_std_request decoded_request;
	sns_physical_sensor_test_config test_config = sns_physical_sensor_test_config_init_default;
	tmd2725_state *state = (tmd2725_state*)this->state->state;
	tmd2725_instance_state *inst_state = (tmd2725_instance_state*)instance->state->state;
	tmd2725_self_test_info *self_test_info;

	if (TMD2725_ALS == state->sensor)
	{
		self_test_info = &inst_state->als_info.test_info;
	}
	else if (TMD2725_PROX == state->sensor)
	{
		self_test_info = &inst_state->prox_info.test_info;
	}
	else
	{
		return false;
	}

	if (tmd2725_get_decoded_self_test_request(this, new_request, &decoded_request, &test_config))
	{
		self_test_info->test_type = test_config.test_type;
		self_test_info->test_client_present = true;
		return true;
	}
	else
	{
		return false;
	}
}

sns_sensor_instance* tmd2725_set_client_request(sns_sensor *const this,
									struct sns_request const *exist_request,
									struct sns_request const *new_request,
									bool remove)
{
	sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
	tmd2725_state *state = (tmd2725_state*)this->state->state;
	sns_time on_timestamp;
	sns_time delta;
	bool reval_config = false;

	SNS_PRINTF(LOW, this, "TMD2725 set_client_req 0x%X  0x%X  %d", exist_request, new_request, remove);

	if (remove)
	{
		if (NULL != instance)
		{
			instance->cb->remove_client_request(instance, exist_request);
			SNS_PRINTF(LOW, this, "TMD2725 set_client_req message_id %d\n", exist_request->message_id);
			if (SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG != exist_request->message_id)
			{
				tmd2725_reval_instance_config(this, instance);
			}
		}
	}
	else
	{
		// 1. If new request then:
		//     a. Power ON rails.
		//     b. Power ON COM port - Instance must handle COM port power.
		//     c. Create new instance.
		//     d. Re-evaluate existing requests and choose appropriate instance config.
		//     e. set_client_config for this instance.
		//     f. Add new_request to list of requests handled by the Instance.
		//     g. Power OFF COM port if not needed- Instance must handle COM port power.
		//     h. Return the Instance.
		// 2. If there is an Instance already present:
		//     a. Add new_request to list of requests handled by the Instance.
		//     b. Remove exist_request from list of requests handled by the Instance.
		//     c. Re-evaluate existing requests and choose appropriate Instance config.
		//     d. set_client_config for the Instance if not the same as current config.
		//     e. publish the updated config.
		//     f. Return the Instance.

		if (NULL == instance)
		{
			state->common.rail_config.rail_vote = SNS_RAIL_ON_NPM;

			state->pwr_rail_service->api->sns_vote_power_rail_update(
				state->pwr_rail_service,
				this,
				&state->common.rail_config,
				&on_timestamp);

			delta = sns_get_system_time() - on_timestamp;

			/* Use on_timestamp to determine correct Timer value. */
			if (delta < sns_convert_ns_to_ticks(TMD2725_OFF_TO_IDLE_MS*1000*1000))
			{
				tmd2725_start_power_rail_timer(this,
					sns_convert_ns_to_ticks(TMD2725_OFF_TO_IDLE_MS*1000*1000) - delta,
					TMD2725_POWER_RAIL_PENDING_SET_CLIENT_REQ);
			}
			else
			{
				/* Rail is already ON */
				reval_config = true;
			}

			/* Create_instance() calls init() for the Sensor Instance */
			instance = this->cb->create_instance(this, sizeof(tmd2725_instance_state));

			/* If rail is already ON then flag instance OK to configure */
			if (reval_config)
			{
				tmd2725_instance_state *inst_state = (tmd2725_instance_state*)instance->state->state;

				inst_state->instance_is_ready_to_configure = true;
			}
		}
		else
		{
			if (NULL != exist_request &&
				NULL != new_request &&
				SNS_STD_MSGID_SNS_STD_FLUSH_REQ == new_request->message_id)
			{
				sns_service_manager *mgr = instance->cb->get_service_manager(instance);
				sns_event_service *e_service = (sns_event_service*)mgr->get_service(mgr, SNS_EVENT_SERVICE);
				sns_sensor_event *event = e_service->api->alloc_event(e_service, instance, 0);

				if (NULL != event)
				{
					event->message_id = SNS_STD_MSGID_SNS_STD_FLUSH_EVENT;
					event->event_len = 0;
					event->timestamp = sns_get_system_time();

					e_service->api->publish_event(e_service, instance, event, &state->my_suid);
				}
				return instance;
			}
			else
			{
				reval_config = true;

				/* An existing client is changing request*/
				if ((NULL != exist_request) && (NULL != new_request))
				{
					instance->cb->remove_client_request(instance, exist_request);
				}
				/* A new client sent new_request*/
				else if (NULL != new_request)
				{
					//No-op. new_request will be added to requests list below.
				}
			}
		}

		/* Add the new request to list of client_requests.*/
		if (NULL != instance)
		{
			tmd2725_instance_state *inst_state = (tmd2725_instance_state*)instance->state->state;
			if (NULL != new_request)
			{
				instance->cb->add_client_request(instance, new_request);

				if (SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG == new_request->message_id)
				{
					if (tmd2725_extract_self_test_info(this, instance, new_request))
					{
						inst_state->new_self_test_request = true;
					}
					SNS_PRINTF(LOW, this, "TMD2725 SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG %d\n",
						inst_state->new_self_test_request);
				}
			}
			if (reval_config && inst_state->instance_is_ready_to_configure)
			{
				inst_state->new_request_sensor = state->sensor;
				tmd2725_reval_instance_config(this, instance);
				if (inst_state->new_self_test_request)
				{
					tmd2725_set_self_test_inst_config(this, instance);
					SNS_PRINTF(LOW, this, "TMD2725 set_client_requet tmd2725_set_self_test_inst_config\n");
				}
			}
		}
	}

	if (NULL != instance &&
		NULL == instance->cb->get_client_request(instance, &(sns_sensor_uid)TMD2725_ALS_SUID, true) &&
		NULL == instance->cb->get_client_request(instance, &(sns_sensor_uid)TMD2725_PROX_SUID, true))
	{
		tmd2725_instance_state *inst_state = (tmd2725_instance_state*)instance->state->state;
		if (inst_state->update_fac_cal_in_registry)
		{
			/* Registry write can be handled in normal mode. */
			//state->island_service->api->sensor_island_exit(state->island_service, this);
			sns_sensor *iter;
			tmd2725_instance_state *inst_state = (tmd2725_instance_state*)instance->state->state;
			for (iter = this->cb->get_library_sensor(this, true);
				NULL != iter;
				iter = this->cb->get_library_sensor(this, false))
			{
				tmd2725_state *sensor_state = (tmd2725_state*)iter->state->state;

				sensor_state->common.fac_cal_scale           = inst_state->lux_scale;
				sensor_state->common.fac_cal_bias            = inst_state->lux_bias;
				sensor_state->common.als_fac_cal_version[0]  = inst_state->als_fac_cal_version[0];
				sensor_state->common.als_fac_cal_version[1]  = inst_state->als_fac_cal_version[1];
				sensor_state->common.near_threshold          = inst_state->near_threshold;
				sensor_state->common.far_threshold           = inst_state->far_threshold;
				sensor_state->common.prox_fac_cal_version[0] = inst_state->prox_fac_cal_version[0];
				sensor_state->common.prox_fac_cal_version[1] = inst_state->prox_fac_cal_version[1];
			}

			tmd2725_write_calibration_to_registry(this);
			inst_state->update_fac_cal_in_registry = false;
			SNS_PRINTF(LOW, this, "TMD2725 set_client_requet update_registry\n");
		}
		this->cb->remove_instance(instance);
		tmd2725_turn_rails_off(this);
	}

	return instance;
}

sns_sensor_api tmd2725_als_sensor_api =
{
	.struct_len         = sizeof(sns_sensor_api),
	.init               = &tmd2725_als_init,
	.deinit             = &tmd2725_als_deinit,
	.get_sensor_uid     = &tmd2725_als_get_sensor_uid,
	.notify_event		= &tmd2725_sensor_notify_event,
	.set_client_request = &tmd2725_set_client_request,
};

sns_sensor_api tmd2725_prox_sensor_api =
{
	.struct_len         = sizeof(sns_sensor_api),
	.init               = &tmd2725_prox_init,
	.deinit             = &tmd2725_prox_deinit,
	.get_sensor_uid     = &tmd2725_prox_get_sensor_uid,
	.notify_event		= &tmd2725_sensor_notify_event,
	.set_client_request = &tmd2725_set_client_request,
};
