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
#include "sns_mem_util.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_types.h"
#include "sns_printf.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_pb_util.h"
#include "sns_suid.pb.h"
#include "sns_registry.pb.h"

#include "sns_tmd2725_sensor.h"

#define TMD2725_CONFIG_ALS              "tmd2725.als.config"
#define TMD2725_CONFIG_PROX             "tmd2725.prox.config"
#define TMD2725_PLATFORM_FAC_CAL_ALS    "tmd2725_platform.als.fac_cal"
#define TMD2725_PLATFORM_FAC_CAL_PROX   "tmd2725_platform.prox.fac_cal"
#define TMD2725_PLATFORM_CONFIG         "tmd2725_platform.config"
#define TMD2725_PLATFORM_COEFFICIENT    "tmd2725_platform.coefficient"

void tmd2725_send_suid_req(sns_sensor *this, char *const data_type, uint32_t data_type_len)
{
	tmd2725_state *state = (tmd2725_state*)this->state->state;
	sns_service_manager *smgr = this->cb->get_service_manager(this);
	sns_stream_service *stream_service = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);
	pb_buffer_arg data = (pb_buffer_arg){.buf = data_type, .buf_len = data_type_len};
	size_t encoded_len;
	uint8_t buffer[50];

	sns_memset(buffer, 0, sizeof(buffer));

	sns_suid_req suid_req = sns_suid_req_init_default;
	suid_req.has_register_updates = true;
	suid_req.register_updates = true;
	suid_req.data_type.funcs.encode = &pb_encode_string_cb;
	suid_req.data_type.arg = &data;

	if (NULL == state->fw_stream)
	{
		stream_service->api->create_sensor_stream(stream_service,
				this, sns_get_suid_lookup(), &state->fw_stream);
	}

	encoded_len = pb_encode_request(buffer, sizeof(buffer), &suid_req, sns_suid_req_fields, NULL);
	if (0 < encoded_len)
	{
		sns_request request = (sns_request){.request_len = encoded_len,
			.request = buffer, .message_id = SNS_SUID_MSGID_SNS_SUID_REQ};
		state->fw_stream->api->send_request(state->fw_stream, &request);
	}
}

void tmd2725_common_init(sns_sensor *const this)
{
	tmd2725_state *state = (tmd2725_state*)this->state->state;

	struct sns_service_manager *smgr= this->cb->get_service_manager(this);
	state->diag_service = (sns_diag_service *)smgr->get_service(smgr, SNS_DIAG_SERVICE);
	state->scp_service = (sns_sync_com_port_service *)smgr->get_service(smgr, SNS_SYNC_COM_PORT_SERVICE);

	sns_memzero(&state->common, sizeof(tmd2725_common_state));

	/* TMD2725 sensor fetches all common dependent sensor SUIDs. */
	if (TMD2725_ALS == state->sensor)
	{
		state->common.start_hw_detect = false;
		tmd2725_send_suid_req(this, "interrupt", sizeof("interrupt"));
		tmd2725_send_suid_req(this, "timer", sizeof("timer"));
	}
	/* Send registry suid request at last */
	tmd2725_send_suid_req(this, "registry", sizeof("registry"));
}

void tmd2725_process_suid_events(sns_sensor *const this)
{
	tmd2725_state *state = (tmd2725_state*)this->state->state;

	for (;
		0 != state->fw_stream->api->get_input_cnt(state->fw_stream);
		state->fw_stream->api->get_next_input(state->fw_stream))
	{
		sns_sensor_event *event = state->fw_stream->api->peek_input(state->fw_stream);

		if (SNS_SUID_MSGID_SNS_SUID_EVENT == event->message_id)
		{
			pb_istream_t stream = pb_istream_from_buffer((void*)event->event, event->event_len);
			sns_suid_event suid_event = sns_suid_event_init_default;
			pb_buffer_arg data_type_arg = {.buf = NULL, .buf_len = 0};
			sns_sensor_uid uid_list;
			sns_suid_search suid_search;
			suid_search.suid = &uid_list;
			suid_search.num_of_suids = 0;

			suid_event.data_type.funcs.decode = &pb_decode_string_cb;
			suid_event.data_type.arg = &data_type_arg;
			suid_event.suid.funcs.decode = &pb_decode_suid_event;
			suid_event.suid.arg = &suid_search;

			if (!pb_decode(&stream, sns_suid_event_fields, &suid_event))
			{
				SNS_PRINTF(ERROR, this, "TMD2725 SUID Decode failed");
				continue;
			}

			/* If no suids found, ignore the event */
			if (suid_search.num_of_suids == 0)
			{
				continue;
			}

			/* Save suid based on incoming data type name */
			if (0 == strncmp(data_type_arg.buf, "interrupt", data_type_arg.buf_len))
			{
				state->common.irq_suid = uid_list;
			}
			else if (0 == strncmp(data_type_arg.buf, "timer", data_type_arg.buf_len))
			{
				state->common.timer_suid = uid_list;
			}
			else if (0 == strncmp(data_type_arg.buf, "registry", data_type_arg.buf_len))
			{
				state->common.reg_suid = uid_list;
			}
			else
			{
				SNS_PRINTF(ERROR, this, "TMD2725 invalid datatype_name");
			}
		}
	}
}

static void tmd2725_sensor_send_registry_request(sns_sensor *const this,
                                                                char *reg_group_name)
{
	tmd2725_state *state = (tmd2725_state*)this->state->state;
	int32_t encoded_len;
	sns_rc rc = SNS_RC_SUCCESS;
	uint8_t buffer[100];
	sns_memset(buffer, 0, sizeof(buffer));

	sns_registry_read_req read_request;
	pb_buffer_arg data = (pb_buffer_arg){.buf = reg_group_name,
		.buf_len = (strlen(reg_group_name) + 1)};

	read_request.name.arg = &data;
	read_request.name.funcs.encode = pb_encode_string_cb;

	encoded_len = pb_encode_request(buffer, sizeof(buffer),
		&read_request, sns_registry_read_req_fields, NULL);
	if (0 < encoded_len)
	{
		sns_request request = (sns_request){.request_len = encoded_len, .request = buffer,
							.message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ};
		rc = state->reg_data_stream->api->send_request(state->reg_data_stream, &request);
	}
}

void tmd2725_request_registry(sns_sensor *const this)
{
	tmd2725_state *state = (tmd2725_state*)this->state->state;
	sns_service_manager *service_mgr = this->cb->get_service_manager(this);
	sns_stream_service *stream_svc =
		(sns_stream_service*)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

	/* Place a request to registry sensor */
	if (NULL == state->reg_data_stream)
	{
		stream_svc->api->create_sensor_stream(stream_svc,
			this, state->common.reg_suid, &state->reg_data_stream);

		if (TMD2725_ALS == state->sensor)
		{
			tmd2725_sensor_send_registry_request(this, TMD2725_PLATFORM_CONFIG);
			tmd2725_sensor_send_registry_request(this, TMD2725_PLATFORM_COEFFICIENT);
			tmd2725_sensor_send_registry_request(this, TMD2725_PLATFORM_FAC_CAL_ALS);
			tmd2725_sensor_send_registry_request(this, TMD2725_CONFIG_ALS);
		}
		else if (TMD2725_PROX == state->sensor)
		{
			tmd2725_sensor_send_registry_request(this, TMD2725_PLATFORM_FAC_CAL_PROX);
			tmd2725_sensor_send_registry_request(this, TMD2725_CONFIG_PROX);
		}
	}
}

/*
 * It extracts each element's version as well as it's value.
 *
 * @return bool True if decoding is successfull else false.
*/
bool tmd2725_custom_parse_registry_float_array(sns_registry_data_item *reg_item,
                                                                pb_buffer_arg  *item_name,
                                                                pb_buffer_arg  *item_str_val,
                                                                void *parsed_buffer)
{
	bool rv = true;
	tmd2725_pb_custom_float_parse_arg *arg = (tmd2725_pb_custom_float_parse_arg*) parsed_buffer;

	if (NULL == reg_item || NULL == item_name || NULL == item_str_val ||
		NULL == parsed_buffer)
	{
		rv = false;
	}
	else if (reg_item->has_flt && arg->size > *arg->index)
	{
		arg->data_array[*arg->index] = reg_item->flt;
		if (reg_item->has_version)
		{
			arg->version_array[*arg->index] = reg_item->version;
		}
		(*arg->index)++;
	}
	else
	{
		rv = false;
	}

	return rv;
}

void tmd2725_sensor_process_registry_event(sns_sensor *const this,
                                                          sns_sensor_event *event)
{
	bool rv = true;
	tmd2725_state *state = (tmd2725_state*)this->state->state;
	sns_sensor *sensor = NULL;

	pb_istream_t stream = pb_istream_from_buffer((void*)event->event, event->event_len);

	if (SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
	{
		sns_registry_read_event read_event = sns_registry_read_event_init_default;
		pb_buffer_arg group_name = {0,0};
		read_event.name.arg = &group_name;
		read_event.name.funcs.decode = pb_decode_string_cb;

		if (!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
		{
			SNS_PRINTF(ERROR, this, "TMD2725 Error decoding registry event");
		}
		else
		{
			stream = pb_istream_from_buffer((void*)event->event, event->event_len);

			if (0 == strncmp((char*)group_name.buf, TMD2725_CONFIG_ALS, group_name.buf_len))
			{
				{
					sns_registry_decode_arg arg = {
						.item_group_name = &group_name,
						.parse_info_len = 1,
						.parse_info[0] = {
							.group_name = "config",
							.parse_func = sns_registry_parse_phy_sensor_cfg,
							.parsed_buffer = &state->common.als_registry_cfg
						}
					};

					read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
					read_event.data.items.arg = &arg;

					rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
				}

				if (rv)
				{
					state->common.als_registry_cfg_received = true;

					SNS_PRINTF(LOW, this, "TMD2725 Registry read event for ALS group registry_cfg received "
						"is_dri:%d, hardware_id:%d resolution_idx:%d, supports_sync_stream:%d",
						state->common.als_registry_cfg.is_dri, state->common.als_registry_cfg.hw_id,
						state->common.als_registry_cfg.res_idx, state->common.als_registry_cfg.sync_stream);
				}
			}
			else if (0 == strncmp((char*)group_name.buf, TMD2725_CONFIG_PROX, group_name.buf_len))
			{
				{
					sns_registry_decode_arg arg = {
						.item_group_name = &group_name,
						.parse_info_len = 1,
						.parse_info[0] = {
							.group_name = "config",
							.parse_func = sns_registry_parse_phy_sensor_cfg,
							.parsed_buffer = &state->common.prox_registry_cfg
						}
					};

					read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
					read_event.data.items.arg = &arg;

					rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
				}

				if (rv)
				{
					tmd2725_state *als_state;
					state->common.prox_registry_cfg_received = true;

					for (sensor = this->cb->get_library_sensor(this, true);
						sensor != NULL;
						sensor = this->cb->get_library_sensor(this, false))
					{
						als_state = (tmd2725_state*)sensor->state->state;

						if (TMD2725_ALS == als_state->sensor)
						{
							als_state->common.prox_registry_cfg_received = true;

							sns_memscpy(&als_state->common.prox_registry_cfg,
								sizeof(sns_registry_phy_sensor_cfg),
								&state->common.prox_registry_cfg,
								sizeof(sns_registry_phy_sensor_cfg));
							break;
						}
					}

					SNS_PRINTF(LOW, this, "TMD2725 Registry read event for Proximity group registry_cfg received "
						"is_dri:%d, hardware_id:%d resolution_idx:%d, supports_sync_stream:%d",
						als_state->common.prox_registry_cfg.is_dri, als_state->common.prox_registry_cfg.hw_id,
						als_state->common.prox_registry_cfg.res_idx, als_state->common.prox_registry_cfg.sync_stream);
				}
			}
			else if (0 == strncmp((char*)group_name.buf, TMD2725_PLATFORM_CONFIG, group_name.buf_len))
			{
				{
					sns_registry_decode_arg arg = {
						.item_group_name = &group_name,
						.parse_info_len = 1,
						.parse_info[0] = {
							.group_name = "config",
							.parse_func = sns_registry_parse_phy_sensor_pf_cfg,
							.parsed_buffer = &state->common.registry_pf_cfg
						}
					};

					read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
					read_event.data.items.arg = &arg;

					rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
				}

				if (rv)
				{
					state->common.registry_pf_cfg_received = true;

					state->common.com_port_info.com_config.bus_type = state->common.registry_pf_cfg.bus_type;
					state->common.com_port_info.com_config.bus_instance = state->common.registry_pf_cfg.bus_instance;
					state->common.com_port_info.com_config.slave_control = state->common.registry_pf_cfg.slave_config;
					state->common.com_port_info.com_config.min_bus_speed_KHz = state->common.registry_pf_cfg.min_bus_speed_khz;
					state->common.com_port_info.com_config.max_bus_speed_KHz = state->common.registry_pf_cfg.max_bus_speed_khz;
					state->common.com_port_info.com_config.reg_addr_type = state->common.registry_pf_cfg.reg_addr_type;
					state->common.irq_config.interrupt_num = state->common.registry_pf_cfg.dri_irq_num;
					state->common.irq_config.interrupt_pull_type = state->common.registry_pf_cfg.irq_pull_type;
					state->common.irq_config.is_chip_pin = state->common.registry_pf_cfg.irq_is_chip_pin;
					state->common.irq_config.interrupt_drive_strength = state->common.registry_pf_cfg.irq_drive_strength;
					state->common.irq_config.interrupt_trigger_type = state->common.registry_pf_cfg.irq_trigger_type;
					state->common.rail_config.num_of_rails = state->common.registry_pf_cfg.num_rail;
					state->common.registry_rail_on_state = state->common.registry_pf_cfg.rail_on_state;
					sns_strlcpy(state->common.rail_config.rails[0].name,
						state->common.registry_pf_cfg.vddio_rail,
						sizeof(state->common.rail_config.rails[0].name));
					sns_strlcpy(state->common.rail_config.rails[1].name,
						state->common.registry_pf_cfg.vdd_rail,
						sizeof(state->common.rail_config.rails[1].name));

					SNS_PRINTF(LOW, this, "TMD2725 Registry read event for group registry_pf_cfg received "
						"bus_type:%d bus_instance:%d slave_control:%d "
						"min_bus_speed_KHz :%d max_bus_speed_KHz:%d reg_addr_type:%d ",
						state->common.com_port_info.com_config.bus_type,
						state->common.com_port_info.com_config.bus_instance,
						state->common.com_port_info.com_config.slave_control,
						state->common.com_port_info.com_config.min_bus_speed_KHz,
						state->common.com_port_info.com_config.max_bus_speed_KHz,
						state->common.com_port_info.com_config.reg_addr_type);

					SNS_PRINTF(LOW, this, "TMD2725 interrupt_num:%d interrupt_pull_type:%d is_chip_pin:%d "
						"interrupt_drive_strength:%d interrupt_trigger_type:%d rigid body type:%d",
						state->common.irq_config.interrupt_num,
						state->common.irq_config.interrupt_pull_type,
						state->common.irq_config.is_chip_pin,
						state->common.irq_config.interrupt_drive_strength,
						state->common.irq_config.interrupt_trigger_type,
						state->common.registry_pf_cfg.rigid_body_type);
				}
			}
			else if (0 == strncmp((char*)group_name.buf, TMD2725_PLATFORM_FAC_CAL_ALS, group_name.buf_len))
			{
				float parsed_float_array[2] = {0};
				uint32_t version_array[2] = {0};
				uint8_t arr_index = 0;
				tmd2725_pb_custom_float_parse_arg arr_arg = {
					.data_array = parsed_float_array,
					.version_array = version_array,
					.index = &arr_index,
					.size = 2
				};

				sns_registry_decode_arg arg = {
					.item_group_name = &group_name,
					.parse_info_len = 1,
					.parse_info[0] = {
						.group_name = TMD2725_PLATFORM_FAC_CAL_ALS,
						.parse_func = tmd2725_custom_parse_registry_float_array,
						.parsed_buffer = &arr_arg
					}
				};

				read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
				read_event.data.items.arg = &arg;

				rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);

				if (rv)
				{
					state->registry_fac_cal_received = true;
					state->common.fac_cal_scale = parsed_float_array[0];
					state->common.fac_cal_bias = parsed_float_array[1];
					state->common.als_fac_cal_version[0] = version_array[0];
					state->common.als_fac_cal_version[1] = version_array[1];

					SNS_PRINTF(LOW, this, "TMD2725 Registry read fac_cal scale:%d bias:%d",
						(uint32_t)state->common.fac_cal_scale, (uint32_t)state->common.fac_cal_bias);
				}
			}
			else if (0 == strncmp((char*)group_name.buf, TMD2725_PLATFORM_FAC_CAL_PROX, group_name.buf_len))
			{
				float parsed_float_array[2] = {0};
				uint32_t version_array[2] = {0};
				uint8_t arr_index = 0;
				tmd2725_pb_custom_float_parse_arg arr_arg = {
					.data_array = parsed_float_array,
					.version_array = version_array,
					.index = &arr_index,
					.size = 2
				};

				sns_registry_decode_arg arg = {
					.item_group_name = &group_name,
					.parse_info_len = 1,
					.parse_info[0] = {
						.group_name = TMD2725_PLATFORM_FAC_CAL_PROX,
						.parse_func = tmd2725_custom_parse_registry_float_array,
						.parsed_buffer = &arr_arg
					}
				};

				read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
				read_event.data.items.arg = &arg;

				rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);

				if (rv)
				{
					tmd2725_state *als_state;
					state->registry_fac_cal_received = true;
					state->common.near_threshold = parsed_float_array[0];
					state->common.far_threshold = parsed_float_array[1];
					state->common.prox_fac_cal_version[0] = version_array[0];
					state->common.prox_fac_cal_version[1] = version_array[1];

					for (sensor = this->cb->get_library_sensor(this, true);
						sensor != NULL;
						sensor = this->cb->get_library_sensor(this, false))
					{
						als_state = (tmd2725_state*)sensor->state->state;
					
						if (TMD2725_ALS == als_state->sensor)
						{
							als_state->common.near_threshold = state->common.near_threshold;
							als_state->common.far_threshold = state->common.far_threshold;
							als_state->common.prox_fac_cal_version[0] = state->common.prox_fac_cal_version[0];
							als_state->common.prox_fac_cal_version[1] = state->common.prox_fac_cal_version[1];
							break;
						}
					}
					
					SNS_PRINTF(LOW, this, "TMD2725 Registry read fac_cal near_thresh:%d far_thresh:%d",
						(uint32_t)state->common.near_threshold, (uint32_t)state->common.far_threshold);
				}
			}
			else if (0 == strncmp((char*)group_name.buf, TMD2725_PLATFORM_COEFFICIENT, group_name.buf_len))
			{
				{
					uint8_t arr_index = 0;
					pb_float_arr_arg arr_arg = {
						.arr = state->common.coefficient,
						.arr_index = &arr_index,
						.arr_len = 12
					};

					sns_registry_decode_arg arg = {
						.item_group_name = &group_name,
						.parse_info_len = 1,
						.parse_info[0] = {
							.group_name = "coefficient",
							.parse_func = sns_registry_parse_float_arr,
							.parsed_buffer = &arr_arg
						}
					};

					read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
					read_event.data.items.arg = &arg;

					rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
				}

				if (rv)
				{
					state->common.registry_coefficient_received = true;

					SNS_PRINTF(LOW, this, "TMD2725 Registry read event for group registry_coefficient received "
						"c[0]:%u c[4]:%u c[10]:%u", (uint32_t)state->common.coefficient[0],
						(uint32_t)state->common.coefficient[4], (uint32_t)state->common.coefficient[10]);
				}
			}
			else
			{
				rv = false;
			}

			if (!rv)
			{
				SNS_PRINTF(ERROR, this, "TMD2725 Error decoding registry group");
			}
		}
	}
	else
	{
		SNS_PRINTF(ERROR, this, "TMD2725 Received unsupported registry event msg id %u", event->message_id);
	}
}

static bool tmd2725_write_to_prox_fac_cal(struct pb_ostream_s *stream,
                                 struct pb_field_s const *field, void *const *arg)
{
	sns_sensor *this = (sns_sensor*) *arg;
	tmd2725_state *state = (tmd2725_state *)this->state->state;
	char const *names[] = {"near_threshold" , "far_threshold"};

	for (int i = 0; i < sizeof(names)/sizeof(names[0]); i++)
	{
		pb_buffer_arg name_data = (pb_buffer_arg)
			{.buf = names[i], .buf_len = strlen(names[i]) + 1};
		sns_registry_data_item pb_item = sns_registry_data_item_init_default;

		pb_item.name.funcs.encode = &pb_encode_string_cb;
		pb_item.name.arg = &name_data;

		if (0 == strncmp(names[i], "near_threshold", sizeof("near_threshold")))
		{
			pb_item.has_version = true;
			pb_item.version = state->common.prox_fac_cal_version[0];
			pb_item.has_flt = true;
			pb_item.flt = (float)state->common.near_threshold;
		}
		else if (0 == strncmp(names[i], "far_threshold", sizeof("far_threshold")))
		{
			pb_item.has_version = true;
			pb_item.version = state->common.prox_fac_cal_version[1];
			pb_item.has_flt = true;
			pb_item.flt = (float)state->common.far_threshold;
		}
		if (!pb_encode_tag_for_field(stream, field))
		{
			SNS_PRINTF(ERROR, this, "TMD2725Error encoding tag item: %d", i);
			return false;
		}

		if (!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
		{
			SNS_PRINTF(ERROR, this, "TMD2725Error encoding item: %d", i);
			return false;
		}
	}
	SNS_PRINTF(HIGH, this, "TMD2725 sns_ams_encode_calibration_registry_cb: done");
	return true;
}

static bool tmd2725_write_to_als_fac_cal(struct pb_ostream_s *stream,
	                            struct pb_field_s const *field, void *const *arg)
{
	sns_sensor *this = (sns_sensor*) *arg;
	tmd2725_state *state = (tmd2725_state *)this->state->state;
	char const *names[] = {"scale"};

	for (int i = 0; i < sizeof(names)/sizeof(names[0]); i++)
	{
		pb_buffer_arg name_data = (pb_buffer_arg)
			{.buf = names[i], .buf_len = strlen(names[i]) + 1};
		sns_registry_data_item pb_item = sns_registry_data_item_init_default;

		pb_item.name.funcs.encode = &pb_encode_string_cb;
		pb_item.name.arg = &name_data;

		if (0 == strncmp(names[i], "scale", sizeof("scale")))
		{
			pb_item.has_version = true;
			pb_item.version = state->common.als_fac_cal_version[0];
			pb_item.has_flt = true;
			pb_item.flt = (float)state->common.fac_cal_scale;
		}
		if (!pb_encode_tag_for_field(stream, field))
		{
			SNS_PRINTF(ERROR, this, "TMD2725 Error encoding tag item: %d", i);
			return false;
		}

		if (!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
		{
			SNS_PRINTF(ERROR, this, "TMD2725 Error encoding item: %d", i);
			return false;
		}
	}
	SNS_PRINTF(HIGH, this, "TMD2725 sns_ams_encode_calibration_registry_cb: done");
	return true;
}

bool tmd2725_write_calibration_to_registry(sns_sensor *const this)
{
	uint8_t buffer[200];
	int32_t encoded_len;
	tmd2725_state *state =  (tmd2725_state *) this->state->state;
	sns_registry_write_req write_req = sns_registry_write_req_init_default;

	const char *names[] = { TMD2725_PLATFORM_FAC_CAL_ALS,
							TMD2725_PLATFORM_FAC_CAL_PROX };

	for (int i = 0 ; i < sizeof(names)/sizeof(names[0]) ; i++)
	{
		pb_buffer_arg name_data = (pb_buffer_arg)
			{.buf = names[i],
			.buf_len = strlen (names[i]) + 1};

		write_req.name.funcs.encode = &pb_encode_string_cb;
		write_req.name.arg = &name_data;
		write_req.data.items.arg = this;

		if (0 == strncmp(names[i], 	TMD2725_PLATFORM_FAC_CAL_ALS, sizeof(TMD2725_PLATFORM_FAC_CAL_ALS)))
		{
			write_req.data.items.funcs.encode = tmd2725_write_to_als_fac_cal;
		}
		else if (0 == strncmp(names[i], TMD2725_PLATFORM_FAC_CAL_PROX, sizeof(TMD2725_PLATFORM_FAC_CAL_PROX)))
		{
			write_req.data.items.funcs.encode = tmd2725_write_to_prox_fac_cal;
		}

		encoded_len = pb_encode_request(buffer, sizeof(buffer),
			&write_req, sns_registry_write_req_fields, NULL);
		if (0 < encoded_len)
		{
			if (NULL == state->reg_data_stream)
			{
				sns_service_manager *smgr = this->cb->get_service_manager(this);
				sns_stream_service *stream_svc = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);

				stream_svc->api->create_sensor_stream(stream_svc, this,
					state->common.reg_suid, &state->reg_data_stream);
			}

			sns_request request = (sns_request){
				.request_len = encoded_len, .request = buffer,
				.message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ };
			state->reg_data_stream->api->send_request(state->reg_data_stream, &request);
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool tmd2725_discover_hw(sns_sensor *const this)
{
	tmd2725_state *state = (tmd2725_state*)this->state->state;
	uint8_t buffer[1] = {0};
	bool hw_is_present = false;
	sns_rc rv = SNS_RC_SUCCESS;

	/* Read and Confirm WHO-AM-I */
	rv = tmd2725_get_who_am_i(state->scp_service,
							state->common.com_port_info.port_handle,
							&buffer[0]);
	SNS_PRINTF(HIGH, this, "TMD2725 tmd2725_discover_hw who am i 0x%x", buffer[0]);

	if (SNS_RC_SUCCESS == rv && (buffer[0]&TMD2725_PARTNO_MASK) == TMD2725_WHOAMI_VALUE)
	{
		hw_is_present = true;
	}

	state->common.who_am_i = buffer[0];

	/* Power Down and Close COM Port */
	state->scp_service->api->sns_scp_update_bus_power(
		state->common.com_port_info.port_handle, false);

	state->scp_service->api->sns_scp_close(state->common.com_port_info.port_handle);
	state->scp_service->api->sns_scp_deregister_com_port(&state->common.com_port_info.port_handle);

	/* Turn Power Rail OFF */
	state->common.rail_config.rail_vote = SNS_RAIL_OFF;
	state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
		this,
		&state->common.rail_config,
		NULL);

	return hw_is_present;
}

/**
 * Publish AVAILABLE attributes
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
void tmd2725_publish_available(sns_sensor *const this)
{
	sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
	value.has_boolean = true;
	value.boolean = true;

	sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_AVAILABLE,
		&value, 1, true);
}

/**
 * Publish attributes read from registry
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
static void tmd2725_publish_registry_attributes(sns_sensor *const this)
{
	tmd2725_state *state = (tmd2725_state*)this->state->state;
	tmd2725_sensor_type sensor_type = state->sensor;

	if ((TMD2725_ALS == sensor_type && !state->common.als_registry_cfg.is_dri) ||
		(TMD2725_PROX == sensor_type && !state->common.prox_registry_cfg.is_dri))
	{
		sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR,
			SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
		values[0].has_flt = true;
		values[0].flt = TMD2725_ODR_2;
		values[1].has_flt = true;
		values[1].flt = TMD2725_ODR_5;
		values[2].has_flt = true;
		values[2].flt = TMD2725_ODR_10;
		values[3].has_flt = true;
		values[3].flt = TMD2725_ODR_15;
		values[4].has_flt = true;
		values[4].flt = TMD2725_ODR_20;
		values[5].has_flt = true;
		values[5].flt = TMD2725_ODR_50;
		values[6].has_flt = true;
		values[6].flt = TMD2725_ODR_100;

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RATES,
			values, ARR_SIZE(values), false);
	}
	{
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.has_boolean = true;
		if ((TMD2725_ALS == sensor_type && state->common.als_registry_cfg.is_dri) ||
			(TMD2725_PROX == sensor_type && state->common.prox_registry_cfg.is_dri))
		{
			value.boolean = true;
		}
		else
		{
			value.boolean = false;
		}

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_DRI,
			&value, 1, false);
	}
	{
		sns_std_attr_value_data values[] = {SNS_ATTR};
		values[0].has_sint = true;
		if ((TMD2725_ALS == sensor_type && state->common.als_registry_cfg.is_dri) ||
			(TMD2725_PROX == sensor_type && state->common.prox_registry_cfg.is_dri))
		{
			values[0].sint = SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE;
		}
		else
		{
			values[0].sint = SNS_STD_SENSOR_STREAM_TYPE_STREAMING;
		}

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE,
			values, ARR_SIZE(values), false);
	}
	{
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.has_sint = true;
		if (TMD2725_ALS == sensor_type)
		{
			value.sint = state->common.als_registry_cfg.hw_id;
		}
		else if (TMD2725_PROX == sensor_type)
		{
			value.sint = state->common.prox_registry_cfg.hw_id;
		}
		else
		{
			value.sint = 0;
		}

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_HW_ID,
			&value, 1, false);
	}
}

static sns_rc tmd2725_register_power_rail(sns_sensor *const this)
{
	tmd2725_state *state = (tmd2725_state*)this->state->state;
	sns_service_manager *smgr = this->cb->get_service_manager(this);
	sns_rc rv = SNS_RC_SUCCESS;

	state->common.rail_config.rail_vote = SNS_RAIL_OFF;

	if (NULL == state->pwr_rail_service)
	{
		state->pwr_rail_service =
			(sns_pwr_rail_service*)smgr->get_service(smgr, SNS_POWER_RAIL_SERVICE);

		state->pwr_rail_service->api->sns_register_power_rails(state->pwr_rail_service,
			&state->common.rail_config);
	}

	if (NULL == state->pwr_rail_service)
	{
		rv = SNS_RC_FAILED;
	}
	return rv;
}

void tmd2725_update_sibling_sensors(sns_sensor *const this)
{
	sns_sensor *sensor = NULL;
	tmd2725_state *state;
	tmd2725_state *als_state = (tmd2725_state*)this->state->state;

	for (sensor = this->cb->get_library_sensor(this, true);
		NULL != sensor;
		sensor = this->cb->get_library_sensor(this, false))
	{
		state = (tmd2725_state*)sensor->state->state;

		if (TMD2725_ALS != state->sensor)
		{
			sns_memscpy(&state->common, sizeof(state->common),
				&als_state->common, sizeof(als_state->common));
			tmd2725_register_power_rail(sensor);
			tmd2725_publish_available(sensor);
		}

		/* Moving registry based attribute publish here. */
		tmd2725_publish_registry_attributes(sensor);

		/* More clean up. */
		sns_sensor_util_remove_sensor_stream(sensor, &state->reg_data_stream);
	}
}

void tmd2725_start_hw_detect_sequence(sns_sensor *const this)
{
	tmd2725_state *state = (tmd2725_state*)this->state->state;
	sns_rc rv = SNS_RC_SUCCESS;

	/* Use thi flag to avoid doing hw detect too many times */
	state->common.start_hw_detect = true;

	/* Register and Open COM Port */
	if (NULL == state->common.com_port_info.port_handle)
	{
		rv = state->scp_service->api->sns_scp_register_com_port(
			&state->common.com_port_info.com_config,
			&state->common.com_port_info.port_handle);

		if (SNS_RC_SUCCESS == rv)
		{
			rv = state->scp_service->api->sns_scp_open(state->common.com_port_info.port_handle);
		}
	}

	/* Register Power Rails */
	if (!sns_sensor_uid_compare(&state->common.timer_suid, &((sns_sensor_uid){{0}}))
		&& NULL == state->pwr_rail_service
		&& SNS_RC_SUCCESS == rv)
	{
		rv = tmd2725_register_power_rail(this);

		/* Turn Power Rails ON */
		state->common.rail_config.rail_vote = state->common.registry_rail_on_state;

		if (SNS_RC_SUCCESS == rv)
		{
			rv = state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
				this,
				&state->common.rail_config,
				NULL);
		}

		/* Create a Timer stream for Power Rail ON timeout */
		if (SNS_RC_SUCCESS == rv)
		{
			tmd2725_start_power_rail_timer(this,
				sns_convert_ns_to_ticks(TMD2725_OFF_TO_IDLE_MS * 1000 * 1000),
				TMD2725_POWER_RAIL_PENDING_INIT);
		}
	}
}
